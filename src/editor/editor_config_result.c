/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editor_config_result.c
 *
 * PURPOSE:
 *   Resolve ordered project configuration layers with property provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/editor_config_result.h"
#include "editor_config_internal.h"
#include "umicom/editor/editor_config_pattern.h"
#include <stdlib.h>
#include <string.h>
typedef struct Property {
  char *key, *value, *source, *pattern;
  size_t line;
} Property;
struct UmiEditorConfigResult {
  Property *items;
  size_t count;
};
static void Release(Property *p) {
  free(p->key);
  free(p->value);
  free(p->source);
  free(p->pattern);
  memset(p, 0, sizeof *p);
}
void UmiEditorConfigResultDestroy(UmiEditorConfigResult *r) {
  if (r) {
    for (size_t i = 0; i < r->count; ++i)
      Release(&r->items[i]);
    free(r->items);
    free(r);
  }
}
static char *Duplicate(const char *s) {
  size_t n = strlen(s) + 1U;
  char *p = malloc(n);
  if (p)
    memcpy(p, s, n);
  return p;
}
static UmiStatus Assign(UmiEditorConfigResult *r, const ConfigPair *pair, const char *source,
                        const char *pattern) {
  size_t at = 0;
  while (at < r->count && strcmp(r->items[at].key, pair->key))
    ++at;
  if (UmiEditorConfigEqualFolded(pair->value, "unset")) {
    if (at < r->count) {
      Release(&r->items[at]);
      memmove(r->items + at, r->items + at + 1U, (r->count - at - 1U) * sizeof *r->items);
      --r->count;
      memset(&r->items[r->count], 0, sizeof *r->items);
    }
    return UMI_STATUS_OK;
  }
  if (at == r->count && r->count == UMI_EDITOR_CONFIG_MAXIMUM_PAIRS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  Property prepared = {Duplicate(pair->key), Duplicate(pair->value), Duplicate(source),
                       Duplicate(pattern), pair->line};
  if (!prepared.key || !prepared.value || !prepared.source || !prepared.pattern) {
    Release(&prepared);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  if (at < r->count)
    Release(&r->items[at]);
  else
    ++r->count;
  r->items[at] = prepared;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorConfigResolve(const UmiEditorConfigLayer *layers, size_t count,
                                 UmiEditorConfigResult **out) {
  if (out)
    *out = NULL;
  if (out == NULL || (count && layers == NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (count > UMI_EDITOR_CONFIG_MAXIMUM_LAYERS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  size_t start = 0;
  for (size_t i = 0; i < count; ++i) {
    if (!layers[i].config || !layers[i].source_name || !layers[i].relative_path)
      return UMI_STATUS_INVALID_ARGUMENT;
    size_t n = 0;
    while (n <= UMI_EDITOR_CONFIG_MAXIMUM_SOURCE_NAME && layers[i].source_name[n])
      ++n;
    if (n > UMI_EDITOR_CONFIG_MAXIMUM_SOURCE_NAME)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    if (n == 0U || UmiEditorConfigTextValidate(layers[i].source_name, n) != UMI_STATUS_OK)
      return UMI_STATUS_INVALID_ARGUMENT;
    if (layers[i].config->root)
      start = i;
  }
  UmiEditorConfigResult *r = calloc(1U, sizeof *r);
  if (!r)
    return UMI_STATUS_OUT_OF_MEMORY;
  r->items = calloc(UMI_EDITOR_CONFIG_MAXIMUM_PAIRS, sizeof *r->items);
  if (!r->items) {
    free(r);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  UmiStatus status = UMI_STATUS_OK;
  for (size_t i = start; i < count && status == UMI_STATUS_OK; ++i) {
    const UmiEditorConfig *c = layers[i].config;
    int matches[UMI_EDITOR_CONFIG_MAXIMUM_SECTIONS] = {0};
    /* Validate the target path even for an empty configuration. Evaluate
     * every section before applying its pairs: no partial success leaks. */
    int ignored = 0;
    status = UmiEditorConfigPatternMatch("*", layers[i].relative_path, &ignored);
    for (size_t j = 0; j < c->section_count && status == UMI_STATUS_OK; ++j)
      status =
          UmiEditorConfigPatternMatch(c->sections[j].pattern, layers[i].relative_path, &matches[j]);
    for (size_t j = 0; j < c->pair_count && status == UMI_STATUS_OK; ++j) {
      const ConfigPair *pair = &c->pairs[j];
      if (pair->section != SIZE_MAX && matches[pair->section])
        status = Assign(r, pair, layers[i].source_name, c->sections[pair->section].pattern);
    }
  }
  if (status == UMI_STATUS_OK)
    *out = r;
  else
    UmiEditorConfigResultDestroy(r);
  return status;
}
size_t UmiEditorConfigResultCount(const UmiEditorConfigResult *r) { return r ? r->count : 0U; }
UmiStatus UmiEditorConfigResultAt(const UmiEditorConfigResult *r, size_t i,
                                  UmiEditorConfigProperty *out) {
  if (!r || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (i >= r->count)
    return UMI_STATUS_NOT_FOUND;
  const Property *p = &r->items[i];
  *out = (UmiEditorConfigProperty){p->key, p->value, p->source, p->pattern, p->line};
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorConfigResultFind(const UmiEditorConfigResult *r, const char *key,
                                    UmiEditorConfigProperty *out) {
  if (!r || !key || !out || !*key)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0; i < r->count; ++i)
    if (UmiEditorConfigEqualFolded(r->items[i].key, key))
      return UmiEditorConfigResultAt(r, i, out);
  return UMI_STATUS_NOT_FOUND;
}
