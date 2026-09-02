/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/controller.c
 *
 * PURPOSE:
 *   Implement the controller behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework reusable debugger controller.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/debug/controller.h"
#include <stdlib.h>
#include <string.h>
struct UmiDebugController{UmiDapClient*client;UmiDebugService*service;UmiDebugCapabilitySet capabilities;UmiDebugControllerSnapshot snapshot;};
/* Provide the record operation used by this module and its client applications. */
static UmiStatus record(UmiDebugController*c,UmiStatus s,int64_t id,UmiDebugControllerState ok){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;c->snapshot.last_status=s;c->snapshot.last_request_id=id;c->snapshot.state=s==UMI_STATUS_OK?ok:UMI_DEBUG_CONTROLLER_FAILED;c->snapshot.revision+=1U;return s;}
/*
 * Initialise debug controller from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_controller_create(UmiDapClient*client,UmiDebugService*service,UmiDebugController**out){UmiDebugController*c;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(client==NULL||service==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;c=calloc(1U,sizeof(*c));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_OUT_OF_MEMORY;c->client=client;c->service=service;c->snapshot.state=UMI_DEBUG_CONTROLLER_IDLE;c->snapshot.active_thread_id=-1;c->snapshot.active_frame_id=-1;c->snapshot.last_status=UMI_STATUS_OK;c->snapshot.revision=1U;umi_debug_capability_set_init(&c->capabilities);*out=c;return UMI_STATUS_OK;}
/*
 * Release or reset state held by debug controller so the same storage can be reused
 * safely.
 */
void umi_debug_controller_destroy(UmiDebugController*c){free(c);}
/*
 * Initialise debug controller from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_debug_controller_initialize(UmiDebugController*c,const char*adapter){int64_t id=0;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||adapter==NULL||adapter[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->snapshot.state!=UMI_DEBUG_CONTROLLER_IDLE&&c->snapshot.state!=UMI_DEBUG_CONTROLLER_TERMINATED)return UMI_STATUS_INVALID_STATE;c->snapshot.state=UMI_DEBUG_CONTROLLER_INITIALIZING;s=umi_dap_initialize(c->client,adapter,&id);return record(c,s,id,UMI_DEBUG_CONTROLLER_CONFIGURING);}
/*
 * Provide the debug controller launch operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_launch(UmiDebugController*c,const char*program,const char*cwd){int64_t id=0;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->snapshot.state!=UMI_DEBUG_CONTROLLER_CONFIGURING)return UMI_STATUS_INVALID_STATE;s=umi_dap_launch(c->client,program,cwd,&id);return record(c,s,id,UMI_DEBUG_CONTROLLER_RUNNING);}
/*
 * Provide the debug controller continue operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_continue(UmiDebugController*c,int t){int64_t id=0;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->snapshot.state!=UMI_DEBUG_CONTROLLER_PAUSED)return UMI_STATUS_INVALID_STATE;s=umi_dap_continue(c->client,t,&id);c->snapshot.active_thread_id=t;return record(c,s,id,UMI_DEBUG_CONTROLLER_RUNNING);}
/*
 * Provide the debug controller pause operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_pause(UmiDebugController*c,int t){int64_t id=0;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->snapshot.state!=UMI_DEBUG_CONTROLLER_RUNNING)return UMI_STATUS_INVALID_STATE;s=umi_dap_pause(c->client,t,&id);c->snapshot.active_thread_id=t;return record(c,s,id,UMI_DEBUG_CONTROLLER_PAUSED);}
/*
 * Provide the debug controller next operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_next(UmiDebugController*c,int t){int64_t id=0;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->snapshot.state!=UMI_DEBUG_CONTROLLER_PAUSED)return UMI_STATUS_INVALID_STATE;s=umi_dap_next(c->client,t,&id);return record(c,s,id,UMI_DEBUG_CONTROLLER_RUNNING);}
/*
 * Provide the debug controller step in operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_step_in(UmiDebugController*c,int t){int64_t id=0;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->snapshot.state!=UMI_DEBUG_CONTROLLER_PAUSED)return UMI_STATUS_INVALID_STATE;s=umi_dap_step_in(c->client,t,&id);return record(c,s,id,UMI_DEBUG_CONTROLLER_RUNNING);}
/*
 * Provide the debug controller step out operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_step_out(UmiDebugController*c,int t){int64_t id=0;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->snapshot.state!=UMI_DEBUG_CONTROLLER_PAUSED)return UMI_STATUS_INVALID_STATE;s=umi_dap_step_out(c->client,t,&id);return record(c,s,id,UMI_DEBUG_CONTROLLER_RUNNING);}
/*
 * Provide the debug controller terminate operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_terminate(UmiDebugController*c,int restart){int64_t id=0;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->snapshot.state==UMI_DEBUG_CONTROLLER_IDLE||c->snapshot.state==UMI_DEBUG_CONTROLLER_TERMINATED)return UMI_STATUS_INVALID_STATE;c->snapshot.state=UMI_DEBUG_CONTROLLER_TERMINATING;s=umi_dap_terminate(c->client,restart,&id);return record(c,s,id,UMI_DEBUG_CONTROLLER_TERMINATED);}
/*
 * Provide the debug controller mark stopped operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_mark_stopped(UmiDebugController*c,int t){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->snapshot.state!=UMI_DEBUG_CONTROLLER_RUNNING)return UMI_STATUS_INVALID_STATE;c->snapshot.state=UMI_DEBUG_CONTROLLER_PAUSED;c->snapshot.active_thread_id=t;c->snapshot.revision+=1U;return UMI_STATUS_OK;}
/*
 * Provide the debug controller snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_controller_snapshot(const UmiDebugController*c,UmiDebugControllerSnapshot*out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=c->snapshot;return UMI_STATUS_OK;}
/*
 * Provide the debug controller state text operation used by this module and its client
 * applications.
 */
const char*umi_debug_controller_state_text(UmiDebugControllerState s){/* Select the behaviour associated with the requested command or state value. */ switch(s){case UMI_DEBUG_CONTROLLER_IDLE:return"idle";case UMI_DEBUG_CONTROLLER_INITIALIZING:return"initializing";case UMI_DEBUG_CONTROLLER_CONFIGURING:return"configuring";case UMI_DEBUG_CONTROLLER_RUNNING:return"running";case UMI_DEBUG_CONTROLLER_PAUSED:return"paused";case UMI_DEBUG_CONTROLLER_TERMINATING:return"terminating";case UMI_DEBUG_CONTROLLER_TERMINATED:return"terminated";case UMI_DEBUG_CONTROLLER_FAILED:return"failed";default:return"unknown";}}
