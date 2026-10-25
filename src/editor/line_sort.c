/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/line_sort.c
 *
 * PURPOSE:
 *   Order selected line contents stably while retaining separator slots and bounded immutable
 * output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/line_sort.h"
#include "umicom/editor/text_position.h"
#include <stdlib.h>
#include <string.h>
typedef struct SortLine {
  size_t start, content_end, end;
} SortLine;
struct UmiEditorLineSortPlan {
  char *text;
  UmiEditorLineSortSummary summary;
};
static SortLine NextSortLine(const char *source, size_t bytes, size_t start) {
  SortLine line = {start, start, start};
  while (line.content_end < bytes && source[line.content_end] != '\n')
    ++line.content_end;
  line.end = line.content_end < bytes ? line.content_end + 1U : bytes;
  if (line.content_end > start && source[line.content_end - 1U] == '\r')
    --line.content_end;
  return line;
}
static int CompareSortLines(const char *source, const SortLine *left, const SortLine *right,
                            int descending) {
  size_t a = left->content_end - left->start, b = right->content_end - right->start,
         n = a < b ? a : b;
  int compared = memcmp(source + left->start, source + right->start, n);
  compared = (compared > 0) - (compared < 0);
  if (compared == 0)
    compared = (a > b) - (a < b);
  return descending ? -compared : compared;
}
UmiStatus UmiEditorLineSortPlanCreate(const char *source, size_t bytes, size_t first, size_t last,
                                      int descending, const UmiCancellationToken *cancel,
                                      UmiEditorLineSortPlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (source == NULL || (descending != 0 && descending != 1) || first > bytes || last > bytes)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (bytes > UMI_EDITOR_LINE_SORT_TEXT_LIMIT)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  if (memchr(source, '\0', bytes) != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorTextBufferView view = {0};
  view.struct_size = (uint32_t)sizeof view;
  view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
  view.bytes = source;
  view.byte_count = view.capacity = bytes;
  UmiEditorTextPosition position;
  UmiStatus status = UmiEditorTextViewPositionAt(&view, bytes, &position);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextViewPositionAt(&view, first, &position);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextViewPositionAt(&view, last, &position);
  if (status != UMI_STATUS_OK)
    return status;
  for (size_t i = 0U; i < bytes; ++i) {
    if (source[i] == '\r' && (i + 1U == bytes || source[i + 1U] != '\n'))
      return UMI_STATUS_NOT_IMPLEMENTED;
    if ((i & 4095U) == 0U && umi_cancellation_token_is_requested(cancel))
      return UMI_STATUS_CANCELLED;
  }
  if (first == last)
    return UMI_STATUS_NOT_FOUND;
  if (first > last) {
    size_t swap = first;
    first = last;
    last = swap;
  }
  size_t block_start = 0U, block_end = 0U, count = 0U;
  for (size_t start = 0U; start < bytes;) {
    SortLine line = NextSortLine(source, bytes, start);
    if (line.end > first && line.start < last) {
      if (count == 0U)
        block_start = line.start;
      block_end = line.end;
      if (++count > UMI_EDITOR_LINE_SORT_LINE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (line.end >= last)
      break;
    start = line.end;
    if (umi_cancellation_token_is_requested(cancel))
      return UMI_STATUS_CANCELLED;
  }
  if (count == 0U)
    return UMI_STATUS_NOT_FOUND;
  SortLine *lines = calloc(count, sizeof *lines);
  size_t *order = malloc(count * sizeof *order), *scratch = malloc(count * sizeof *scratch);
  UmiEditorLineSortPlan *plan = calloc(1U, sizeof *plan);
  if (lines == NULL || order == NULL || scratch == NULL || plan == NULL) {
    free(lines);
    free(order);
    free(scratch);
    free(plan);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  for (size_t i = 0U, start = block_start; i < count; ++i) {
    lines[i] = NextSortLine(source, bytes, start);
    start = lines[i].end;
    order[i] = i;
  }
  /* Stable merge sort avoids a process-global comparator context and has
   * bounded O(n log n) comparisons even for already sorted/adversarial input. */
  for (size_t width = 1U; width < count && status == UMI_STATUS_OK; width *= 2U) {
    for (size_t base = 0U; base < count; base += 2U * width) {
      size_t middle = base + width < count ? base + width : count,
             end = base + 2U * width < count ? base + 2U * width : count;
      size_t left = base, right = middle;
      for (size_t at = base; at < end; ++at) {
        if (left < middle &&
            (right >= end ||
             CompareSortLines(source, &lines[order[left]], &lines[order[right]], descending) <= 0))
          scratch[at] = order[left++];
        else
          scratch[at] = order[right++];
      }
      if (umi_cancellation_token_is_requested(cancel)) {
        status = UMI_STATUS_CANCELLED;
        break;
      }
    }
    if (status == UMI_STATUS_OK) {
      size_t *swap = order;
      order = scratch;
      scratch = swap;
    }
  }
  if (status == UMI_STATUS_OK) {
    plan->text = malloc(bytes + 1U);
    if (plan->text == NULL)
      status = UMI_STATUS_OUT_OF_MEMORY;
  }
  if (status == UMI_STATUS_OK) {
    memcpy(plan->text, source, block_start);
    size_t at = block_start;
    for (size_t i = 0U; i < count; ++i) {
      const SortLine *content = &lines[order[i]], *slot = &lines[i];
      size_t length = content->content_end - content->start, ending = slot->end - slot->content_end;
      memcpy(plan->text + at, source + content->start, length);
      at += length;
      memcpy(plan->text + at, source + slot->content_end, ending);
      at += ending;
      if (umi_cancellation_token_is_requested(cancel)) {
        status = UMI_STATUS_CANCELLED;
        break;
      }
    }
    if (status == UMI_STATUS_OK) {
      memcpy(plan->text + at, source + block_end, bytes - block_end);
      plan->text[bytes] = '\0';
      int changed = memcmp(source, plan->text, bytes) != 0;
      plan->summary = (UmiEditorLineSortSummary){bytes, changed ? block_start : first,
                                                 changed ? block_end - block_start : last - first,
                                                 count, changed};
    }
  }
  free(lines);
  free(order);
  free(scratch);
  if (status != UMI_STATUS_OK) {
    UmiEditorLineSortPlanDestroy(plan);
    return status;
  }
  *out = plan;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorLineSortPlanInspect(const UmiEditorLineSortPlan *plan,
                                       UmiEditorLineSortSummary *out) {
  if (plan == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = plan->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorLineSortPlanRead(const UmiEditorLineSortPlan *plan, const char **out,
                                    size_t *bytes) {
  if (out == NULL || bytes == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  *bytes = 0U;
  if (plan == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = plan->text;
  *bytes = plan->summary.source_bytes;
  return UMI_STATUS_OK;
}
void UmiEditorLineSortPlanDestroy(UmiEditorLineSortPlan *plan) {
  if (plan != NULL) {
    free(plan->text);
    free(plan);
  }
}
