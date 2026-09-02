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

/*
 * Provide the editor wb copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_wb_copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
/* Check that editor wb id satisfies its contract before another service relies on it. */
int umi_editor_wb_id_valid(const char *identifier) { return identifier != NULL && identifier[0] != '\0' && strlen(identifier) < UMI_EDITOR_WB_ID_CAPACITY; }
/*
 * Provide the editor wb position compare operation used by this module and its client
 * applications.
 */
int umi_editor_wb_position_compare(UmiEditorWbPosition left, UmiEditorWbPosition right) {
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.line < right.line) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.line > right.line) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.column < right.column) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.column > right.column) return 1;
    return 0;
}
/*
 * Provide the editor wb range normalize operation used by this module and its client
 * applications.
 */
UmiEditorWbRange umi_editor_wb_range_normalize(UmiEditorWbRange range) {
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_wb_position_compare(range.start, range.end) > 0) { UmiEditorWbPosition t=range.start; range.start=range.end; range.end=t; }
    return range;
}
