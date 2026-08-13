/* Umicom Framework DAP configurationDone. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_configuration_done(UmiDapClient *c,int64_t *id){return umi_dap_request(c,"configurationDone","{}",id);}
