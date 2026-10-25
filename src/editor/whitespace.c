/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/whitespace.c
 *
 * PURPOSE:
 *   Convert leading indentation or trim trailing ASCII whitespace without changing source
 * ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/whitespace.h"
#include "umicom/editor/text_position.h"
#include <stdlib.h>
#include <string.h>
struct UmiEditorWhitespacePlan {
  char *text;
  UmiEditorWhitespaceSummary summary;
};
typedef struct WhitespaceWriter {
  char *text;
  size_t used;
  UmiEditorWhitespaceSummary summary;
  size_t endpoints[2], mapped[2];
  int mapped_set[2];
} WhitespaceWriter;
static UmiStatus WhitespaceAppend(WhitespaceWriter *writer, const char *text, size_t bytes) {
  if (bytes > UMI_EDITOR_WHITESPACE_TEXT_LIMIT - writer->used)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (writer->text != NULL && bytes != 0U)
    memcpy(writer->text + writer->used, text, bytes);
  writer->used += bytes;
  return UMI_STATUS_OK;
}
static UmiStatus WhitespaceRepeat(WhitespaceWriter *writer, char value, size_t count) {
  if (count > UMI_EDITOR_WHITESPACE_TEXT_LIMIT - writer->used)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (writer->text != NULL && count != 0U)
    memset(writer->text + writer->used, value, count);
  writer->used += count;
  return UMI_STATUS_OK;
}
static size_t WhitespaceColumn(const char *text, size_t start, size_t end, unsigned width) {
  size_t column = 0U;
  for (size_t i = start; i < end; ++i)
    column += text[i] == '\t' ? width - column % width : 1U;
  return column;
}
/* Two bounded passes avoid reallocating during publication. The first proves
 * output capacity; the second owns exact output and endpoint mapping. No caller
 * observes a partial transformed buffer. */
static UmiStatus WhitespacePass(const char *source, size_t bytes,
                                const UmiEditorWhitespaceOptions *options,
                                const UmiCancellationToken *cancel, WhitespaceWriter *writer) {
  for (size_t start = 0U; start < bytes;) {
    if (umi_cancellation_token_is_requested(cancel))
      return UMI_STATUS_CANCELLED;
    size_t content_end = start;
    while (content_end < bytes && source[content_end] != '\r' && source[content_end] != '\n') {
      ++content_end;
      if ((content_end & 4095U) == 0U && umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    }
    size_t end = content_end;
    if (end < bytes) {
      ++end;
      if (source[content_end] == '\r' && end < bytes && source[end] == '\n')
        ++end;
    }
    const size_t first = writer->endpoints[0], last = writer->endpoints[1];
    int selected =
        !options->selected_lines ||
        (first == last ? first >= start && (first < end || (first == end && end == content_end))
                       : first < end && last > start);
    size_t cut_start = content_end, cut_end = content_end, columns = 0U, replacement = 0U;
    if (selected) {
      ++writer->summary.touched_lines;
      if (options->operation == UMI_EDITOR_WHITESPACE_TRIM_TRAILING) {
        while (cut_start > start &&
               (source[cut_start - 1U] == ' ' || source[cut_start - 1U] == '\t'))
          --cut_start;
      } else {
        cut_start = start;
        cut_end = start;
        while (cut_end < content_end && (source[cut_end] == ' ' || source[cut_end] == '\t'))
          ++cut_end;
        columns = WhitespaceColumn(source, start, cut_end, options->tab_width);
        replacement = options->operation == UMI_EDITOR_WHITESPACE_EXPAND_LEADING_TABS
                          ? columns
                          : columns / options->tab_width + columns % options->tab_width;
      }
    }
    size_t removed = cut_end - cut_start;
    int changed = removed != replacement;
    if (!changed && removed != 0U) {
      for (size_t i = 0U; i < replacement; ++i) {
        char expected = options->operation == UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES &&
                                i < columns / options->tab_width
                            ? '\t'
                            : ' ';
        if (source[cut_start + i] != expected) {
          changed = 1;
          break;
        }
      }
    }
    const size_t output_start = writer->used;
    UmiStatus status = WhitespaceAppend(writer, source + start, cut_start - start);
    if (status != UMI_STATUS_OK)
      return status;
    if (options->operation == UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES) {
      status = WhitespaceRepeat(writer, '\t', columns / options->tab_width);
      if (status == UMI_STATUS_OK)
        status = WhitespaceRepeat(writer, ' ', columns % options->tab_width);
    } else
      status = WhitespaceRepeat(writer, ' ', replacement);
    if (status == UMI_STATUS_OK)
      status = WhitespaceAppend(writer, source + cut_end, end - cut_end);
    if (status != UMI_STATUS_OK)
      return status;
    if (changed) {
      ++writer->summary.changed_lines;
      writer->summary.removed_bytes += removed;
      writer->summary.inserted_bytes += replacement;
    }
    for (size_t i = 0U; i < 2U; ++i) {
      size_t point = writer->endpoints[i];
      if (writer->mapped_set[i] || point < start || point > end)
        continue;
      size_t mapped;
      if (point < cut_start)
        mapped = output_start + point - start;
      else if (point >= cut_end)
        mapped = output_start + cut_start - start + replacement + point - cut_end;
      else if (options->operation == UMI_EDITOR_WHITESPACE_TRIM_TRAILING)
        mapped = output_start + cut_start - start;
      else {
        size_t column = WhitespaceColumn(source, start, point, options->tab_width);
        mapped =
            output_start + (options->operation == UMI_EDITOR_WHITESPACE_EXPAND_LEADING_TABS
                                ? column
                                : column / options->tab_width +
                                      (column / options->tab_width == columns / options->tab_width
                                           ? column % options->tab_width
                                           : 0U));
      }
      writer->mapped[i] = mapped;
      writer->mapped_set[i] = 1;
    }
    start = end;
  }
  /* Empty sources and EOF after a terminal newline have no bytes to convert. */
  for (size_t i = 0U; i < 2U; ++i)
    if (!writer->mapped_set[i])
      writer->mapped[i] = writer->used;
  writer->summary.source_bytes = bytes;
  writer->summary.proposed_bytes = writer->used;
  writer->summary.cursor_offset = writer->mapped[0];
  writer->summary.selection_bytes = writer->mapped[1] - writer->mapped[0];
  writer->summary.changed = writer->summary.changed_lines != 0U;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorWhitespacePlanCreate(const char *source, size_t bytes, size_t cursor,
                                        size_t selected, const UmiEditorWhitespaceOptions *options,
                                        const UmiCancellationToken *cancel,
                                        UmiEditorWhitespacePlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (source == NULL || options == NULL || cursor > bytes || selected > bytes - cursor ||
      options->operation < UMI_EDITOR_WHITESPACE_TRIM_TRAILING ||
      options->operation > UMI_EDITOR_WHITESPACE_COMPACT_LEADING_SPACES ||
      options->tab_width < 1U || options->tab_width > 16U ||
      (options->selected_lines != 0 && options->selected_lines != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (bytes > UMI_EDITOR_WHITESPACE_TEXT_LIMIT)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  if (memchr(source, 0, bytes) != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorTextBufferView view = {0};
  view.struct_size = (uint32_t)sizeof view;
  view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
  view.bytes = source;
  view.byte_count = view.capacity = bytes;
  UmiEditorTextPosition position;
  UmiStatus status = UmiEditorTextViewPositionAt(&view, bytes, &position);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextViewPositionAt(&view, cursor, &position);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextViewPositionAt(&view, cursor + selected, &position);
  if (status != UMI_STATUS_OK)
    return status;
  WhitespaceWriter measure = {0};
  measure.endpoints[0] = cursor;
  measure.endpoints[1] = cursor + selected;
  status = WhitespacePass(source, bytes, options, cancel, &measure);
  if (status != UMI_STATUS_OK)
    return status;
  UmiEditorWhitespacePlan *plan = calloc(1U, sizeof *plan);
  if (plan == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  plan->text = malloc(measure.used + 1U);
  if (plan->text == NULL) {
    free(plan);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  WhitespaceWriter writer = {0};
  writer.text = plan->text;
  writer.endpoints[0] = cursor;
  writer.endpoints[1] = cursor + selected;
  status = WhitespacePass(source, bytes, options, cancel, &writer);
  if (status != UMI_STATUS_OK) {
    UmiEditorWhitespacePlanDestroy(plan);
    return status;
  }
  plan->text[writer.used] = '\0';
  plan->summary = writer.summary;
  *out = plan;
  return UMI_STATUS_OK;
}
void UmiEditorWhitespacePlanDestroy(UmiEditorWhitespacePlan *plan) {
  if (plan != NULL) {
    free(plan->text);
    free(plan);
  }
}
UmiStatus UmiEditorWhitespacePlanInspect(const UmiEditorWhitespacePlan *plan,
                                         UmiEditorWhitespaceSummary *out) {
  if (plan == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = plan->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorWhitespacePlanRead(const UmiEditorWhitespacePlan *plan, const char **text,
                                      size_t *bytes) {
  if (text != NULL)
    *text = NULL;
  if (bytes != NULL)
    *bytes = 0U;
  if (plan == NULL || text == NULL || bytes == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *text = plan->text;
  *bytes = plan->summary.proposed_bytes;
  return UMI_STATUS_OK;
}
