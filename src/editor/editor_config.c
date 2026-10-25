/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editor_config.c
 *
 * PURPOSE:
 *   Read and inspect owned project editor configuration without modifying settings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "editor_config_internal.h"
#include <stdlib.h>
#include <string.h>
static int Space(char c) { return c == ' ' || c == '\t' || c == '\v' || c == '\f'; }
static char *Copy(const char *text, size_t n, int fold) {
  char *v = malloc(n + 1U);
  if (v == NULL)
    return NULL;
  memcpy(v, text, n);
  v[n] = 0;
  if (fold)
    for (size_t i = 0; i < n; ++i)
      if (v[i] >= 'A' && v[i] <= 'Z')
        v[i] = (char)(v[i] - 'A' + 'a');
  return v;
}
void UmiEditorConfigDestroy(UmiEditorConfig *c) {
  if (c == NULL)
    return;
  for (size_t i = 0; i < c->section_count; ++i)
    free(c->sections[i].pattern);
  for (size_t i = 0; i < c->pair_count; ++i) {
    free(c->pairs[i].key);
    free(c->pairs[i].value);
  }
  free(c->sections);
  free(c->pairs);
  free(c);
}
UmiStatus UmiEditorConfigParse(const char *source, size_t bytes, UmiEditorConfig **out,
                               UmiEditorConfigDiagnostic *diagnostic) {
  if (out)
    *out = NULL;
  UmiStatus status = UMI_STATUS_OK;
  size_t line = 0U;
  if (out == NULL || source == NULL)
    status = UMI_STATUS_INVALID_ARGUMENT;
  else if (bytes > UMI_EDITOR_CONFIG_MAXIMUM_BYTES)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  else
    status = UmiEditorConfigTextValidate(source, bytes);
  UmiEditorConfig *c = NULL;
  if (status != UMI_STATUS_OK)
    goto done;
  c = calloc(1U, sizeof *c);
  if (c == NULL) {
    status = UMI_STATUS_OUT_OF_MEMORY;
    goto done;
  }
  c->sections = calloc(UMI_EDITOR_CONFIG_MAXIMUM_SECTIONS, sizeof *c->sections);
  c->pairs = calloc(UMI_EDITOR_CONFIG_MAXIMUM_PAIRS, sizeof *c->pairs);
  if (c->sections == NULL || c->pairs == NULL) {
    status = UMI_STATUS_OUT_OF_MEMORY;
    goto done;
  }
  size_t offset = 0U, section = SIZE_MAX;
  /* The UTF-8 signature is accepted only at the start, never stripped from
   * values. Every source byte belongs to an explicit bounded line. */
  if (bytes >= 3U && !memcmp(source, "\xef\xbb\xbf", 3U))
    offset = 3U;
  while (offset < bytes) {
    ++line;
    size_t start = offset, end = offset;
    while (end < bytes && source[end] != '\n')
      ++end;
    offset = end < bytes ? end + 1U : end;
    if (end < bytes && end > start && source[end - 1U] == '\r')
      --end;
    if (memchr(source + start, '\r', end - start)) {
      status = UMI_STATUS_PARSE_ERROR;
      goto done;
    }
    while (start < end && Space(source[start]))
      ++start;
    while (end > start && Space(source[end - 1U]))
      --end;
    if (start == end || source[start] == '#' || source[start] == ';')
      continue;
    if (source[start] == '[' && source[end - 1U] == ']') {
      size_t n = end - start - 2U;
      if (n == 0U) {
        status = UMI_STATUS_PARSE_ERROR;
        goto done;
      }
      if (n > UMI_EDITOR_CONFIG_MAXIMUM_PATTERN ||
          c->section_count == UMI_EDITOR_CONFIG_MAXIMUM_SECTIONS) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
      }
      char *p = Copy(source + start + 1U, n, 0);
      if (p == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
      }
      section = c->section_count++;
      c->sections[section] = (ConfigSection){p, line};
      continue;
    }
    size_t eq = start;
    while (eq < end && source[eq] != '=')
      ++eq;
    if (eq == end) {
      status = UMI_STATUS_PARSE_ERROR;
      goto done;
    }
    size_t keyEnd = eq, valueStart = eq + 1U;
    while (keyEnd > start && Space(source[keyEnd - 1U]))
      --keyEnd;
    while (valueStart < end && Space(source[valueStart]))
      ++valueStart;
    size_t kn = keyEnd - start, vn = end - valueStart;
    if (kn == 0U) {
      status = UMI_STATUS_PARSE_ERROR;
      goto done;
    }
    if (kn > UMI_EDITOR_CONFIG_MAXIMUM_KEY || vn > UMI_EDITOR_CONFIG_MAXIMUM_VALUE ||
        c->pair_count == UMI_EDITOR_CONFIG_MAXIMUM_PAIRS) {
      status = UMI_STATUS_CAPACITY_EXCEEDED;
      goto done;
    }
    ConfigPair *pair = &c->pairs[c->pair_count++];
    pair->key = Copy(source + start, kn, 1);
    pair->value = Copy(source + valueStart, vn, 0);
    pair->line = line;
    pair->section = section;
    if (pair->key == NULL || pair->value == NULL) {
      status = UMI_STATUS_OUT_OF_MEMORY;
      goto done;
    }
    if (section == SIZE_MAX && !strcmp(pair->key, "root"))
      c->root = UmiEditorConfigEqualFolded(pair->value, "true");
  }
  *out = c;
  c = NULL;
done:
  UmiEditorConfigDestroy(c);
  if (diagnostic)
    *diagnostic = (UmiEditorConfigDiagnostic){status == UMI_STATUS_OK ? 0U : line, status};
  return status;
}
int UmiEditorConfigIsRoot(const UmiEditorConfig *c) { return c != NULL && c->root; }
size_t UmiEditorConfigSectionCount(const UmiEditorConfig *c) { return c ? c->section_count : 0U; }
size_t UmiEditorConfigPairCount(const UmiEditorConfig *c) { return c ? c->pair_count : 0U; }
UmiStatus UmiEditorConfigSectionAt(const UmiEditorConfig *c, size_t index, const char **out,
                                   size_t *line) {
  if (c == NULL || out == NULL || line == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= c->section_count)
    return UMI_STATUS_NOT_FOUND;
  *out = c->sections[index].pattern;
  *line = c->sections[index].line;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorConfigPairAt(const UmiEditorConfig *c, size_t index, UmiEditorConfigPair *out) {
  if (c == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= c->pair_count)
    return UMI_STATUS_NOT_FOUND;
  const ConfigPair *p = &c->pairs[index];
  *out = (UmiEditorConfigPair){p->key, p->value, p->line, p->section};
  return UMI_STATUS_OK;
}
