/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/indentation.c
 *
 * PURPOSE:
 *   Centralise bounded indentation policy and tab-stop calculation without toolkit or global locale
 * dependence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/indentation.h"
#include "umicom/editor/text_position.h"
#include <string.h>
UmiStatus UmiEditorIndentationDefault(UmiEditorIndentation *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = (UmiEditorIndentation){4U, 1};
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorIndentationValidate(const UmiEditorIndentation *options) {
  return options != NULL && options->tab_width >= 1U &&
                 options->tab_width <= UMI_EDITOR_INDENTATION_MAXIMUM_WIDTH &&
                 (options->insert_spaces == 0 || options->insert_spaces == 1)
             ? UMI_STATUS_OK
             : UMI_STATUS_INVALID_ARGUMENT;
}
static UmiStatus IndentationWrite(const UmiEditorIndentation *options, size_t spaces, char *output,
                                  size_t capacity, size_t *outBytes) {
  UmiStatus status = UmiEditorIndentationValidate(options);
  if (status != UMI_STATUS_OK || output == NULL || outBytes == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t count = options->insert_spaces ? spaces : 1U;
  if (capacity <= count)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  /* Copy from a complete local value after all checks, so short destinations
   * never expose a partial indentation token. */
  char token[UMI_EDITOR_INDENTATION_MAXIMUM_WIDTH + 1U];
  memset(token, options->insert_spaces ? ' ' : '\t', count);
  token[count] = '\0';
  memcpy(output, token, count + 1U);
  *outBytes = count;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorIndentationUnit(const UmiEditorIndentation *options, char *output,
                                   size_t capacity, size_t *outBytes) {
  if (UmiEditorIndentationValidate(options) != UMI_STATUS_OK)
    return UMI_STATUS_INVALID_ARGUMENT;
  return IndentationWrite(options, options->tab_width, output, capacity, outBytes);
}
UmiStatus UmiEditorIndentationAt(const UmiEditorIndentation *options, const char *text,
                                 size_t bytes, size_t offset, char *output, size_t capacity,
                                 size_t *outBytes) {
  if (UmiEditorIndentationValidate(options) != UMI_STATUS_OK || text == NULL || offset > bytes ||
      output == NULL || outBytes == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorTextBufferView view = {0};
  view.struct_size = (uint32_t)sizeof view;
  view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
  view.bytes = text;
  view.byte_count = view.capacity = bytes;
  UmiEditorTextPosition position;
  UmiStatus status = UmiEditorTextViewPositionAt(&view, offset, &position);
  if (status != UMI_STATUS_OK)
    return status;
  if (memchr(text, '\0', offset) != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t start = offset;
  while (start > 0U && text[start - 1U] != '\n' && text[start - 1U] != '\r')
    --start;
  /* Only the remainder is needed. It cannot overflow even for a very long line. */
  size_t remainder = 0U;
  for (size_t i = start; i < offset; ++i) {
    unsigned char byte = (unsigned char)text[i];
    if (byte == '\t')
      remainder = 0U;
    else if ((byte & 0xc0U) != 0x80U)
      remainder = (remainder + 1U) % options->tab_width;
  }
  return IndentationWrite(options, options->tab_width - remainder, output, capacity, outBytes);
}
