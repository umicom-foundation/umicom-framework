/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_implementation.c
 *
 * PURPOSE:
 *   Implement the lsp implementation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP implementation. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_implementation(UmiLspClient *c,const char *u,UmiLspPosition p,int64_t *id){return umi_lsp_request_position(c,"textDocument/implementation",u,p,NULL,id);}
