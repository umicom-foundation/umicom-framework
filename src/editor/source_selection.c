/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/source_selection.c
 *
 * PURPOSE:
 *   Resolve selection boundaries using shared UTF-8 coordinate validation and physical line
 * endings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/source_selection.h"
#include <string.h>
static int SelectionWordByte(unsigned char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' ||
         c >= 0x80U;
}
UmiStatus UmiEditorSourceSelectionAt(const UmiEditorTextBufferView *view, size_t caret,
                                     UmiEditorSourceSelectionKind kind,
                                     UmiEditorSourceSelection *out) {
  if (view == NULL || out == NULL || kind < UMI_EDITOR_SELECT_SOURCE_WORD ||
      kind > UMI_EDITOR_SELECT_LINE_WITH_ENDING)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (view->byte_count > 8U * 1024U * 1024U)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiEditorTextPosition position;
  UmiStatus status = UmiEditorTextViewPositionAt(view, view->byte_count, &position);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextViewPositionAt(view, caret, &position);
  if (status != UMI_STATUS_OK)
    return status;
  if (view->byte_count && memchr(view->bytes, 0, view->byte_count))
    return UMI_STATUS_INVALID_ARGUMENT;
  const char *text = view->bytes;
  size_t bytes = view->byte_count, start = caret, end = caret;
  if (kind == UMI_EDITOR_SELECT_SOURCE_WORD) {
    if (start < bytes && SelectionWordByte((unsigned char)text[start])) {
    } else if (start > 0U && SelectionWordByte((unsigned char)text[start - 1U]))
      --start;
    else
      return UMI_STATUS_NOT_FOUND;
    while (start > 0U && SelectionWordByte((unsigned char)text[start - 1U]))
      --start;
    end = start;
    while (end < bytes && SelectionWordByte((unsigned char)text[end]))
      ++end;
  } else {
    while (start > 0U && text[start - 1U] != '\r' && text[start - 1U] != '\n')
      --start;
    while (end < bytes && text[end] != '\r' && text[end] != '\n')
      ++end;
    if (kind == UMI_EDITOR_SELECT_LINE_WITH_ENDING && end < bytes) {
      if (text[end++] == '\r' && end < bytes && text[end] == '\n')
        ++end;
    }
  }
  *out = (UmiEditorSourceSelection){start, end - start};
  return UMI_STATUS_OK;
}
