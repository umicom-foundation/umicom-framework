/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_prepare_rename.c
 *
 * PURPOSE:
 *   Implement the lsp prepare rename behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP prepareRename. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_prepare_rename(UmiLspClient *c,const char *u,UmiLspPosition p,int64_t *id){return umi_lsp_request_position(c,"textDocument/prepareRename",u,p,NULL,id);}
