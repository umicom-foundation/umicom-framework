/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_signature_help.c
 *
 * PURPOSE:
 *   Implement the lsp signature help behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP signatureHelp. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_signature_help(UmiLspClient *c,const char *u,UmiLspPosition p,int64_t *id){return umi_lsp_request_position(c,"textDocument/signatureHelp",u,p,NULL,id);}
