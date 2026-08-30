/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/types.h
 *
 * PURPOSE:
 *   Define stable, toolkit-neutral value types shared by the Layout Browser,
 *   visual designer, drag-and-drop engine, review surfaces, persistence bridge
 *   and frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_TYPES_H
#define UMICOM_WORKBENCH_DESIGNER_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/workbench_layout/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_WORKBENCH_DESIGNER_ID_CAPACITY 128U
#define UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY 192U
#define UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY 512U
#define UMI_WORKBENCH_DESIGNER_PATH_CAPACITY 512U
#define UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS 64U
#define UMI_WORKBENCH_DESIGNER_MAX_CANVAS_ITEMS 256U
#define UMI_WORKBENCH_DESIGNER_MAX_TREE_ROWS 256U
#define UMI_WORKBENCH_DESIGNER_MAX_PALETTE_ITEMS 256U
#define UMI_WORKBENCH_DESIGNER_MAX_PROPERTIES 64U
#define UMI_WORKBENCH_DESIGNER_MAX_COMMANDS 128U
#define UMI_WORKBENCH_DESIGNER_MAX_CLIPBOARD_NODES 64U
#define UMI_WORKBENCH_DESIGNER_MAX_DROP_TARGETS 128U
#define UMI_WORKBENCH_DESIGNER_MAX_GUIDES 64U
#define UMI_WORKBENCH_DESIGNER_MAX_MONITORS 16U
#define UMI_WORKBENCH_DESIGNER_MAX_BROWSER_ITEMS 128U
#define UMI_WORKBENCH_DESIGNER_MAX_COLLABORATORS 32U
#define UMI_WORKBENCH_DESIGNER_MAX_ISSUES 128U
#define UMI_WORKBENCH_DESIGNER_MAX_KEYBINDINGS 96U
#define UMI_WORKBENCH_DESIGNER_MAX_TOOLBAR_ITEMS 64U
#define UMI_WORKBENCH_DESIGNER_MAX_LISTENERS 32U
#define UMI_WORKBENCH_DESIGNER_MAX_SESSIONS 8U
#define UMI_WORKBENCH_DESIGNER_INDEX_NONE ((size_t)-1)

typedef enum UmiWorkbenchDesignerState {
    UMI_WORKBENCH_DESIGNER_STATE_CREATED = 1,
    UMI_WORKBENCH_DESIGNER_STATE_INITIALISED = 2,
    UMI_WORKBENCH_DESIGNER_STATE_RUNNING = 3,
    UMI_WORKBENCH_DESIGNER_STATE_QUIESCED = 4,
    UMI_WORKBENCH_DESIGNER_STATE_STOPPED = 5,
    UMI_WORKBENCH_DESIGNER_STATE_FAILED = 6
} UmiWorkbenchDesignerState;

typedef enum UmiWorkbenchDesignerMode {
    UMI_WORKBENCH_DESIGNER_MODE_BROWSE = 1,
    UMI_WORKBENCH_DESIGNER_MODE_DESIGN = 2,
    UMI_WORKBENCH_DESIGNER_MODE_PREVIEW = 3,
    UMI_WORKBENCH_DESIGNER_MODE_REVIEW = 4,
    UMI_WORKBENCH_DESIGNER_MODE_CONFLICT = 5
} UmiWorkbenchDesignerMode;

typedef enum UmiWorkbenchDesignerTool {
    UMI_WORKBENCH_DESIGNER_TOOL_SELECT = 1,
    UMI_WORKBENCH_DESIGNER_TOOL_PAN = 2,
    UMI_WORKBENCH_DESIGNER_TOOL_SPLIT_HORIZONTAL = 3,
    UMI_WORKBENCH_DESIGNER_TOOL_SPLIT_VERTICAL = 4,
    UMI_WORKBENCH_DESIGNER_TOOL_TAB_GROUP = 5,
    UMI_WORKBENCH_DESIGNER_TOOL_FLOAT = 6,
    UMI_WORKBENCH_DESIGNER_TOOL_DOCK = 7,
    UMI_WORKBENCH_DESIGNER_TOOL_LINK_CONTEXT = 8
} UmiWorkbenchDesignerTool;

typedef enum UmiWorkbenchDesignerDropZone {
    UMI_WORKBENCH_DESIGNER_DROP_NONE = 0,
    UMI_WORKBENCH_DESIGNER_DROP_LEFT = 1,
    UMI_WORKBENCH_DESIGNER_DROP_RIGHT = 2,
    UMI_WORKBENCH_DESIGNER_DROP_TOP = 3,
    UMI_WORKBENCH_DESIGNER_DROP_BOTTOM = 4,
    UMI_WORKBENCH_DESIGNER_DROP_CENTRE = 5,
    UMI_WORKBENCH_DESIGNER_DROP_TAB = 6,
    UMI_WORKBENCH_DESIGNER_DROP_FLOATING = 7
} UmiWorkbenchDesignerDropZone;

typedef enum UmiWorkbenchDesignerPropertyKind {
    UMI_WORKBENCH_DESIGNER_PROPERTY_TEXT = 1,
    UMI_WORKBENCH_DESIGNER_PROPERTY_NUMBER = 2,
    UMI_WORKBENCH_DESIGNER_PROPERTY_BOOLEAN = 3,
    UMI_WORKBENCH_DESIGNER_PROPERTY_CHOICE = 4,
    UMI_WORKBENCH_DESIGNER_PROPERTY_RECT = 5,
    UMI_WORKBENCH_DESIGNER_PROPERTY_IDENTIFIER = 6
} UmiWorkbenchDesignerPropertyKind;

typedef enum UmiWorkbenchDesignerCommandKind {
    UMI_WORKBENCH_DESIGNER_COMMAND_NONE = 0,
    UMI_WORKBENCH_DESIGNER_COMMAND_ADD_PANEL = 1,
    UMI_WORKBENCH_DESIGNER_COMMAND_REMOVE_NODE = 2,
    UMI_WORKBENCH_DESIGNER_COMMAND_MOVE_NODE = 3,
    UMI_WORKBENCH_DESIGNER_COMMAND_SET_PROPERTY = 4,
    UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_HORIZONTAL = 5,
    UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_VERTICAL = 6,
    UMI_WORKBENCH_DESIGNER_COMMAND_CREATE_TAB_GROUP = 7,
    UMI_WORKBENCH_DESIGNER_COMMAND_FLOAT_NODE = 8,
    UMI_WORKBENCH_DESIGNER_COMMAND_DOCK_NODE = 9,
    UMI_WORKBENCH_DESIGNER_COMMAND_REORDER_TAB = 10,
    UMI_WORKBENCH_DESIGNER_COMMAND_SET_ACTIVE_TAB = 11,
    UMI_WORKBENCH_DESIGNER_COMMAND_RENAME_LAYOUT = 12,
    UMI_WORKBENCH_DESIGNER_COMMAND_LOCK_LAYOUT = 13,
    UMI_WORKBENCH_DESIGNER_COMMAND_UNLOCK_LAYOUT = 14,
    UMI_WORKBENCH_DESIGNER_COMMAND_UNDO = 15,
    UMI_WORKBENCH_DESIGNER_COMMAND_REDO = 16,
    UMI_WORKBENCH_DESIGNER_COMMAND_SAVE = 17,
    UMI_WORKBENCH_DESIGNER_COMMAND_IMPORT = 18,
    UMI_WORKBENCH_DESIGNER_COMMAND_EXPORT = 19
} UmiWorkbenchDesignerCommandKind;

typedef enum UmiWorkbenchDesignerSaveState {
    UMI_WORKBENCH_DESIGNER_SAVE_CLEAN = 1,
    UMI_WORKBENCH_DESIGNER_SAVE_DIRTY = 2,
    UMI_WORKBENCH_DESIGNER_SAVE_PENDING = 3,
    UMI_WORKBENCH_DESIGNER_SAVE_SAVING = 4,
    UMI_WORKBENCH_DESIGNER_SAVE_FAILED = 5,
    UMI_WORKBENCH_DESIGNER_SAVE_CONFLICT = 6
} UmiWorkbenchDesignerSaveState;

typedef enum UmiWorkbenchDesignerIssueSeverity {
    UMI_WORKBENCH_DESIGNER_ISSUE_INFO = 1,
    UMI_WORKBENCH_DESIGNER_ISSUE_WARNING = 2,
    UMI_WORKBENCH_DESIGNER_ISSUE_ERROR = 3
} UmiWorkbenchDesignerIssueSeverity;

typedef enum UmiWorkbenchDesignerEventKind {
    UMI_WORKBENCH_DESIGNER_EVENT_SESSION_OPENED = 1,
    UMI_WORKBENCH_DESIGNER_EVENT_SESSION_CLOSED = 2,
    UMI_WORKBENCH_DESIGNER_EVENT_SESSION_ACTIVATED = 3,
    UMI_WORKBENCH_DESIGNER_EVENT_SELECTION_CHANGED = 4,
    UMI_WORKBENCH_DESIGNER_EVENT_DOCUMENT_CHANGED = 5,
    UMI_WORKBENCH_DESIGNER_EVENT_COMMAND_EXECUTED = 6,
    UMI_WORKBENCH_DESIGNER_EVENT_SAVE_STARTED = 7,
    UMI_WORKBENCH_DESIGNER_EVENT_SAVE_COMPLETED = 8,
    UMI_WORKBENCH_DESIGNER_EVENT_SAVE_FAILED = 9,
    UMI_WORKBENCH_DESIGNER_EVENT_MODE_CHANGED = 10,
    UMI_WORKBENCH_DESIGNER_EVENT_TOOL_CHANGED = 11,
    UMI_WORKBENCH_DESIGNER_EVENT_CONFLICT_DETECTED = 12
} UmiWorkbenchDesignerEventKind;

typedef struct UmiWorkbenchDesignerPoint {
    double x;
    double y;
} UmiWorkbenchDesignerPoint;

typedef struct UmiWorkbenchDesignerRect {
    double x;
    double y;
    double width;
    double height;
} UmiWorkbenchDesignerRect;

typedef struct UmiWorkbenchDesignerSize {
    double width;
    double height;
} UmiWorkbenchDesignerSize;

typedef struct UmiWorkbenchDesignerIdentifier {
    char value[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
} UmiWorkbenchDesignerIdentifier;

const char *umi_workbench_designer_state_text(UmiWorkbenchDesignerState state);
const char *umi_workbench_designer_mode_text(UmiWorkbenchDesignerMode mode);
const char *umi_workbench_designer_tool_text(UmiWorkbenchDesignerTool tool);
const char *umi_workbench_designer_drop_zone_text(UmiWorkbenchDesignerDropZone zone);
const char *umi_workbench_designer_command_kind_text(UmiWorkbenchDesignerCommandKind kind);
const char *umi_workbench_designer_save_state_text(UmiWorkbenchDesignerSaveState state);
bool umi_workbench_designer_rect_is_valid(const UmiWorkbenchDesignerRect *rect);
bool umi_workbench_designer_rect_contains_point(const UmiWorkbenchDesignerRect *rect, UmiWorkbenchDesignerPoint point);
bool umi_workbench_designer_rect_intersects(const UmiWorkbenchDesignerRect *left, const UmiWorkbenchDesignerRect *right);
UmiWorkbenchDesignerRect umi_workbench_designer_rect_intersection(const UmiWorkbenchDesignerRect *left, const UmiWorkbenchDesignerRect *right);
double umi_workbench_designer_clamp(double value, double minimum, double maximum);

#ifdef __cplusplus
}
#endif

#endif
