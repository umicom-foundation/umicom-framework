/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_next.c
 *
 * PURPOSE:
 *   Implement the dap next behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP next. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_next(UmiDapClient*c,int t,int64_t*id){return umi_dap_thread_request(c,"next",t,",\"singleThread\":false",id);}
