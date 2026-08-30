/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_session_state_model.h
 *
 * PURPOSE:
 *   Track debugger lifecycle state and enforce legal high-level phase transitions.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_STATE_MODEL_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_STATE_MODEL_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugSessionStateModel {
    char session_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    UmiDebugWorkbenchSessionPhase phase;
    uint64_t stop_sequence;
    uint64_t revision;
} UmiDebugWorkbenchDebugSessionStateModel;

UmiStatus umi_debug_workbench_debug_session_state_model_init(UmiDebugWorkbenchDebugSessionStateModel *model, const char *session_id);
UmiStatus umi_debug_workbench_debug_session_state_model_transition(UmiDebugWorkbenchDebugSessionStateModel *model, UmiDebugWorkbenchSessionPhase phase);
UmiStatus umi_debug_workbench_debug_session_state_model_record_stop(UmiDebugWorkbenchDebugSessionStateModel *model);
int umi_debug_workbench_debug_session_state_model_valid(const UmiDebugWorkbenchDebugSessionStateModel *model);

#ifdef __cplusplus
}
#endif
#endif
