/* Umicom Framework DAP loadedSources. Sammy Hegab, Umicom Foundation, MIT. */
#include "dap_request_internal.h"
UmiStatus umi_dap_loaded_sources(UmiDapClient*c,int64_t*id){return umi_dap_request(c,"loadedSources","{}",id);}
