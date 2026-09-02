/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/controller.h
 *
 * PURPOSE:
 *   Publish the public controller contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_CONTROLLER_H
#define UMICOM_DEBUG_CONTROLLER_H
#include "umicom/debug/service.h"
#include "umicom/debug/capabilities.h"
#include "umicom/protocol/dap_extended.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named debug controller state values accepted by this public contract.
 */
typedef enum UmiDebugControllerState{UMI_DEBUG_CONTROLLER_IDLE=0,UMI_DEBUG_CONTROLLER_INITIALIZING=1,UMI_DEBUG_CONTROLLER_CONFIGURING=2,UMI_DEBUG_CONTROLLER_RUNNING=3,UMI_DEBUG_CONTROLLER_PAUSED=4,UMI_DEBUG_CONTROLLER_TERMINATING=5,UMI_DEBUG_CONTROLLER_TERMINATED=6,UMI_DEBUG_CONTROLLER_FAILED=7}UmiDebugControllerState;
/**
 * Represent the debug controller snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDebugControllerSnapshot{UmiDebugControllerState state;int active_thread_id;int active_frame_id;int64_t last_request_id;UmiStatus last_status;uint64_t revision;}UmiDebugControllerSnapshot;
/**
 * Represent the debug controller data shared with callers of this public contract.
 */
typedef struct UmiDebugController UmiDebugController;
/**
 * Initialise debug controller from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_controller_create(UmiDapClient *client,UmiDebugService *service,UmiDebugController **out_controller);
/**
 * Release or reset state held by debug controller so the same storage can be reused
 * safely.
 */
void umi_debug_controller_destroy(UmiDebugController *controller);
/**
 * Initialise debug controller from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_controller_initialize(UmiDebugController *controller,const char *adapter_id);
/**
 * Provide the debug controller launch operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_launch(UmiDebugController *controller,const char *program,const char *working_directory);
/**
 * Provide the debug controller continue operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_continue(UmiDebugController *controller,int thread_id);
/**
 * Provide the debug controller pause operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_pause(UmiDebugController *controller,int thread_id);
/**
 * Provide the debug controller next operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_next(UmiDebugController *controller,int thread_id);
/**
 * Provide the debug controller step in operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_step_in(UmiDebugController *controller,int thread_id);
/**
 * Provide the debug controller step out operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_step_out(UmiDebugController *controller,int thread_id);
/**
 * Provide the debug controller terminate operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_terminate(UmiDebugController *controller,int restart);
/**
 * Provide the debug controller mark stopped operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_mark_stopped(UmiDebugController *controller,int thread_id);
/**
 * Provide the debug controller snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_snapshot(const UmiDebugController *controller,UmiDebugControllerSnapshot *out_snapshot);
/**
 * Provide the debug controller state text operation used by this module and its client
 * applications.
 */
const char *umi_debug_controller_state_text(UmiDebugControllerState state);
#ifdef __cplusplus
}
#endif
#endif
