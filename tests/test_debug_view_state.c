/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_view_state.c
 *
 * PURPOSE:
 *   Implement the test debug view state behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework debugger view-state test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include "umicom/debug/view_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiProtocolTransport*t=NULL;UmiProtocolClient*c=NULL;UmiDapClient dap;UmiDebugService*s=NULL;UmiDebugController*d=NULL;UmiDebugViewState v;assert(umi_protocol_transport_create_memory(16U,&t)==UMI_STATUS_OK);assert(umi_protocol_client_create(t,&c)==UMI_STATUS_OK);assert(umi_protocol_client_start(c)==UMI_STATUS_OK);assert(umi_dap_client_init(&dap,c)==UMI_STATUS_OK);assert(umi_debug_service_create(&s)==UMI_STATUS_OK);assert(umi_debug_controller_create(&dap,s,&d)==UMI_STATUS_OK);assert(umi_debug_view_state_build(s,d,&v)==UMI_STATUS_OK);assert(v.active_thread_id==-1);umi_debug_controller_destroy(d);umi_debug_service_destroy(s);umi_protocol_client_destroy(c);umi_protocol_transport_destroy(t);return 0;}
