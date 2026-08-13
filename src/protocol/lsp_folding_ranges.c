/* Umicom Framework LSP foldingRange. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_folding_ranges(UmiLspClient *c,const char *u,int64_t *id){return umi_lsp_request_document(c,"textDocument/foldingRange",u,id);}
