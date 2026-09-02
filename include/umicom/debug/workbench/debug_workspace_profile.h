/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_workspace_profile.h
 *
 * PURPOSE:
 *   Persist per-workspace debugger layout and presentation preferences.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_WORKSPACE_PROFILE_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_WORKSPACE_PROFILE_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug workspace profile data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugWorkspaceProfile {
    char workspace_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    UmiDebugWorkbenchViewKind primary_view;
    uint64_t visible_views;
    bool follow_instruction_pointer;
    bool open_console_on_output;
    uint64_t revision;
} UmiDebugWorkbenchDebugWorkspaceProfile;

/**
 * Initialise debug workbench debug workspace profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_init(UmiDebugWorkbenchDebugWorkspaceProfile *model, const char *workspace_id);
/**
 * Provide the debug workbench debug workspace profile set primary view operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_set_primary_view(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view);
/**
 * Provide the debug workbench debug workspace profile set view visible operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_set_view_visible(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view, bool visible);
/**
 * Provide the debug workbench debug workspace profile view visible operation used by this
 * module and its client applications.
 */
int umi_debug_workbench_debug_workspace_profile_view_visible(const UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view);
/**
 * Check that debug workbench debug workspace profile satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_workspace_profile_valid(const UmiDebugWorkbenchDebugWorkspaceProfile *model);

#ifdef __cplusplus
}
#endif
#endif
