/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/types.c
 *
 * PURPOSE:
 *   Define stable editor-workbench identifiers, positions, ranges and shared enums.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/types.h"
#include <string.h>

UmiStatus umi_editor_wb_copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
int umi_editor_wb_id_valid(const char *identifier) { return identifier != NULL && identifier[0] != '\0' && strlen(identifier) < UMI_EDITOR_WB_ID_CAPACITY; }
int umi_editor_wb_position_compare(UmiEditorWbPosition left, UmiEditorWbPosition right) {
    if (left.line < right.line) return -1;
    if (left.line > right.line) return 1;
    if (left.column < right.column) return -1;
    if (left.column > right.column) return 1;
    return 0;
}
UmiEditorWbRange umi_editor_wb_range_normalize(UmiEditorWbRange range) {
    if (umi_editor_wb_position_compare(range.start, range.end) > 0) { UmiEditorWbPosition t=range.start; range.start=range.end; range.end=t; }
    return range;
}
