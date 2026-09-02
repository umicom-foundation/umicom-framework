/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_document_highlights.c
 *
 * PURPOSE:
 *   Implement the lsp document highlights behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP documentHighlight. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
/*
 * Provide the lsp document highlights operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_document_highlights(UmiLspClient *c,const char *u,UmiLspPosition p,int64_t *id){return umi_lsp_request_position(c,"textDocument/documentHighlight",u,p,NULL,id);}
