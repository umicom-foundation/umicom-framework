/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_pause.c
 *
 * PURPOSE:
 *   Implement the dap pause behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP pause. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
/* Provide the dap pause operation used by this module and its client applications. */
UmiStatus umi_dap_pause(UmiDapClient*c,int t,int64_t*id){return umi_dap_thread_request(c,"pause",t,NULL,id);}
