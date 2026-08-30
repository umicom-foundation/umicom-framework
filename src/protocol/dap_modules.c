/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_modules.c
 *
 * PURPOSE:
 *   Implement the dap modules behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP modules. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
UmiStatus umi_dap_modules(UmiDapClient*c,int start,int count,int64_t*id){char p[128];int n;if(start<0||count<0)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(p,sizeof(p),"{\"startModule\":%d,\"moduleCount\":%d}",start,count);if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"modules",p,id);}
