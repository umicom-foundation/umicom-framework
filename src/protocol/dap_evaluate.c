/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_evaluate.c
 *
 * PURPOSE:
 *   Implement the dap evaluate behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP evaluate. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
UmiStatus umi_dap_evaluate(UmiDapClient*c,const char*expression,int frame,const char*context,int64_t*id){char e[4096],x[256],p[UMI_PROTOCOL_MESSAGE_CAPACITY];int n;UmiStatus s;if(expression==NULL||context==NULL||frame<0)return UMI_STATUS_INVALID_ARGUMENT;s=umi_dap_escape(expression,e,sizeof(e));if(s==UMI_STATUS_OK)s=umi_dap_escape(context,x,sizeof(x));if(s!=UMI_STATUS_OK)return s;n=snprintf(p,sizeof(p),"{\"expression\":\"%s\",\"frameId\":%d,\"context\":\"%s\"}",e,frame,x);if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"evaluate",p,id);}
