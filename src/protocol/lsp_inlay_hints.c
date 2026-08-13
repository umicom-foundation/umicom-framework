/* Umicom Framework LSP inlayHint. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_inlay_hints(UmiLspClient *c,const char *u,UmiLspRange r,int64_t *id){return umi_lsp_request_range(c,"textDocument/inlayHint",u,r,NULL,id);}
