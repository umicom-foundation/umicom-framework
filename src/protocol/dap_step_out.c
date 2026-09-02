/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_step_out.c
 *
 * PURPOSE:
 *   Implement the dap step out behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP stepOut. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
/* Provide the dap step out operation used by this module and its client applications. */
UmiStatus umi_dap_step_out(UmiDapClient*c,int t,int64_t*id){return umi_dap_thread_request(c,"stepOut",t,",\"singleThread\":false",id);}
