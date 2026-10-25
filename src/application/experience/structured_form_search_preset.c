/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form_search_preset.c
 *
 * PURPOSE:
 *   Presets store query intent; pagination and authorization are deliberately fresh.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Presets store query intent; pagination and authorization are deliberately fresh. */
#include "umicom/application/experience/structured_form_search_preset.h"
#include "form_wire_internal.h"
#include <stdlib.h>
UmiStatus umi_structured_form_search_preset_validate(const UmiStructuredFormSchema *schema,
                                                     const UmiStructuredFormSearchPreset *preset) {
  if (!preset || !memchr(preset->title, 0, sizeof preset->title) ||
      !memchr(preset->entity_prefix, 0, sizeof preset->entity_prefix) ||
      !memchr(preset->metadata_text, 0, sizeof preset->metadata_text))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!preset->title[0] || !preset->entity_prefix[0])
    return UMI_STATUS_INVALID_ARGUMENT;
  for (const unsigned char *p = (const unsigned char *)preset->title; *p; ++p)
    if (*p < 32 || *p == 127)
      return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = strlen(preset->entity_prefix);
  if (preset->entity_prefix[length - 1] != '.')
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0; i < length; ++i) {
    char c = preset->entity_prefix[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' ||
          c == '_' || c == '-'))
      return UMI_STATUS_INVALID_ARGUMENT;
  }
  return umi_structured_form_filter_validate(schema, &preset->filter);
}
UmiStatus umi_structured_form_search_preset_encode(const UmiStructuredFormSchema *schema,
                                                   const UmiStructuredFormSearchPreset *preset,
                                                   void *out, size_t capacity, size_t *out_length) {
  if (!out || !out_length)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_search_preset_validate(schema, preset);
  if (status != UMI_STATUS_OK)
    return status;
  unsigned char *staged = malloc(UMI_FORM_SEARCH_PRESET_MAX_BYTES),
                *filter = malloc(UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES);
  if (!staged || !filter) {
    free(staged);
    free(filter);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  size_t length = 0;
  status = umi_structured_form_filter_snapshot_encode(schema, &preset->filter, filter,
                                                      UMI_FORM_FILTER_SNAPSHOT_MAX_BYTES, &length);
  UmiFormWireWriter w = {staged, 0, UMI_FORM_SEARCH_PRESET_MAX_BYTES, status};
  umi_form_wire_put(&w, "UMIPST1\n", 8);
  umi_form_wire_text(&w, preset->title);
  umi_form_wire_text(&w, preset->entity_prefix);
  umi_form_wire_text(&w, preset->metadata_text);
  umi_form_wire_number(&w, length, 2);
  umi_form_wire_put(&w, filter, length);
  status = w.status;
  if (status == UMI_STATUS_OK && w.at > capacity)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  if (status == UMI_STATUS_OK) {
    memcpy(out, staged, w.at);
    *out_length = w.at;
  }
  free(filter);
  free(staged);
  return status;
}
UmiStatus umi_structured_form_search_preset_decode(const UmiStructuredFormSchema *schema,
                                                   const void *bytes, size_t length,
                                                   UmiStructuredFormSearchPreset *out) {
  if (!bytes || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length < 8 || length > UMI_FORM_SEARCH_PRESET_MAX_BYTES || memcmp(bytes, "UMIPST1\n", 8))
    return UMI_STATUS_PARSE_ERROR;
  UmiStructuredFormSearchPreset *candidate = calloc(1, sizeof *candidate);
  if (!candidate)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiFormWireReader r = {bytes, 8, length, UMI_STATUS_OK};
  umi_form_wire_take_text(&r, candidate->title, sizeof candidate->title);
  umi_form_wire_take_text(&r, candidate->entity_prefix, sizeof candidate->entity_prefix);
  umi_form_wire_take_text(&r, candidate->metadata_text, sizeof candidate->metadata_text);
  size_t filter_length = (size_t)umi_form_wire_take_number(&r, 2);
  if (r.status == UMI_STATUS_OK && filter_length != r.length - r.at)
    r.status = UMI_STATUS_PARSE_ERROR;
  if (r.status == UMI_STATUS_OK)
    r.status = umi_structured_form_filter_snapshot_decode(schema, r.bytes + r.at, filter_length,
                                                          &candidate->filter);
  if (r.status == UMI_STATUS_OK)
    r.status = umi_structured_form_search_preset_validate(schema, candidate);
  if (r.status == UMI_STATUS_OK)
    *out = *candidate;
  UmiStatus status = r.status;
  free(candidate);
  return status;
}
