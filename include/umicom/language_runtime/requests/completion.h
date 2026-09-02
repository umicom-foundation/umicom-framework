/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/completion.h
 *
 * PURPOSE:
 *   Send the LSP textDocument/completion request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_COMPLETION_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_COMPLETION_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime request completion operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_completion(UmiLanguageRuntimeServer*s,const char*document_id,const char*uri,uint32_t line,uint32_t column,uint64_t*out_id);
#ifdef __cplusplus
}
#endif
#endif
