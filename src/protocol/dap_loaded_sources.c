/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_loaded_sources.c
 *
 * PURPOSE:
 *   Implement the dap loaded sources behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP loadedSources. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_loaded_sources(UmiDapClient*c,int64_t*id){return umi_dap_request(c,"loadedSources","{}",id);}
