/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/controller.h
 * PURPOSE: Coordinate DAP state transitions through one reusable controller.
 * Created by: Sammy Hegab | Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_CONTROLLER_H
#define UMICOM_DEBUG_CONTROLLER_H
#include "umicom/debug/service.h"
#include "umicom/debug/capabilities.h"
#include "umicom/protocol/dap_extended.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiDebugControllerState{UMI_DEBUG_CONTROLLER_IDLE=0,UMI_DEBUG_CONTROLLER_INITIALIZING=1,UMI_DEBUG_CONTROLLER_CONFIGURING=2,UMI_DEBUG_CONTROLLER_RUNNING=3,UMI_DEBUG_CONTROLLER_PAUSED=4,UMI_DEBUG_CONTROLLER_TERMINATING=5,UMI_DEBUG_CONTROLLER_TERMINATED=6,UMI_DEBUG_CONTROLLER_FAILED=7}UmiDebugControllerState;
typedef struct UmiDebugControllerSnapshot{UmiDebugControllerState state;int active_thread_id;int active_frame_id;int64_t last_request_id;UmiStatus last_status;uint64_t revision;}UmiDebugControllerSnapshot;
typedef struct UmiDebugController UmiDebugController;
UmiStatus umi_debug_controller_create(UmiDapClient *client,UmiDebugService *service,UmiDebugController **out_controller);
void umi_debug_controller_destroy(UmiDebugController *controller);
UmiStatus umi_debug_controller_initialize(UmiDebugController *controller,const char *adapter_id);
UmiStatus umi_debug_controller_launch(UmiDebugController *controller,const char *program,const char *working_directory);
UmiStatus umi_debug_controller_continue(UmiDebugController *controller,int thread_id);
UmiStatus umi_debug_controller_pause(UmiDebugController *controller,int thread_id);
UmiStatus umi_debug_controller_next(UmiDebugController *controller,int thread_id);
UmiStatus umi_debug_controller_step_in(UmiDebugController *controller,int thread_id);
UmiStatus umi_debug_controller_step_out(UmiDebugController *controller,int thread_id);
UmiStatus umi_debug_controller_terminate(UmiDebugController *controller,int restart);
UmiStatus umi_debug_controller_mark_stopped(UmiDebugController *controller,int thread_id);
UmiStatus umi_debug_controller_snapshot(const UmiDebugController *controller,UmiDebugControllerSnapshot *out_snapshot);
const char *umi_debug_controller_state_text(UmiDebugControllerState state);
#ifdef __cplusplus
}
#endif
#endif
