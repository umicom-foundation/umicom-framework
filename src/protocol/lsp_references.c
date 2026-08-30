/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_references.c
 *
 * PURPOSE:
 *   Implement the lsp references behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP references. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_references(UmiLspClient *c,const char *u,UmiLspPosition p,int include,int64_t *id){return umi_lsp_request_position(c,"textDocument/references",u,p,include?",\"context\":{\"includeDeclaration\":true}":",\"context\":{\"includeDeclaration\":false}",id);}
