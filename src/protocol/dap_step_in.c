/* Umicom Framework DAP stepIn. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_step_in(UmiDapClient*c,int t,int64_t*id){return umi_dap_thread_request(c,"stepIn",t,",\"singleThread\":false",id);}
