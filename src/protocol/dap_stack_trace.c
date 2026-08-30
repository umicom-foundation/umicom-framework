/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_stack_trace.c
 *
 * PURPOSE:
 *   Implement the dap stack trace behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP stackTrace. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
UmiStatus umi_dap_stack_trace(UmiDapClient*c,int t,int start,int levels,int64_t*id){char p[256];int n;if(t<0||start<0||levels<0)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(p,sizeof(p),"{\"threadId\":%d,\"startFrame\":%d,\"levels\":%d}",t,start,levels);if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"stackTrace",p,id);}
