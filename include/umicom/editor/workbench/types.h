/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/types.h
 *
 * PURPOSE:
 *   Define stable editor-workbench identifiers, positions, ranges and shared enums.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_TYPES_H
#define UMICOM_EDITOR_WORKBENCH_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WB_ID_CAPACITY 128U
#define UMI_EDITOR_WB_PATH_CAPACITY 320U
#define UMI_EDITOR_WB_TEXT_CAPACITY 256U
#define UMI_EDITOR_WB_MAX_ITEMS 128U
#define UMI_EDITOR_WB_MAX_GROUPS 32U
#define UMI_EDITOR_WB_MAX_SEGMENTS 64U
#define UMI_EDITOR_WB_MAX_COMMANDS 96U

typedef enum UmiEditorWbOrientation { UMI_EDITOR_WB_HORIZONTAL=1, UMI_EDITOR_WB_VERTICAL=2 } UmiEditorWbOrientation;
typedef enum UmiEditorWbOpenMode { UMI_EDITOR_WB_OPEN_NORMAL=1, UMI_EDITOR_WB_OPEN_PREVIEW=2, UMI_EDITOR_WB_OPEN_PINNED=3 } UmiEditorWbOpenMode;
typedef enum UmiEditorWbCloseDecision { UMI_EDITOR_WB_CLOSE_ALLOW=1, UMI_EDITOR_WB_CLOSE_CONFIRM=2, UMI_EDITOR_WB_CLOSE_DENY=3 } UmiEditorWbCloseDecision;
typedef enum UmiEditorWbSeverity { UMI_EDITOR_WB_INFO=1, UMI_EDITOR_WB_WARNING=2, UMI_EDITOR_WB_ERROR=3 } UmiEditorWbSeverity;
typedef enum UmiEditorWbLineNumberMode { UMI_EDITOR_WB_LINE_NUMBERS_HIDDEN=0, UMI_EDITOR_WB_LINE_NUMBERS_ABSOLUTE=1, UMI_EDITOR_WB_LINE_NUMBERS_RELATIVE=2 } UmiEditorWbLineNumberMode;

typedef struct UmiEditorWbPosition { uint32_t line; uint32_t column; } UmiEditorWbPosition;
typedef struct UmiEditorWbRange { UmiEditorWbPosition start; UmiEditorWbPosition end; } UmiEditorWbRange;

/* Copy metadata into a bounded public ABI buffer and reject silent truncation. */
UmiStatus umi_editor_wb_copy_text(char *destination, size_t capacity, const char *source);
/* Return one only for stable non-empty editor-workbench identifiers. */
int umi_editor_wb_id_valid(const char *identifier);
/* Compare two source positions lexicographically by line then column. */
int umi_editor_wb_position_compare(UmiEditorWbPosition left, UmiEditorWbPosition right);
/* Normalize a range so start is never after end. */
UmiEditorWbRange umi_editor_wb_range_normalize(UmiEditorWbRange range);

#ifdef __cplusplus
}
#endif
#endif
