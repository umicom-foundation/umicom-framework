/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_document_symbols.c
 *
 * PURPOSE:
 *   Implement the lsp document symbols behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP documentSymbol. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_document_symbols(UmiLspClient *c,const char *u,int64_t *id){return umi_lsp_request_document(c,"textDocument/documentSymbol",u,id);}
