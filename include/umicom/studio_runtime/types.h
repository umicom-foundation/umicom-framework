/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/types.h
 *
 * PURPOSE:
 *   Define the Framework-owned runtime composition records used by a thin
 *   Umicom Studio application. These records connect the generic Application
 *   Shell, IDE Integration platform, DocumentCoordinator and Command Registry.
 *
 * ARCHITECTURE:
 *   Umicom Studio does not own duplicate editor, test, debug, VCS, AI or layout
 *   state. The application renders these Framework-owned records and forwards
 *   user intent to Framework commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_TYPES_H
#define UMICOM_STUDIO_RUNTIME_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_RUNTIME_API_VERSION 1U
#define UMI_STUDIO_RUNTIME_ID_CAPACITY 192U
#define UMI_STUDIO_RUNTIME_TEXT_CAPACITY 1024U
#define UMI_STUDIO_RUNTIME_PATH_CAPACITY 2048U
#define UMI_STUDIO_RUNTIME_STATUS_ITEM_CAPACITY 16U
#define UMI_STUDIO_RUNTIME_LAYOUT_PRESET_CAPACITY 16U
#define UMI_STUDIO_RUNTIME_ACTIVATION_CAPACITY 64U
#define UMI_STUDIO_RUNTIME_ROUTE_CAPACITY 32U

/**
 * List the named studio runtime surface kind values accepted by this public contract.
 */
typedef enum UmiStudioRuntimeSurfaceKind {
    UMI_STUDIO_SURFACE_EXPLORER = 1,
    UMI_STUDIO_SURFACE_EDITOR = 2,
    UMI_STUDIO_SURFACE_OUTLINE = 3,
    UMI_STUDIO_SURFACE_SEARCH = 4,
    UMI_STUDIO_SURFACE_PROBLEMS = 5,
    UMI_STUDIO_SURFACE_OUTPUT = 6,
    UMI_STUDIO_SURFACE_TERMINAL = 7,
    UMI_STUDIO_SURFACE_SOURCE_CONTROL = 8,
    UMI_STUDIO_SURFACE_TEST_EXPLORER = 9,
    UMI_STUDIO_SURFACE_DEBUG = 10,
    UMI_STUDIO_SURFACE_BUILD = 11,
    UMI_STUDIO_SURFACE_AI_CHAT = 12,
    UMI_STUDIO_SURFACE_AI_REVIEW = 13,
    UMI_STUDIO_SURFACE_AI_APPROVALS = 14,
    UMI_STUDIO_SURFACE_AI_TOOLS = 15,
    UMI_STUDIO_SURFACE_AI_VALIDATION = 16,
    UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON = 17
} UmiStudioRuntimeSurfaceKind;

/* Keep bounded surface loops aligned when a reusable Studio panel is added. */
#define UMI_STUDIO_SURFACE_LAST UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON

/**
 * List the named studio runtime selection kind values accepted by this public contract.
 */
typedef enum UmiStudioRuntimeSelectionKind {
    UMI_STUDIO_SELECTION_NONE = 0,
    UMI_STUDIO_SELECTION_PROBLEM = 1,
    UMI_STUDIO_SELECTION_TEST = 2,
    UMI_STUDIO_SELECTION_SOURCE_CONTROL = 3,
    UMI_STUDIO_SELECTION_DEBUG_FRAME = 4,
    UMI_STUDIO_SELECTION_SYMBOL = 5,
    UMI_STUDIO_SELECTION_DIAGNOSTIC = 6,
    UMI_STUDIO_SELECTION_EDITOR = 7,
    UMI_STUDIO_SELECTION_AI_APPROVAL = 8,
    UMI_STUDIO_SELECTION_AI_PATCH_FILE = 9
} UmiStudioRuntimeSelectionKind;

/**
 * List the named studio runtime status kind values accepted by this public contract.
 */
typedef enum UmiStudioRuntimeStatusKind {
    UMI_STUDIO_STATUS_WORKSPACE = 1,
    UMI_STUDIO_STATUS_BRANCH = 2,
    UMI_STUDIO_STATUS_PROBLEMS = 3,
    UMI_STUDIO_STATUS_TESTS = 4,
    UMI_STUDIO_STATUS_BUILD = 5,
    UMI_STUDIO_STATUS_DEBUG = 6,
    UMI_STUDIO_STATUS_AI = 7,
    UMI_STUDIO_STATUS_LANGUAGE = 8,
    UMI_STUDIO_STATUS_ENCODING = 9,
    UMI_STUDIO_STATUS_CURSOR = 10
} UmiStudioRuntimeStatusKind;

/**
 * List the named studio runtime close decision values accepted by this public contract.
 */
typedef enum UmiStudioRuntimeCloseDecision {
    UMI_STUDIO_CLOSE_ALLOW = 0,
    UMI_STUDIO_CLOSE_CONFIRM_DIRTY = 1,
    UMI_STUDIO_CLOSE_CONFIRM_RUNNING = 2,
    UMI_STUDIO_CLOSE_CONFIRM_DIRTY_AND_RUNNING = 3,
    UMI_STUDIO_CLOSE_BLOCKED = 4
} UmiStudioRuntimeCloseDecision;

/**
 * Represent the studio runtime selection data shared with callers of this public contract.
 */
typedef struct UmiStudioRuntimeSelection {
    UmiStudioRuntimeSelectionKind kind;
    char subject_id[UMI_STUDIO_RUNTIME_ID_CAPACITY];
    char path[UMI_STUDIO_RUNTIME_PATH_CAPACITY];
    char label[UMI_STUDIO_RUNTIME_TEXT_CAPACITY];
    size_t index;
    uint32_t line;
    uint32_t column;
    uint64_t revision;
} UmiStudioRuntimeSelection;

/**
 * Represent the studio runtime status item data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeStatusItem {
    UmiStudioRuntimeStatusKind kind;
    char item_id[UMI_STUDIO_RUNTIME_ID_CAPACITY];
    char text[UMI_STUDIO_RUNTIME_TEXT_CAPACITY];
    char command_id[UMI_STUDIO_RUNTIME_ID_CAPACITY];
    uint32_t badge_count;
    int visible;
    int important;
    uint64_t revision;
} UmiStudioRuntimeStatusItem;

/**
 * Represent the studio runtime window title data shared with callers of this public
 * contract.
 */
typedef struct UmiStudioRuntimeWindowTitle {
    char application_name[128];
    char workspace_name[256];
    char document_name[256];
    char title[768];
    int document_dirty;
    uint64_t revision;
} UmiStudioRuntimeWindowTitle;

/**
 * Provide the studio runtime surface kind text operation used by this module and its
 * client applications.
 */
const char *umi_studio_runtime_surface_kind_text(
    UmiStudioRuntimeSurfaceKind kind);

/**
 * Provide the studio runtime selection kind text operation used by this module and its
 * client applications.
 */
const char *umi_studio_runtime_selection_kind_text(
    UmiStudioRuntimeSelectionKind kind);

/**
 * Provide the studio runtime status kind text operation used by this module and its client
 * applications.
 */
const char *umi_studio_runtime_status_kind_text(
    UmiStudioRuntimeStatusKind kind);

/**
 * Provide the studio runtime close decision text operation used by this module and its
 * client applications.
 */
const char *umi_studio_runtime_close_decision_text(
    UmiStudioRuntimeCloseDecision decision);

#ifdef __cplusplus
}
#endif
#endif
