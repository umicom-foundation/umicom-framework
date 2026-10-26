/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_inspection.c
 *
 * PURPOSE:
 *   Own immutable complete-source inspection and refuse partial observations on invalid boundaries
 * or cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "text_inspection_internal.h"
#include <stdlib.h>
#include <string.h>
static int Boundary(const char *text, size_t bytes, size_t offset) {
  if (offset > bytes)
    return 0;
  if (offset < bytes && (((unsigned char)text[offset] & 0xc0U) == 0x80U))
    return 0;
  return !(offset > 0U && offset < bytes && text[offset - 1U] == '\r' && text[offset] == '\n');
}
void UmiEditorTextInspectionDestroy(UmiEditorTextInspection *value) {
  if (value != NULL) {
    free(value->text);
    free(value->notices);
    free(value);
  }
}
UmiStatus UmiEditorTextInspectionCreate(const char *text, size_t bytes, size_t cursor,
                                        size_t selection, const UmiCancellationToken *cancel,
                                        UmiEditorTextInspection **out) {
  if (out == NULL || *out != NULL || (text == NULL && bytes != 0U))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (bytes > UMI_EDITOR_INSPECTION_TEXT_LIMIT)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (cursor > bytes || selection > bytes - cursor)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  const char *source = text != NULL ? text : "";
  /* Retain the former ordering for review. A leading continuation byte is
   * malformed source, not evidence that a valid character was split. Validate
   * the owned document before interpreting cursor boundaries below. */
#if 0
  if (!Boundary(source, bytes, cursor) || !Boundary(source, bytes, cursor + selection))
    return UMI_STATUS_INVALID_ARGUMENT;
#endif
  UmiEditorTextInspection *value = calloc(1U, sizeof *value);
  if (value == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  value->notices = calloc(UMI_EDITOR_INSPECTION_NOTICE_LIMIT, sizeof *value->notices);
  value->text = malloc(bytes + 1U);
  if (value->notices == NULL || value->text == NULL) {
    UmiEditorTextInspectionDestroy(value);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  memcpy(value->text, source, bytes);
  value->text[bytes] = '\0';
  UmiStatus status = InspectionMeasure(value->text, bytes, cancel, &value->summary.document, value);
  if (status == UMI_STATUS_OK &&
      (!Boundary(value->text, bytes, cursor) ||
       !Boundary(value->text, bytes, cursor + selection)))
    status = UMI_STATUS_INVALID_ARGUMENT;
  if (status == UMI_STATUS_OK)
    status =
        InspectionMeasure(value->text + cursor, selection, cancel, &value->summary.selection, NULL);
  if (status != UMI_STATUS_OK) {
    UmiEditorTextInspectionDestroy(value);
    return status;
  }
  value->summary.cursor_offset = cursor;
  value->summary.selection_bytes = selection;
  value->summary.notices_truncated = value->summary.notice_count > value->summary.retained_notices;
  *out = value;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorTextInspectionSummaryRead(const UmiEditorTextInspection *value,
                                             UmiEditorTextInspectionSummary *out) {
  if (value == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = value->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorTextInspectionNoticeAt(const UmiEditorTextInspection *value, size_t index,
                                          UmiEditorTextNotice *out) {
  if (value == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= value->summary.retained_notices)
    return UMI_STATUS_NOT_FOUND;
  *out = value->notices[index];
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorTextInspectionRead(const UmiEditorTextInspection *value, const char **text,
                                      size_t *bytes) {
  if (value == NULL || text == NULL || bytes == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *text = value->text;
  *bytes = value->summary.document.bytes;
  return UMI_STATUS_OK;
}
