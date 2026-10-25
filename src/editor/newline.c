/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/newline.c
 *
 * PURPOSE:
 *   Centralise newline planning for native and headless source editors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/newline.h"
#include "umicom/editor/text_position.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiEditorNewlineCreate(const char *source, size_t bytes, size_t cursor, size_t selection,
                                 int inherit, char **out, size_t *out_bytes) {
  if (source == NULL || out == NULL || out_bytes == NULL || (inherit != 0 && inherit != 1) ||
      cursor > bytes || selection > bytes - cursor)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (bytes > UMI_EDITOR_NEWLINE_MAXIMUM_SOURCE) /* Retain the draft spelling for review; use the
                                                    declared capacity status. */
#if 0
        return UMI_STATUS_LIMIT_EXCEEDED;
#endif
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (memchr(source, 0, bytes) != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorTextBufferView view = {0};
  view.struct_size = sizeof view;
  view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
  view.bytes = source;
  view.byte_count = bytes;
  view.capacity = bytes;
  UmiEditorTextPosition position;
  UmiStatus status = UmiEditorTextViewPositionAt(&view, bytes, &position);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextViewPositionAt(&view, cursor, &position);
  if (status == UMI_STATUS_OK)
    status = UmiEditorTextViewPositionAt(&view, cursor + selection, &position);
  if (status != UMI_STATUS_OK)
    return status;
  for (size_t i = 0U; i < bytes; ++i)
    if (source[i] == '\r' && (i + 1U == bytes || source[i + 1U] != '\n'))
      return UMI_STATUS_NOT_IMPLEMENTED;
  size_t start = cursor;
  while (start != 0U && source[start - 1U] != '\n')
    --start;
  size_t end = cursor;
  while (end < bytes && source[end] != '\n')
    ++end;
  /* A line without a terminator inherits the preceding line's convention,
   * rather than consulting the platform on which a file happens to be open. */
  size_t newline = end < bytes ? end : start != 0U ? start - 1U : bytes;
  const int crlf = newline < bytes && newline != 0U && source[newline - 1U] == '\r';
  size_t indentation = 0U;
  if (inherit)
    while (start + indentation < cursor &&
           (source[start + indentation] == ' ' || source[start + indentation] == '\t'))
      ++indentation;
  const size_t length = (crlf ? 2U : 1U) + indentation;
  if (length > UMI_EDITOR_NEWLINE_MAXIMUM_SOURCE - (bytes - selection))
  /* Retain the draft spelling for review; use the declared capacity status. */
#if 0
        return UMI_STATUS_LIMIT_EXCEEDED;
#endif
    return UMI_STATUS_CAPACITY_EXCEEDED;
  char *token = malloc(length + 1U);
  if (token == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  size_t offset = 0U;
  if (crlf)
    token[offset++] = '\r';
  token[offset++] = '\n';
  memcpy(token + offset, source + start, indentation);
  token[length] = '\0';
  *out = token;
  *out_bytes = length;
  return UMI_STATUS_OK;
}
void UmiEditorNewlineFree(char *text) { free(text); }
