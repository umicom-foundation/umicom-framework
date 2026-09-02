/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_configuration_done.c
 *
 * PURPOSE:
 *   Implement the dap configuration done behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP configurationDone. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
/*
 * Provide the dap configuration done operation used by this module and its client
 * applications.
 */
UmiStatus umi_dap_configuration_done(UmiDapClient *c,int64_t *id){return umi_dap_request(c,"configurationDone","{}",id);}
