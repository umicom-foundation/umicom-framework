/* Umicom Framework DAP setVariable. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
#include <stdio.h>
UmiStatus umi_dap_set_variable(UmiDapClient*c,int r,const char*name,const char*value,int64_t*id){char n1[1024],v1[4096],p[UMI_PROTOCOL_MESSAGE_CAPACITY];int n;UmiStatus s;if(r<0||name==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_dap_escape(name,n1,sizeof(n1));if(s==UMI_STATUS_OK)s=umi_dap_escape(value,v1,sizeof(v1));if(s!=UMI_STATUS_OK)return s;n=snprintf(p,sizeof(p),"{\"variablesReference\":%d,\"name\":\"%s\",\"value\":\"%s\"}",r,n1,v1);if(n<0||(size_t)n>=sizeof(p))return UMI_STATUS_CAPACITY_EXCEEDED;return umi_dap_request(c,"setVariable",p,id);}
