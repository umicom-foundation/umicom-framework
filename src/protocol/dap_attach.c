/* Umicom Framework DAP attach. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
UmiStatus umi_dap_attach(UmiDapClient *c,int pid,const char *cwd,int64_t *id){char e[UMI_PROTOCOL_URI_CAPACITY*2U],p[UMI_PROTOCOL_MESSAGE_CAPACITY];int n;UmiStatus s;if(pid<0||cwd==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_dap_escape(cwd,e,sizeof(e));if(s!=UMI_STATUS_OK)return s;n=snprintf(p,sizeof(p),"{\"processId\":%d,\"cwd\":\"%s\"}",pid,e);if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"attach",p,id);}
