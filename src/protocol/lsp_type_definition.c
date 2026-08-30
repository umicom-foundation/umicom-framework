/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_type_definition.c
 *
 * PURPOSE:
 *   Implement the lsp type definition behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP typeDefinition. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_type_definition(UmiLspClient *c,const char *u,UmiLspPosition p,int64_t *id){return umi_lsp_request_position(c,"textDocument/typeDefinition",u,p,NULL,id);}
