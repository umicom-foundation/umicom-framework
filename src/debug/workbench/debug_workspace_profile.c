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

static uint64_t view_bit(UmiDebugWorkbenchViewKind view)
{ if (view < UMI_DEBUG_WORKBENCH_VIEW_BREAKPOINTS || view > UMI_DEBUG_WORKBENCH_VIEW_TIMELINE) return 0U; return UINT64_C(1) << (unsigned int)(view - UMI_DEBUG_WORKBENCH_VIEW_BREAKPOINTS); }
UmiStatus umi_debug_workbench_debug_workspace_profile_init(UmiDebugWorkbenchDebugWorkspaceProfile *model, const char *workspace_id)
{ if (model == NULL || !umi_debug_workbench_id_valid(workspace_id)) return UMI_STATUS_INVALID_ARGUMENT; memset(model, 0, sizeof *model); if (umi_debug_workbench_copy_text(model->workspace_id, sizeof model->workspace_id, workspace_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; model->primary_view = UMI_DEBUG_WORKBENCH_VIEW_VARIABLES; model->follow_instruction_pointer = true; model->revision = 1U; return UMI_STATUS_OK; }
UmiStatus umi_debug_workbench_debug_workspace_profile_set_primary_view(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view)
{ if (model == NULL || view_bit(view) == 0U) return UMI_STATUS_INVALID_ARGUMENT; model->primary_view = view; model->revision++; return UMI_STATUS_OK; }
UmiStatus umi_debug_workbench_debug_workspace_profile_set_view_visible(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view, bool visible)
{ uint64_t bit; if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT; bit = view_bit(view); if (bit == 0U) return UMI_STATUS_INVALID_ARGUMENT; if (visible) model->visible_views |= bit; else model->visible_views &= ~bit; model->revision++; return UMI_STATUS_OK; }
int umi_debug_workbench_debug_workspace_profile_view_visible(const UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view)
{ uint64_t bit = view_bit(view); return model != NULL && bit != 0U && (model->visible_views & bit) != 0U; }
int umi_debug_workbench_debug_workspace_profile_valid(const UmiDebugWorkbenchDebugWorkspaceProfile *model)
{ return model != NULL && umi_debug_workbench_id_valid(model->workspace_id) && view_bit(model->primary_view) != 0U && model->revision > 0U; }
