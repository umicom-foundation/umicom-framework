/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_code_action.c
 *
 * PURPOSE:
 *   Implement the lsp code action behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP codeAction. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
/* Provide the lsp code action operation used by this module and its client applications. */
UmiStatus umi_lsp_code_action(UmiLspClient *c,const char *u,UmiLspRange r,int64_t *id){return umi_lsp_request_range(c,"textDocument/codeAction",u,r,",\"context\":{\"diagnostics\":[]}",id);}
