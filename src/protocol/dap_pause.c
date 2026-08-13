/* Umicom Framework DAP pause. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_pause(UmiDapClient*c,int t,int64_t*id){return umi_dap_thread_request(c,"pause",t,NULL,id);}
