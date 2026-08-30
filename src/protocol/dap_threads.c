/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_threads.c
 *
 * PURPOSE:
 *   Implement the dap threads behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP threads. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_threads(UmiDapClient*c,int64_t*id){return umi_dap_request(c,"threads","{}",id);}
