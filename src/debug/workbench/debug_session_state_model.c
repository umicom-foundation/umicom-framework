/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_session_state_model.c
 *
 * PURPOSE:
 *   Track debugger lifecycle state and enforce legal high-level phase transitions.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/debug_session_state_model.h"
#include <string.h>

UmiStatus umi_debug_workbench_debug_session_state_model_init(UmiDebugWorkbenchDebugSessionStateModel *model, const char *session_id)
{
    if (model == NULL || !umi_debug_workbench_id_valid(session_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    if (umi_debug_workbench_copy_text(model->session_id, sizeof model->session_id, session_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    model->phase = UMI_DEBUG_WORKBENCH_SESSION_IDLE;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_debug_session_state_model_transition(UmiDebugWorkbenchDebugSessionStateModel *model, UmiDebugWorkbenchSessionPhase phase)
{
    if (!umi_debug_workbench_debug_session_state_model_valid(model)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_debug_workbench_session_transition_allowed(model->phase, phase)) return UMI_STATUS_INVALID_STATE;
    model->phase = phase;
    model->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_debug_session_state_model_record_stop(UmiDebugWorkbenchDebugSessionStateModel *model)
{
    if (!umi_debug_workbench_debug_session_state_model_valid(model)) return UMI_STATUS_INVALID_ARGUMENT;
    model->stop_sequence++;
    model->revision++;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_debug_session_state_model_valid(const UmiDebugWorkbenchDebugSessionStateModel *model)
{
    return model != NULL && umi_debug_workbench_id_valid(model->session_id) && model->phase >= UMI_DEBUG_WORKBENCH_SESSION_IDLE && model->phase <= UMI_DEBUG_WORKBENCH_SESSION_FAILED && model->revision > 0U;
}
