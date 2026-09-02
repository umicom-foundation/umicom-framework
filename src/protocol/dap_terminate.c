/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_terminate.c
 *
 * PURPOSE:
 *   Implement the dap terminate behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP terminate. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
/* Provide the dap terminate operation used by this module and its client applications. */
UmiStatus umi_dap_terminate(UmiDapClient*c,int restart,int64_t*id){return umi_dap_request(c,"terminate",restart?"{\"restart\":true}":"{\"restart\":false}",id);}
