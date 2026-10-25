/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/indentation_record.c
 *
 * PURPOSE:
 *   Keep editor preference decoding strict and independent from application settings stores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/indentation_record.h"
#include <stdio.h>
#include <string.h>
UmiStatus UmiEditorIndentationEncode(const UmiEditorIndentation *options, char *out_text,
                                     size_t capacity, size_t *out_size) {
  if (out_text == NULL || out_size == NULL || capacity == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiEditorIndentationValidate(options);
  if (status != UMI_STATUS_OK)
    return status;
  char encoded[UMI_EDITOR_INDENTATION_RECORD_MAXIMUM_BYTES];
  int length =
      snprintf(encoded, sizeof encoded, "umicom.editor.indentation/1\nwidth=%u\nspaces=%d\n",
               options->tab_width, options->insert_spaces);
  if (length < 0 || (size_t)length >= sizeof encoded)
    return UMI_STATUS_INTERNAL_ERROR;
  if ((size_t)length >= capacity)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(out_text, encoded, (size_t)length + 1U);
  *out_size = (size_t)length;
  return UMI_STATUS_OK;
}
static int IndentationRecordLine(const unsigned char *bytes, size_t size, size_t *offset,
                                 char *line, size_t capacity) {
  size_t length = 0U;
  if (*offset >= size)
    return 0;
  while (*offset < size && bytes[*offset] != '\n') {
    unsigned char c = bytes[(*offset)++];
    if (c == '\r') {
      if (*offset >= size || bytes[*offset] != '\n')
        return 0;
      break;
    }
    if (c < 0x20U || c > 0x7eU || length + 1U >= capacity)
      return 0;
    line[length++] = (char)c;
  }
  if (*offset < size && bytes[*offset] == '\n')
    ++*offset;
  line[length] = '\0';
  return 1;
}
UmiStatus UmiEditorIndentationDecode(const void *bytes, size_t size, UmiEditorIndentation *out) {
  if (bytes == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (size > UMI_EDITOR_INDENTATION_RECORD_MAXIMUM_BYTES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  char lines[3][64];
  size_t offset = 0U;
  for (size_t i = 0U; i < 3U; ++i)
    if (!IndentationRecordLine(bytes, size, &offset, lines[i], sizeof lines[i]))
      return UMI_STATUS_PARSE_ERROR;
  if (offset != size)
    return UMI_STATUS_PARSE_ERROR;
  if (strcmp(lines[0], "umicom.editor.indentation/1") != 0)
    return UMI_STATUS_NOT_IMPLEMENTED;
  if (strncmp(lines[1], "width=", 6U) != 0 || strlen(lines[1]) < 7U || strlen(lines[1]) > 8U)
    return UMI_STATUS_PARSE_ERROR;
  uint32_t width = 0U;
  for (size_t i = 6U; lines[1][i] != '\0'; ++i) {
    if (lines[1][i] < '0' || lines[1][i] > '9')
      return UMI_STATUS_PARSE_ERROR;
    width = width * 10U + (uint32_t)(lines[1][i] - '0');
  }
  if (strcmp(lines[2], "spaces=0") != 0 && strcmp(lines[2], "spaces=1") != 0)
    return UMI_STATUS_PARSE_ERROR;
  UmiEditorIndentation candidate = {width, lines[2][7] == '1'};
  UmiStatus status = UmiEditorIndentationValidate(&candidate);
  if (status == UMI_STATUS_OK)
    *out = candidate;
  return status;
}
