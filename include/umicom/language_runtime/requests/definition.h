/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/definition.h
 *
 * PURPOSE:
 *   Send the LSP textDocument/definition request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_DEFINITION_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_DEFINITION_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime request definition operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_definition(UmiLanguageRuntimeServer*s,const char*document_id,const char*uri,uint32_t line,uint32_t column,uint64_t*out_id);
#ifdef __cplusplus
}
#endif
#endif
