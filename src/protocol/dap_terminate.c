/* Umicom Framework DAP terminate. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_terminate(UmiDapClient*c,int restart,int64_t*id){return umi_dap_request(c,"terminate",restart?"{\"restart\":true}":"{\"restart\":false}",id);}
