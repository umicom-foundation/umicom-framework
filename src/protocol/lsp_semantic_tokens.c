/* Umicom Framework LSP semanticTokens/full. Sammy Hegab, Umicom Foundation, MIT. */
#include "lsp_request_internal.h"
UmiStatus umi_lsp_semantic_tokens_full(UmiLspClient *c,const char *u,int64_t *id){return umi_lsp_request_document(c,"textDocument/semanticTokens/full",u,id);}
