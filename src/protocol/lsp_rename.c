/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_rename.c
 *
 * PURPOSE:
 *   Implement the lsp rename behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP rename. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
#include <stdio.h>
UmiStatus umi_lsp_rename(UmiLspClient *c,const char *u,UmiLspPosition p,const char *name,int64_t *id){char e[1024],suffix[1152];int n;UmiStatus s=umi_lsp_escape_text(name,e,sizeof(e));if(s!=UMI_STATUS_OK)return s;n=snprintf(suffix,sizeof(suffix),",\"newName\":\"%s\"",e);if(n<0||(size_t)n>=sizeof(suffix))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_lsp_request_position(c,"textDocument/rename",u,p,suffix,id);}
