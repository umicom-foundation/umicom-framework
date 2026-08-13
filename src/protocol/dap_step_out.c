/* Umicom Framework DAP stepOut. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_step_out(UmiDapClient*c,int t,int64_t*id){return umi_dap_thread_request(c,"stepOut",t,",\"singleThread\":false",id);}
