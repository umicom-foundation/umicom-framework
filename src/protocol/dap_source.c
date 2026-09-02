/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_source.c
 *
 * PURPOSE:
 *   Implement the dap source behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP source. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
/* Provide the dap source operation used by this module and its client applications. */
UmiStatus umi_dap_source(UmiDapClient*c,int r,int64_t*id){char p[128];int n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r<0)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(p,sizeof(p),"{\"sourceReference\":%d}",r);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"source",p,id);}
