/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workspace.h
 *
 * PURPOSE:
 *   Coordinate the debugger state presented by professional IDE workspaces.
 *   The workspace does not duplicate Debug Adapter Protocol records. It owns
 *   selection and presentation state over the canonical debug registries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEBUG_WORKSPACE_H
#define UMICOM_DEBUG_WORKSPACE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/debug/controller.h"
#include "umicom/debug/service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEBUG_WORKSPACE_API_VERSION 1U
#define UMI_DEBUG_WORKSPACE_ID_CAPACITY 128U
#define UMI_DEBUG_WORKSPACE_LABEL_CAPACITY 256U

/**
 * Represent the debug workspace data shared with callers of this public contract.
 */
typedef struct UmiDebugWorkspace UmiDebugWorkspace;

/*
 * A snapshot is deliberately small enough to copy by value. Detailed records
 * remain in the existing registries and are read through the typed *_at APIs.
 */
typedef struct UmiDebugWorkspaceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    UmiDebugControllerState controller_state;
    char controller_state_label[UMI_DEBUG_WORKSPACE_LABEL_CAPACITY];
    char selected_thread_id[UMI_DEBUG_WORKSPACE_ID_CAPACITY];
    char selected_frame_id[UMI_DEBUG_WORKSPACE_ID_CAPACITY];
    char selected_scope_id[UMI_DEBUG_WORKSPACE_ID_CAPACITY];
    size_t thread_count;
    size_t visible_frame_count;
    size_t visible_scope_count;
    size_t visible_variable_count;
    size_t watch_count;
    size_t breakpoint_count;
    size_t console_entry_count;
    int can_start;
    int can_continue;
    int can_pause;
    int can_step;
    int can_stop;
    int follows_active_thread;
} UmiDebugWorkspaceSnapshot;

/**
 * Initialise debug workspace from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_workspace_create(
    UmiDebugService *service,
    UmiDebugController *controller,
    UmiDebugWorkspace **out_workspace
);
/**
 * Release or reset state held by debug workspace so the same storage can be reused safely.
 */
void umi_debug_workspace_destroy(UmiDebugWorkspace *workspace);

/**
 * Provide the debug workspace refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_refresh(UmiDebugWorkspace *workspace);
/**
 * Provide the debug workspace snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_snapshot(
    UmiDebugWorkspace *workspace,
    UmiDebugWorkspaceSnapshot *out_snapshot
);

/**
 * Provide the debug workspace select thread operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_select_thread(
    UmiDebugWorkspace *workspace,
    const char *thread_id
);
/**
 * Provide the debug workspace select frame operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_select_frame(
    UmiDebugWorkspace *workspace,
    const char *frame_id
);
/**
 * Provide the debug workspace select scope operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_select_scope(
    UmiDebugWorkspace *workspace,
    const char *scope_id
);
/**
 * Provide the debug workspace set follow active thread operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workspace_set_follow_active_thread(
    UmiDebugWorkspace *workspace,
    int enabled
);

/**
 * Provide the debug workspace add watch operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_add_watch(
    UmiDebugWorkspace *workspace,
    const char *expression,
    char *out_watch_id,
    size_t out_watch_id_capacity
);
/**
 * Provide the debug workspace remove watch operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workspace_remove_watch(
    UmiDebugWorkspace *workspace,
    const char *watch_id
);
/**
 * Provide the debug workspace clear watches operation used by this module and its client
 * applications.
 */
void umi_debug_workspace_clear_watches(UmiDebugWorkspace *workspace);

/**
 * Provide the debug workspace set breakpoint enabled operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_workspace_set_breakpoint_enabled(
    UmiDebugWorkspace *workspace,
    const char *breakpoint_id,
    int enabled
);
/**
 * Provide the debug workspace remove breakpoint operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_workspace_remove_breakpoint(
    UmiDebugWorkspace *workspace,
    const char *breakpoint_id
);
/**
 * Provide the debug workspace clear breakpoints operation used by this module and its
 * client applications.
 */
void umi_debug_workspace_clear_breakpoints(UmiDebugWorkspace *workspace);
/**
 * Provide the debug workspace clear console operation used by this module and its client
 * applications.
 */
void umi_debug_workspace_clear_console(UmiDebugWorkspace *workspace);

/**
 * Find debug workspace thread while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_workspace_thread_at(
    UmiDebugWorkspace *workspace,
    size_t index,
    UmiDebugThreadSnapshot *out_thread
);
/**
 * Find debug workspace frame while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_workspace_frame_at(
    UmiDebugWorkspace *workspace,
    size_t visible_index,
    UmiDebugStackFrameSnapshot *out_frame
);
/**
 * Find debug workspace scope while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_workspace_scope_at(
    UmiDebugWorkspace *workspace,
    size_t visible_index,
    UmiDebugScopeSnapshot *out_scope
);
/**
 * Find debug workspace variable while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_workspace_variable_at(
    UmiDebugWorkspace *workspace,
    size_t visible_index,
    UmiDebugVariableSnapshot *out_variable
);
/**
 * Find debug workspace watch while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_workspace_watch_at(
    UmiDebugWorkspace *workspace,
    size_t index,
    UmiDebugWatchSnapshot *out_watch
);
/**
 * Find debug workspace breakpoint while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_workspace_breakpoint_at(
    UmiDebugWorkspace *workspace,
    size_t index,
    UmiDebugBreakpointSnapshot *out_breakpoint
);
/**
 * Find debug workspace console entry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_debug_workspace_console_entry_at(
    UmiDebugWorkspace *workspace,
    size_t index,
    UmiDebugConsoleEntrySnapshot *out_entry
);

#ifdef __cplusplus
}
#endif

#endif
