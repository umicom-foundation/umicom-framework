/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/search_range.inc
 *
 * PURPOSE:
 *   Reuse literal matching and full-source boundary policy for complete range-limited replacements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/search_range.h"
#include <stdint.h>
static UmiStatus SearchRangePolicy(const UmiEditorSearchOptions *options,
                                   UmiEditorSearchOptions *effective) {
  *effective = (UmiEditorSearchOptions){UMI_EDITOR_SEARCH_CASE_SENSITIVE, 0, 0, 0};
  if (options != NULL)
    *effective = *options;
  if (effective->case_mode < UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
      effective->case_mode > UMI_EDITOR_SEARCH_CASE_SMART ||
      (effective->whole_word != 0 && effective->whole_word != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  return UMI_STATUS_OK;
}
static int SearchRangeOverlap(const void *a, size_t an, const void *b, size_t bn) {
  if (an == 0U || bn == 0U)
    return 0;
  uintptr_t aa = (uintptr_t)a, bb = (uintptr_t)b;
  return aa <= bb ? bb - aa < an : aa - bb < bn;
}
UmiStatus UmiEditorSearchReplaceRangeSize(const char *text, size_t bytes, size_t start, size_t span,
                                          const char *needle, size_t needle_bytes,
                                          size_t replacement_bytes,
                                          const UmiEditorSearchOptions *options, size_t *out_bytes,
                                          size_t *out_count) {
  if ((text == NULL && bytes != 0U) || needle == NULL || needle_bytes == 0U || out_bytes == NULL ||
      start > bytes || span > bytes - start)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorSearchOptions policy;
  UmiStatus status = SearchRangePolicy(options, &policy);
  if (status != UMI_STATUS_OK)
    return status;
  int sensitive =
      policy.case_mode == UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
      (policy.case_mode == UMI_EDITOR_SEARCH_CASE_SMART && needle_has_upper(needle, needle_bytes));
  size_t result = bytes, count = 0U, end = start + span;
  for (size_t offset = start; offset < end;) {
    if (needle_bytes <= end - offset &&
        SearchMatchAt(text, bytes, offset, needle, needle_bytes, sensitive, policy.whole_word)) {
      if (replacement_bytes > needle_bytes) {
        size_t growth = replacement_bytes - needle_bytes;
        if (growth > SIZE_MAX - result)
          return UMI_STATUS_CAPACITY_EXCEEDED;
        result += growth;
      } else
        result -= needle_bytes - replacement_bytes;
      ++count;
      offset += needle_bytes;
    } else
      ++offset;
  }
  *out_bytes = result;
  if (out_count != NULL)
    *out_count = count;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorSearchReplaceRange(const char *text, size_t bytes, size_t start, size_t span,
                                      const char *needle, size_t needle_bytes,
                                      const char *replacement, size_t replacement_bytes,
                                      const UmiEditorSearchOptions *options, char *out,
                                      size_t capacity, size_t *out_count) {
  if (out == NULL || capacity == 0U || (replacement == NULL && replacement_bytes != 0U))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t result = 0U, count = 0U;
  UmiStatus status = UmiEditorSearchReplaceRangeSize(text, bytes, start, span, needle, needle_bytes,
                                                     replacement_bytes, options, &result, &count);
  if (status != UMI_STATUS_OK)
    return status;
  if (result >= capacity)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (SearchRangeOverlap(out, result + 1U, text, bytes) ||
      SearchRangeOverlap(out, result + 1U, needle, needle_bytes) ||
      SearchRangeOverlap(out, result + 1U, replacement, replacement_bytes))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorSearchOptions policy;
  (void)SearchRangePolicy(options, &policy);
  int sensitive =
      policy.case_mode == UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
      (policy.case_mode == UMI_EDITOR_SEARCH_CASE_SMART && needle_has_upper(needle, needle_bytes));
  size_t used = start, end = start + span;
  if (start)
    memcpy(out, text, start);
  for (size_t offset = start; offset < end;) {
    if (needle_bytes <= end - offset &&
        SearchMatchAt(text, bytes, offset, needle, needle_bytes, sensitive, policy.whole_word)) {
      if (replacement_bytes)
        memcpy(out + used, replacement, replacement_bytes);
      used += replacement_bytes;
      offset += needle_bytes;
    } else
      out[used++] = text[offset++];
  }
  if (bytes > end)
    memcpy(out + used, text + end, bytes - end);
  used += bytes - end;
  out[used] = '\0';
  if (out_count != NULL)
    *out_count = count;
  return UMI_STATUS_OK;
}
