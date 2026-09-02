/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_workspace_symbols.c
 *
 * PURPOSE:
 *   Implement the lsp workspace symbols behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP workspace/symbol. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
#include <stdio.h>
/*
 * Provide the lsp workspace symbols operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_workspace_symbols(UmiLspClient *c,const char *q,int64_t *id){char e[2048],p[2304];int n;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->client==NULL||q==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_lsp_escape_text(q,e,sizeof(e));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;n=snprintf(p,sizeof(p),"{\"query\":\"%s\"}",e);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_protocol_client_request(c->client,"workspace/symbol",p,id);}
