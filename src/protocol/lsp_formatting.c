/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_formatting.c
 *
 * PURPOSE:
 *   Implement the lsp formatting behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP formatting. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
#include <stdio.h>
/* Provide the lsp formatting operation used by this module and its client applications. */
UmiStatus umi_lsp_formatting(UmiLspClient *c,const char *u,uint32_t tab,int spaces,int64_t *id){char e[UMI_PROTOCOL_URI_CAPACITY*2U],p[UMI_PROTOCOL_MESSAGE_CAPACITY];int n;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->client==NULL||u==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_lsp_escape_text(u,e,sizeof(e));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;n=snprintf(p,sizeof(p),"{\"textDocument\":{\"uri\":\"%s\"},\"options\":{\"tabSize\":%u,\"insertSpaces\":%s}}",e,tab,spaces?"true":"false");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_protocol_client_request(c->client,"textDocument/formatting",p,id);}
