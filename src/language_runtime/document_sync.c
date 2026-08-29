/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/document_sync.c
 *
 * PURPOSE:
 *   Implement LSP document synchronization and mirror metadata into UmiLanguageDocumentRegistry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/document_sync.h"
#include "umicom/base/text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiLanguageRuntimeDocumentSync {
  UmiLanguageService *l;
  UmiLanguageRuntimeDocumentState d[UMI_LANGUAGE_RUNTIME_MAX_DOCUMENTS];
  size_t n;
};
static size_t lines(const char *t) {
  size_t n = 1;
  if (!t)
    return 1;
  for (; *t; t++)
    if (*t == '\n')
      n++;
  return n;
}
static size_t find(const UmiLanguageRuntimeDocumentSync *s, const char *d) {
  size_t i;
  if (!s || !d)
    return (size_t)-1;
  for (i = 0; i < s->n; i++)
    if (strcmp(s->d[i].document_id, d) == 0)
      return i;
  return (size_t)-1;
}
static UmiStatus pub(UmiLanguageRuntimeDocumentSync *s, const UmiLanguageRuntimeDocumentState *d) {
  UmiLanguageDocumentSnapshot x = {0};
  x.struct_size = sizeof(x);
  x.api_version = UMI_LANGUAGE_DOCUMENT_API_VERSION;
  (void)umi_text_format(x.id, sizeof(x.id), "%s", d->document_id);
  (void)umi_text_format(x.uri, sizeof(x.uri), "%s", d->uri);
  (void)umi_text_format(x.language_id, sizeof(x.language_id), "%s", d->language_id);
  x.version = d->version;
  x.line_count = d->line_count;
  x.open = d->open;
  x.dirty = d->dirty;
  x.revision = d->version;
  return umi_language_document_registry_upsert(umi_language_service_document(s->l), &x);
}
UmiStatus umi_language_runtime_document_sync_create(UmiLanguageService *l,
                                                    UmiLanguageRuntimeDocumentSync **out) {
  UmiLanguageRuntimeDocumentSync *s;
  if (!l || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  s = calloc(1, sizeof(*s));
  if (!s)
    return UMI_STATUS_OUT_OF_MEMORY;
  s->l = l;
  *out = s;
  return UMI_STATUS_OK;
}
void umi_language_runtime_document_sync_destroy(UmiLanguageRuntimeDocumentSync *s) { free(s); }
UmiStatus umi_language_runtime_document_open(UmiLanguageRuntimeDocumentSync *s,
                                             UmiLanguageRuntimeServer *server, const char *d,
                                             const char *uri, const char *lang, const char *text) {
  UmiLanguageRuntimeDocumentState *x;
  char n[128];
  UmiStatus q;
  if (!s || !server || !d || !*d || !uri || !*uri || !lang || !text)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (find(s, d) != (size_t)-1)
    return UMI_STATUS_ALREADY_EXISTS;
  if (s->n >= UMI_LANGUAGE_RUNTIME_MAX_DOCUMENTS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  q = umi_language_runtime_normalize_language_id(lang, n, sizeof(n));
  if (q != UMI_STATUS_OK)
    return q;
  x = &s->d[s->n];
  memset(x, 0, sizeof(*x));
  (void)umi_text_format(x->document_id, sizeof(x->document_id), "%s", d);
  (void)umi_text_format(x->uri, sizeof(x->uri), "%s", uri);
  (void)umi_text_format(x->language_id, sizeof(x->language_id), "%s", n);
  x->version = 1;
  x->line_count = lines(text);
  x->open = 1;
  x->server = server;
  q = umi_language_runtime_request_did_open(server, uri, n, 1, text);
  if (q == UMI_STATUS_OK)
    q = pub(s, x);
  if (q != UMI_STATUS_OK) {
    memset(x, 0, sizeof(*x));
    return q;
  }
  s->n++;
  return UMI_STATUS_OK;
}
UmiStatus umi_language_runtime_document_change(UmiLanguageRuntimeDocumentSync *s, const char *d,
                                               const char *text) {
  size_t i = find(s, d);
  UmiLanguageRuntimeDocumentState *x;
  UmiStatus q;
  if (!s || !d || !text)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (i == (size_t)-1)
    return UMI_STATUS_NOT_FOUND;
  x = &s->d[i];
  x->version++;
  x->line_count = lines(text);
  x->dirty = 1;
  q = umi_language_runtime_request_did_change(x->server, x->uri, x->version, text);
  return q == UMI_STATUS_OK ? pub(s, x) : q;
}
UmiStatus umi_language_runtime_document_save(UmiLanguageRuntimeDocumentSync *s, const char *d,
                                             const char *text) {
  size_t i = find(s, d);
  UmiLanguageRuntimeDocumentState *x;
  UmiStatus q;
  if (!s || !d)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (i == (size_t)-1)
    return UMI_STATUS_NOT_FOUND;
  x = &s->d[i];
  q = umi_language_runtime_request_did_save(x->server, x->uri, text);
  if (q != UMI_STATUS_OK)
    return q;
  x->dirty = 0;
  return pub(s, x);
}
UmiStatus umi_language_runtime_document_close(UmiLanguageRuntimeDocumentSync *s, const char *d) {
  size_t i = find(s, d), tail;
  UmiStatus q;
  if (!s || !d)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (i == (size_t)-1)
    return UMI_STATUS_NOT_FOUND;
  q = umi_language_runtime_request_did_close(s->d[i].server, s->d[i].uri);
  if (q != UMI_STATUS_OK)
    return q;
  (void)umi_language_document_registry_remove(umi_language_service_document(s->l), d);
  tail = s->n - i - 1;
  if (tail)
    memmove(&s->d[i], &s->d[i + 1], tail * sizeof(s->d[0]));
  s->n--;
  memset(&s->d[s->n], 0, sizeof(s->d[0]));
  return UMI_STATUS_OK;
}
UmiStatus umi_language_runtime_document_find(const UmiLanguageRuntimeDocumentSync *s, const char *d,
                                             UmiLanguageRuntimeDocumentState *out) {
  size_t i;
  if (!s || !d || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  i = find(s, d);
  if (i == (size_t)-1)
    return UMI_STATUS_NOT_FOUND;
  *out = s->d[i];
  return UMI_STATUS_OK;
}
UmiStatus umi_language_runtime_document_find_by_uri(const UmiLanguageRuntimeDocumentSync *s,
                                                    const char *uri,
                                                    UmiLanguageRuntimeDocumentState *out) {
  size_t i;
  if (!s || !uri || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (i = 0; i < s->n; i++)
    if (strcmp(s->d[i].uri, uri) == 0) {
      *out = s->d[i];
      return UMI_STATUS_OK;
    }
  return UMI_STATUS_NOT_FOUND;
}
size_t umi_language_runtime_document_sync_count(const UmiLanguageRuntimeDocumentSync *s) {
  return s ? s->n : 0;
}
