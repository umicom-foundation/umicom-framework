/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_workspace_profile.c
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
#include "umicom/debug/workbench/debug_workspace_profile.h"
#include <string.h>

/* Provide the view bit operation used by this module and its client applications. */
static uint64_t view_bit(UmiDebugWorkbenchViewKind view)
{ /* Apply this branch only when its contract condition is satisfied. */ if (view < UMI_DEBUG_WORKBENCH_VIEW_BREAKPOINTS || view > UMI_DEBUG_WORKBENCH_VIEW_TIMELINE) return 0U; return UINT64_C(1) << (unsigned int)(view - UMI_DEBUG_WORKBENCH_VIEW_BREAKPOINTS); }
/*
 * Initialise debug workbench debug workspace profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_init(UmiDebugWorkbenchDebugWorkspaceProfile *model, const char *workspace_id)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL || !umi_debug_workbench_id_valid(workspace_id)) return UMI_STATUS_INVALID_ARGUMENT; memset(model, 0, sizeof *model); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (umi_debug_workbench_copy_text(model->workspace_id, sizeof model->workspace_id, workspace_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; model->primary_view = UMI_DEBUG_WORKBENCH_VIEW_VARIABLES; model->follow_instruction_pointer = true; model->revision = 1U; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench debug workspace profile set primary view operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_set_primary_view(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL || view_bit(view) == 0U) return UMI_STATUS_INVALID_ARGUMENT; model->primary_view = view; model->revision++; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench debug workspace profile set view visible operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_set_view_visible(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view, bool visible)
{ uint64_t bit; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT; bit = view_bit(view); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (bit == 0U) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (visible) model->visible_views |= bit; /* Use this fallback path when the earlier condition does not apply. */ else model->visible_views &= ~bit; model->revision++; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench debug workspace profile view visible operation used by this
 * module and its client applications.
 */
int umi_debug_workbench_debug_workspace_profile_view_visible(const UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view)
{ uint64_t bit = view_bit(view); return model != NULL && bit != 0U && (model->visible_views & bit) != 0U; }
/*
 * Check that debug workbench debug workspace profile satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_workspace_profile_valid(const UmiDebugWorkbenchDebugWorkspaceProfile *model)
{ return model != NULL && umi_debug_workbench_id_valid(model->workspace_id) && view_bit(model->primary_view) != 0U && model->revision > 0U; }
