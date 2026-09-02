/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_dap_extended.c
 *
 * PURPOSE:
 *   Implement the test dap extended behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework extended DAP test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include "umicom/protocol/dap_extended.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiProtocolTransport*t=NULL;UmiProtocolClient*c=NULL;UmiDapClient dap;UmiDapSourceBreakpoint b={12,1,"x > 0","","value={x}"};int64_t id=0;assert(umi_protocol_transport_create_memory(64U,&t)==UMI_STATUS_OK);assert(umi_protocol_client_create(t,&c)==UMI_STATUS_OK);assert(umi_protocol_client_start(c)==UMI_STATUS_OK);assert(umi_dap_client_init(&dap,c)==UMI_STATUS_OK);assert(umi_dap_attach(&dap,42,".",&id)==UMI_STATUS_OK);assert(umi_dap_set_breakpoints(&dap,"main.c",&b,1U,&id)==UMI_STATUS_OK);assert(umi_dap_configuration_done(&dap,&id)==UMI_STATUS_OK);assert(umi_dap_threads(&dap,&id)==UMI_STATUS_OK);assert(umi_dap_stack_trace(&dap,1,0,20,&id)==UMI_STATUS_OK);assert(umi_dap_scopes(&dap,2,&id)==UMI_STATUS_OK);assert(umi_dap_variables(&dap,3,0,100,&id)==UMI_STATUS_OK);assert(umi_dap_evaluate(&dap,"x",2,"watch",&id)==UMI_STATUS_OK);assert(umi_dap_read_memory(&dap,"0x1000",0,64,&id)==UMI_STATUS_OK);assert(umi_dap_disassemble(&dap,"0x1000",0,16,&id)==UMI_STATUS_OK);assert(umi_protocol_transport_count(t)==10U);umi_protocol_client_destroy(c);umi_protocol_transport_destroy(t);return 0;}
