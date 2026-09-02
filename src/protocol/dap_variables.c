/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_variables.c
 *
 * PURPOSE:
 *   Implement the dap variables behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP variables. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
/* Provide the dap variables operation used by this module and its client applications. */
UmiStatus umi_dap_variables(UmiDapClient*c,int r,int start,int count,int64_t*id){char p[256];int n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r<0||start<0||count<0)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(p,sizeof(p),"{\"variablesReference\":%d,\"start\":%d,\"count\":%d}",r,start,count);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"variables",p,id);}
