/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_read_memory.c
 *
 * PURPOSE:
 *   Implement the dap read memory behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP readMemory. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
UmiStatus umi_dap_read_memory(UmiDapClient*c,const char*ref,int offset,int count,int64_t*id){char e[1024],p[2048];int n;UmiStatus s;if(ref==NULL||count<0)return UMI_STATUS_INVALID_ARGUMENT;s=umi_dap_escape(ref,e,sizeof(e));if(s!=UMI_STATUS_OK)return s;n=snprintf(p,sizeof(p),"{\"memoryReference\":\"%s\",\"offset\":%d,\"count\":%d}",e,offset,count);if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"readMemory",p,id);}
