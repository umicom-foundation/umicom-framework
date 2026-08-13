/* Umicom Framework DAP next. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_next(UmiDapClient*c,int t,int64_t*id){return umi_dap_thread_request(c,"next",t,",\"singleThread\":false",id);}
