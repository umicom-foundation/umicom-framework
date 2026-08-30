/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_range_formatting.c
 *
 * PURPOSE:
 *   Implement the lsp range formatting behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP rangeFormatting. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
#include <stdio.h>
UmiStatus umi_lsp_range_formatting(UmiLspClient *c,const char *u,UmiLspRange r,uint32_t tab,int spaces,int64_t *id){char suffix[256];int n=snprintf(suffix,sizeof(suffix),",\"options\":{\"tabSize\":%u,\"insertSpaces\":%s}",tab,spaces?"true":"false");if(n<0||(size_t)n>=sizeof(suffix))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_lsp_request_range(c,"textDocument/rangeFormatting",u,r,suffix,id);}
