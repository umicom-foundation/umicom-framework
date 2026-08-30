/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_restart.c
 *
 * PURPOSE:
 *   Implement the dap restart behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP restart. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_restart(UmiDapClient*c,int64_t*id){return umi_dap_request(c,"restart","{}",id);}
