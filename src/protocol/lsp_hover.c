/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_hover.c
 *
 * PURPOSE:
 *   Implement the lsp hover behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP hover. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_hover(UmiLspClient *c,const char *u,UmiLspPosition p,int64_t *id){return umi_lsp_request_position(c,"textDocument/hover",u,p,NULL,id);}
