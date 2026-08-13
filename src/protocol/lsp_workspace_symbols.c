/* Umicom Framework LSP workspace/symbol. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
#include <stdio.h>
UmiStatus umi_lsp_workspace_symbols(UmiLspClient *c,const char *q,int64_t *id){char e[2048],p[2304];int n;UmiStatus s;if(c==NULL||c->client==NULL||q==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_lsp_escape_text(q,e,sizeof(e));if(s!=UMI_STATUS_OK)return s;n=snprintf(p,sizeof(p),"{\"query\":\"%s\"}",e);if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_protocol_client_request(c->client,"workspace/symbol",p,id);}
