/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/semantic_tokens.h
 *
 * PURPOSE:
 *   Send the LSP textDocument/semanticTokens/full request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_SEMANTIC_TOKENS_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_SEMANTIC_TOKENS_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_semantic_tokens(UmiLanguageRuntimeServer*s,const char*document_id,const char*uri,uint64_t*out_id);
#ifdef __cplusplus
}
#endif
#endif
