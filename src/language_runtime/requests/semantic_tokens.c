/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/semantic_tokens.c
 *
 * PURPOSE:
 *   Implement the LSP textDocument/semanticTokens/full request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/semantic_tokens.h"
/*
 * Provide the language runtime request semantic tokens operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_request_semantic_tokens(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint64_t*out){char p[2048];UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_params_document(u,p,sizeof(p));return q==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/semanticTokens/full",p,d,out):q;}
