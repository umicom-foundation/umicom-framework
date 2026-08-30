/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/types.h
 *
 * PURPOSE:
 *   Define the stable, toolkit-neutral value types shared by persistent workbench layouts, sessions, history, import/export and recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_TYPES_H
#define UMICOM_WORKBENCH_LAYOUT_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION 2U

#define UMI_WORKBENCH_LAYOUT_ID_CAPACITY 128U
#define UMI_WORKBENCH_LAYOUT_NAME_CAPACITY 192U
#define UMI_WORKBENCH_LAYOUT_DESCRIPTION_CAPACITY 512U
#define UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY 256U
#define UMI_WORKBENCH_LAYOUT_PATH_CAPACITY 512U
#define UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY 512U
#define UMI_WORKBENCH_LAYOUT_TAG_CAPACITY 64U

#define UMI_WORKBENCH_LAYOUT_MAX_NODES 256U
#define UMI_WORKBENCH_LAYOUT_MAX_CHILDREN 16U
#define UMI_WORKBENCH_LAYOUT_MAX_TAGS 16U
#define UMI_WORKBENCH_LAYOUT_MAX_DIAGNOSTICS 128U
#define UMI_WORKBENCH_LAYOUT_MAX_OPERATIONS 256U
#define UMI_WORKBENCH_LAYOUT_MAX_HISTORY 64U
#define UMI_WORKBENCH_LAYOUT_MAX_DIFF_ENTRIES 512U
#define UMI_WORKBENCH_LAYOUT_MAX_CONFLICTS 128U
#define UMI_WORKBENCH_LAYOUT_MAX_TEMPLATES 64U
#define UMI_WORKBENCH_LAYOUT_MAX_PANELS 256U
#define UMI_WORKBENCH_LAYOUT_MAX_PERSPECTIVES 64U
#define UMI_WORKBENCH_LAYOUT_MAX_MONITORS 32U
#define UMI_WORKBENCH_LAYOUT_MAX_BREAKPOINTS 16U
#define UMI_WORKBENCH_LAYOUT_MAX_OPEN_PANELS 128U
#define UMI_WORKBENCH_LAYOUT_MAX_RECOVERY_ENTRIES 256U
#define UMI_WORKBENCH_LAYOUT_MAX_STORE_RECORDS 128U
#define UMI_WORKBENCH_LAYOUT_MAX_BROWSER_RESULTS 128U
#define UMI_WORKBENCH_LAYOUT_MAX_LISTENERS 32U
#define UMI_WORKBENCH_LAYOUT_MAX_COMMAND_ARGUMENTS 16U
#define UMI_WORKBENCH_LAYOUT_MAX_SHARE_ITEMS 64U

#define UMI_WORKBENCH_LAYOUT_INDEX_NONE ((size_t)-1)

typedef enum UmiWorkbenchLayoutNodeKind {
    UMI_WORKBENCH_LAYOUT_NODE_EMPTY = 1,
    UMI_WORKBENCH_LAYOUT_NODE_WINDOW = 2,
    UMI_WORKBENCH_LAYOUT_NODE_SPLIT = 3,
    UMI_WORKBENCH_LAYOUT_NODE_TAB_GROUP = 4,
    UMI_WORKBENCH_LAYOUT_NODE_PANEL = 5,
    UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP = 6,
    UMI_WORKBENCH_LAYOUT_NODE_FLOATING_WINDOW = 7
} UmiWorkbenchLayoutNodeKind;

typedef enum UmiWorkbenchLayoutOrientation {
    UMI_WORKBENCH_LAYOUT_ORIENTATION_NONE = 0,
    UMI_WORKBENCH_LAYOUT_ORIENTATION_HORIZONTAL = 1,
    UMI_WORKBENCH_LAYOUT_ORIENTATION_VERTICAL = 2
} UmiWorkbenchLayoutOrientation;

typedef enum UmiWorkbenchLayoutDockRegion {
    UMI_WORKBENCH_LAYOUT_DOCK_CANVAS = 1,
    UMI_WORKBENCH_LAYOUT_DOCK_DOCUMENT = 2,
    UMI_WORKBENCH_LAYOUT_DOCK_LEFT = 3,
    UMI_WORKBENCH_LAYOUT_DOCK_RIGHT = 4,
    UMI_WORKBENCH_LAYOUT_DOCK_TOP = 5,
    UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM = 6,
    UMI_WORKBENCH_LAYOUT_DOCK_FLOATING = 7
} UmiWorkbenchLayoutDockRegion;

typedef enum UmiWorkbenchLayoutVisibility {
    UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE = 1,
    UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN = 2,
    UMI_WORKBENCH_LAYOUT_VISIBILITY_AUTO = 3
} UmiWorkbenchLayoutVisibility;

typedef enum UmiWorkbenchLayoutSeverity {
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_INFO = 1,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_WARNING = 2,
    UMI_WORKBENCH_LAYOUT_DIAGNOSTIC_ERROR = 3
} UmiWorkbenchLayoutSeverity;

typedef enum UmiWorkbenchLayoutOperationKind {
    UMI_WORKBENCH_LAYOUT_OPERATION_ADD_NODE = 1,
    UMI_WORKBENCH_LAYOUT_OPERATION_REMOVE_NODE = 2,
    UMI_WORKBENCH_LAYOUT_OPERATION_MOVE_NODE = 3,
    UMI_WORKBENCH_LAYOUT_OPERATION_SET_ROOT = 4,
    UMI_WORKBENCH_LAYOUT_OPERATION_SET_SPLIT_RATIO = 5,
    UMI_WORKBENCH_LAYOUT_OPERATION_SET_ACTIVE_CHILD = 6,
    UMI_WORKBENCH_LAYOUT_OPERATION_SET_BOUNDS = 7,
    UMI_WORKBENCH_LAYOUT_OPERATION_SET_VISIBILITY = 8,
    UMI_WORKBENCH_LAYOUT_OPERATION_SET_CONTEXT_GROUP = 9,
    UMI_WORKBENCH_LAYOUT_OPERATION_SET_COMPONENT = 10,
    UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT = 11,
    UMI_WORKBENCH_LAYOUT_OPERATION_SET_LOCKED = 12,
    UMI_WORKBENCH_LAYOUT_OPERATION_SET_DIRTY = 13
} UmiWorkbenchLayoutOperationKind;

typedef enum UmiWorkbenchLayoutDiffKind {
    UMI_WORKBENCH_LAYOUT_DIFF_METADATA_CHANGED = 1,
    UMI_WORKBENCH_LAYOUT_DIFF_NODE_ADDED = 2,
    UMI_WORKBENCH_LAYOUT_DIFF_NODE_REMOVED = 3,
    UMI_WORKBENCH_LAYOUT_DIFF_NODE_MOVED = 4,
    UMI_WORKBENCH_LAYOUT_DIFF_NODE_CHANGED = 5,
    UMI_WORKBENCH_LAYOUT_DIFF_ORDER_CHANGED = 6
} UmiWorkbenchLayoutDiffKind;

typedef enum UmiWorkbenchLayoutConflictKind {
    UMI_WORKBENCH_LAYOUT_CONFLICT_METADATA = 1,
    UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_ADDED_DIFFERENTLY = 2,
    UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_REMOVED_AND_CHANGED = 3,
    UMI_WORKBENCH_LAYOUT_CONFLICT_NODE_CHANGED_DIFFERENTLY = 4,
    UMI_WORKBENCH_LAYOUT_CONFLICT_PARENT_CHANGED_DIFFERENTLY = 5,
    UMI_WORKBENCH_LAYOUT_CONFLICT_ORDER_CHANGED_DIFFERENTLY = 6
} UmiWorkbenchLayoutConflictKind;

typedef enum UmiWorkbenchLayoutMigrationAction {
    UMI_WORKBENCH_LAYOUT_MIGRATION_NONE = 0,
    UMI_WORKBENCH_LAYOUT_MIGRATION_NORMALISE_IDENTIFIERS = 1,
    UMI_WORKBENCH_LAYOUT_MIGRATION_ADD_STRUCTURE_SIZES = 2,
    UMI_WORKBENCH_LAYOUT_MIGRATION_ADD_VISIBILITY = 3,
    UMI_WORKBENCH_LAYOUT_MIGRATION_ADD_MONITOR_PLACEMENT = 4,
    UMI_WORKBENCH_LAYOUT_MIGRATION_REWRITE_SCHEMA_VERSION = 5
} UmiWorkbenchLayoutMigrationAction;

typedef enum UmiWorkbenchLayoutStoreKind {
    UMI_WORKBENCH_LAYOUT_STORE_MEMORY = 1,
    UMI_WORKBENCH_LAYOUT_STORE_DATA_SERVER = 2,
    UMI_WORKBENCH_LAYOUT_STORE_FILE = 3
} UmiWorkbenchLayoutStoreKind;

typedef enum UmiWorkbenchLayoutSortOrder {
    UMI_WORKBENCH_LAYOUT_SORT_NAME_ASCENDING = 1,
    UMI_WORKBENCH_LAYOUT_SORT_NAME_DESCENDING = 2,
    UMI_WORKBENCH_LAYOUT_SORT_RECENT_FIRST = 3,
    UMI_WORKBENCH_LAYOUT_SORT_REVISION_DESCENDING = 4
} UmiWorkbenchLayoutSortOrder;

typedef enum UmiWorkbenchLayoutAccessAction {
    UMI_WORKBENCH_LAYOUT_ACCESS_VIEW = 1,
    UMI_WORKBENCH_LAYOUT_ACCESS_CREATE = 2,
    UMI_WORKBENCH_LAYOUT_ACCESS_EDIT = 3,
    UMI_WORKBENCH_LAYOUT_ACCESS_DELETE = 4,
    UMI_WORKBENCH_LAYOUT_ACCESS_SHARE = 5,
    UMI_WORKBENCH_LAYOUT_ACCESS_LOCK = 6,
    UMI_WORKBENCH_LAYOUT_ACCESS_ADMINISTER = 7
} UmiWorkbenchLayoutAccessAction;

typedef enum UmiWorkbenchLayoutRole {
    UMI_WORKBENCH_LAYOUT_ROLE_VIEWER = 1,
    UMI_WORKBENCH_LAYOUT_ROLE_EDITOR = 2,
    UMI_WORKBENCH_LAYOUT_ROLE_OWNER = 3,
    UMI_WORKBENCH_LAYOUT_ROLE_ADMINISTRATOR = 4
} UmiWorkbenchLayoutRole;

typedef enum UmiWorkbenchLayoutEventKind {
    UMI_WORKBENCH_LAYOUT_EVENT_CREATED = 1,
    UMI_WORKBENCH_LAYOUT_EVENT_UPDATED = 2,
    UMI_WORKBENCH_LAYOUT_EVENT_DELETED = 3,
    UMI_WORKBENCH_LAYOUT_EVENT_ACTIVATED = 4,
    UMI_WORKBENCH_LAYOUT_EVENT_SAVED = 5,
    UMI_WORKBENCH_LAYOUT_EVENT_RESTORED = 6,
    UMI_WORKBENCH_LAYOUT_EVENT_IMPORTED = 7,
    UMI_WORKBENCH_LAYOUT_EVENT_EXPORTED = 8,
    UMI_WORKBENCH_LAYOUT_EVENT_RECOVERY_REQUIRED = 9,
    UMI_WORKBENCH_LAYOUT_EVENT_CONFLICT_DETECTED = 10
} UmiWorkbenchLayoutEventKind;

typedef enum UmiWorkbenchLayoutCommandKind {
    UMI_WORKBENCH_LAYOUT_COMMAND_CREATE = 1,
    UMI_WORKBENCH_LAYOUT_COMMAND_CLONE = 2,
    UMI_WORKBENCH_LAYOUT_COMMAND_DELETE = 3,
    UMI_WORKBENCH_LAYOUT_COMMAND_ACTIVATE = 4,
    UMI_WORKBENCH_LAYOUT_COMMAND_APPLY_OPERATION = 5,
    UMI_WORKBENCH_LAYOUT_COMMAND_UNDO = 6,
    UMI_WORKBENCH_LAYOUT_COMMAND_REDO = 7,
    UMI_WORKBENCH_LAYOUT_COMMAND_SAVE = 8,
    UMI_WORKBENCH_LAYOUT_COMMAND_RESTORE = 9,
    UMI_WORKBENCH_LAYOUT_COMMAND_IMPORT = 10,
    UMI_WORKBENCH_LAYOUT_COMMAND_EXPORT = 11,
    UMI_WORKBENCH_LAYOUT_COMMAND_LOCK = 12,
    UMI_WORKBENCH_LAYOUT_COMMAND_UNLOCK = 13
} UmiWorkbenchLayoutCommandKind;

typedef enum UmiWorkbenchLayoutControllerState {
    UMI_WORKBENCH_LAYOUT_CONTROLLER_CREATED = 1,
    UMI_WORKBENCH_LAYOUT_CONTROLLER_INITIALISED = 2,
    UMI_WORKBENCH_LAYOUT_CONTROLLER_RUNNING = 3,
    UMI_WORKBENCH_LAYOUT_CONTROLLER_QUIESCED = 4,
    UMI_WORKBENCH_LAYOUT_CONTROLLER_STOPPED = 5,
    UMI_WORKBENCH_LAYOUT_CONTROLLER_FAILED = 6
} UmiWorkbenchLayoutControllerState;

typedef struct UmiWorkbenchLayoutRect {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} UmiWorkbenchLayoutRect;

typedef struct UmiWorkbenchLayoutSize {
    int32_t width;
    int32_t height;
} UmiWorkbenchLayoutSize;

typedef struct UmiWorkbenchLayoutPoint {
    int32_t x;
    int32_t y;
} UmiWorkbenchLayoutPoint;

typedef struct UmiWorkbenchLayoutTag {
    char value[UMI_WORKBENCH_LAYOUT_TAG_CAPACITY];
} UmiWorkbenchLayoutTag;

typedef struct UmiWorkbenchLayoutIdentity {
    char layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_user_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
} UmiWorkbenchLayoutIdentity;

typedef struct UmiWorkbenchLayoutVersion {
    uint32_t schema_version;
    uint64_t revision;
    uint64_t generation;
    uint64_t base_revision;
} UmiWorkbenchLayoutVersion;

typedef struct UmiWorkbenchLayoutAudit {
    char created_by[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char modified_by[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t created_at_ms;
    uint64_t modified_at_ms;
    char correlation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
} UmiWorkbenchLayoutAudit;

const char *umi_workbench_layout_node_kind_text(
    UmiWorkbenchLayoutNodeKind kind);
const char *umi_workbench_layout_orientation_text(
    UmiWorkbenchLayoutOrientation orientation);
const char *umi_workbench_layout_dock_region_text(
    UmiWorkbenchLayoutDockRegion region);
const char *umi_workbench_layout_visibility_text(
    UmiWorkbenchLayoutVisibility visibility);
const char *umi_workbench_layout_severity_text(
    UmiWorkbenchLayoutSeverity severity);
const char *umi_workbench_layout_operation_kind_text(
    UmiWorkbenchLayoutOperationKind kind);
const char *umi_workbench_layout_diff_kind_text(
    UmiWorkbenchLayoutDiffKind kind);
const char *umi_workbench_layout_conflict_kind_text(
    UmiWorkbenchLayoutConflictKind kind);
const char *umi_workbench_layout_store_kind_text(
    UmiWorkbenchLayoutStoreKind kind);
const char *umi_workbench_layout_event_kind_text(
    UmiWorkbenchLayoutEventKind kind);
const char *umi_workbench_layout_command_kind_text(
    UmiWorkbenchLayoutCommandKind kind);
const char *umi_workbench_layout_controller_state_text(
    UmiWorkbenchLayoutControllerState state);

bool umi_workbench_layout_rect_is_valid(
    const UmiWorkbenchLayoutRect *rect);
bool umi_workbench_layout_rect_contains(
    const UmiWorkbenchLayoutRect *outer,
    const UmiWorkbenchLayoutRect *inner);
bool umi_workbench_layout_rect_intersects(
    const UmiWorkbenchLayoutRect *left,
    const UmiWorkbenchLayoutRect *right);
UmiWorkbenchLayoutRect umi_workbench_layout_rect_intersection(
    const UmiWorkbenchLayoutRect *left,
    const UmiWorkbenchLayoutRect *right);
int64_t umi_workbench_layout_rect_area(
    const UmiWorkbenchLayoutRect *rect);

#ifdef __cplusplus
}
#endif

#endif
