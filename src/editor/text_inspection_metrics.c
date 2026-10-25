/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_inspection_metrics.c
 *
 * PURPOSE:
 *   Measure logical lines and Unicode counts in one bounded pass, retaining explicit notice
 * locations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "text_inspection_internal.h"
#include <string.h>
static void FinishLine(UmiEditorTextMetrics *m, size_t scalars, int blank, int trailing) {
  if (blank)
    ++m->blank_lines;
  if (trailing)
    ++m->trailing_whitespace_lines;
  if (scalars > m->longest_line_scalars)
    m->longest_line_scalars = scalars;
}
UmiStatus InspectionMeasure(const char *text, size_t bytes, const UmiCancellationToken *cancel,
                            UmiEditorTextMetrics *out, UmiEditorTextInspection *notices) {
  UmiEditorTextMetrics m = {0};
  m.bytes = bytes;
  m.lines = 1U;
  size_t column = 0U;
  int blank = 1, trailing = 0, word = 0;
  for (size_t offset = 0U; offset < bytes;) {
    if (umi_cancellation_token_is_requested(cancel))
      return UMI_STATUS_CANCELLED;
    uint32_t c;
    size_t width;
    UmiStatus status = InspectionScalar(text, bytes, offset, &c, &width);
    if (status != UMI_STATUS_OK)
      return status;
    ++m.scalars;
    m.utf16_units += c > 0xffffU ? 2U : 1U;
    m.non_ascii += c > 0x7fU ? 1U : 0U;
    if (c == ' ')
      ++m.spaces;
    if (c == '\t')
      ++m.tabs;
    int separator = c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
    if (!separator && !word)
      ++m.words;
    word = !separator;
    UmiEditorTextNoticeKind kind = InspectionNoticeKind(c);
    if (notices != NULL && kind != 0) {
      ++notices->summary.notice_count;
      if (notices->summary.retained_notices < UMI_EDITOR_INSPECTION_NOTICE_LIMIT) {
        UmiEditorTextNotice n = {kind, c, offset, width, m.lines, column + 1U};
        notices->notices[notices->summary.retained_notices++] = n;
      }
    }
    if (c == '\r' || c == '\n') {
      FinishLine(&m, column, blank, trailing);
      if (c == '\r' && offset + 1U < bytes && text[offset + 1U] == '\n') {
        ++m.crlf;
        ++m.scalars;
        ++m.utf16_units;
        ++width;
      } else if (c == '\r')
        ++m.cr;
      else
        ++m.lf;
      ++m.lines;
      column = 0U;
      blank = 1;
      trailing = 0;
      m.ends_with_line_break = 1;
    } else {
      ++column;
      trailing = c == ' ' || c == '\t';
      blank = blank && trailing;
      m.ends_with_line_break = 0;
    }
    offset += width;
  }
  FinishLine(&m, column, blank, trailing);
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  *out = m;
  return UMI_STATUS_OK;
}
