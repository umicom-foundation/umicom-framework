/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_execute_command.c
 *
 * PURPOSE:
 *   Implement the lsp execute command behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP workspace/executeCommand. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
#include <stdio.h>
/*
 * Provide the lsp execute command operation used by this module and its client
 * applications.
 */
UmiStatus umi_lsp_execute_command(UmiLspClient *c,const char *cmd,const char *args,int64_t *id){char e[1024],p[UMI_PROTOCOL_MESSAGE_CAPACITY];const char *a=args!=NULL?args:"[]";int n;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||c->client==NULL||cmd==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_lsp_escape_text(cmd,e,sizeof(e));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;n=snprintf(p,sizeof(p),"{\"command\":\"%s\",\"arguments\":%s}",e,a);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_protocol_client_request(c->client,"workspace/executeCommand",p,id);}
