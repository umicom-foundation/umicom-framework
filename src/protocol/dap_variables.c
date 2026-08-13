/* Umicom Framework DAP variables. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
UmiStatus umi_dap_variables(UmiDapClient*c,int r,int start,int count,int64_t*id){char p[256];int n;if(r<0||start<0||count<0)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(p,sizeof(p),"{\"variablesReference\":%d,\"start\":%d,\"count\":%d}",r,start,count);if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"variables",p,id);}
