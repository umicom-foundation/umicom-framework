/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_disassemble.c
 *
 * PURPOSE:
 *   Implement the dap disassemble behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP disassemble. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
/* Provide the dap disassemble operation used by this module and its client applications. */
UmiStatus umi_dap_disassemble(UmiDapClient*c,const char*ref,int offset,int count,int64_t*id){char e[1024],p[2048];int n;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(ref==NULL||count<0)return UMI_STATUS_INVALID_ARGUMENT;s=umi_dap_escape(ref,e,sizeof(e));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;n=snprintf(p,sizeof(p),"{\"memoryReference\":\"%s\",\"instructionOffset\":%d,\"instructionCount\":%d,\"resolveSymbols\":true}",e,offset,count);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"disassemble",p,id);}
