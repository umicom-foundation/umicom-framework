/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_status_model.c
 *
 * PURPOSE:
 *   Aggregate active session, stop reason and inspection-count status for workbench chrome.
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
#include "umicom/debug/workbench/debug_status_model.h"
#include <string.h>

/*
 * Initialise debug workbench debug status model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_status_model_init(UmiDebugWorkbenchDebugStatusModel *model, const char *session_id)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL || !umi_debug_workbench_id_valid(session_id)) return UMI_STATUS_INVALID_ARGUMENT; memset(model, 0, sizeof *model); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (umi_debug_workbench_copy_text(model->session_id, sizeof model->session_id, session_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; model->phase = UMI_DEBUG_WORKBENCH_SESSION_IDLE; model->revision = 1U; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench debug status model update operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_debug_status_model_update(UmiDebugWorkbenchDebugStatusModel *model, UmiDebugWorkbenchSessionPhase phase, const char *stop_reason, uint32_t threads, uint32_t frames, uint32_t variables)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL || phase < UMI_DEBUG_WORKBENCH_SESSION_IDLE || phase > UMI_DEBUG_WORKBENCH_SESSION_FAILED) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (stop_reason != NULL && stop_reason[0] != '\0' && umi_debug_workbench_copy_text(model->stop_reason, sizeof model->stop_reason, stop_reason) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED; model->phase = phase; model->thread_count = threads; model->frame_count = frames; model->variable_count = variables; model->revision++; return UMI_STATUS_OK; }
/*
 * Check that debug workbench debug status model satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_status_model_valid(const UmiDebugWorkbenchDebugStatusModel *model)
{ return model != NULL && umi_debug_workbench_id_valid(model->session_id) && model->phase >= UMI_DEBUG_WORKBENCH_SESSION_IDLE && model->phase <= UMI_DEBUG_WORKBENCH_SESSION_FAILED && model->revision > 0U; }
