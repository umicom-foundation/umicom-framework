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

/**
 * Represent the debug workbench debug session state model data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugSessionStateModel {
    char session_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    UmiDebugWorkbenchSessionPhase phase;
    uint64_t stop_sequence;
    uint64_t revision;
} UmiDebugWorkbenchDebugSessionStateModel;

/**
 * Initialise debug workbench debug session state model from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_session_state_model_init(UmiDebugWorkbenchDebugSessionStateModel *model, const char *session_id);
/**
 * Provide the debug workbench debug session state model transition operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_state_model_transition(UmiDebugWorkbenchDebugSessionStateModel *model, UmiDebugWorkbenchSessionPhase phase);
/**
 * Provide the debug workbench debug session state model record stop operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_session_state_model_record_stop(UmiDebugWorkbenchDebugSessionStateModel *model);
/**
 * Check that debug workbench debug session state model satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_session_state_model_valid(const UmiDebugWorkbenchDebugSessionStateModel *model);

#ifdef __cplusplus
}
#endif
#endif
