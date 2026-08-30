/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/dap_scopes.c
 *
 * PURPOSE:
 *   Implement the dap scopes behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP scopes. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
UmiStatus umi_dap_scopes(UmiDapClient*c,int f,int64_t*id){char p[128];int n;if(f<0)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(p,sizeof(p),"{\"frameId\":%d}",f);if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"scopes",p,id);}
