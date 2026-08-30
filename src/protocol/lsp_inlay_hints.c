/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_inlay_hints.c
 *
 * PURPOSE:
 *   Implement the lsp inlay hints behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP inlayHint. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_inlay_hints(UmiLspClient *c,const char *u,UmiLspRange r,int64_t *id){return umi_lsp_request_range(c,"textDocument/inlayHint",u,r,NULL,id);}
