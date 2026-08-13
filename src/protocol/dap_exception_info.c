/* Umicom Framework DAP exceptionInfo. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_exception_info(UmiDapClient*c,int t,int64_t*id){return umi_dap_thread_request(c,"exceptionInfo",t,NULL,id);}
