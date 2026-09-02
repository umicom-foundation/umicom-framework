/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/request_support.h
 *
 * PURPOSE:
 *   Share bounded LSP parameter builders across method-specific requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_SUPPORT_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_SUPPORT_H
#include "umicom/language_runtime/server.h"
#include "umicom/language_runtime/json_writer.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime params position operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_params_position(const char*uri,uint32_t line,uint32_t col,char*out,size_t cap);
/**
 * Provide the language runtime params document operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_params_document(const char*uri,char*out,size_t cap);
/**
 * Provide the language runtime params range operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_params_range(const char*uri,const UmiLanguageRuntimeRange*r,char*out,size_t cap);
/**
 * Provide the language runtime params workspace query operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_params_workspace_query(const char*q,char*out,size_t cap);
#ifdef __cplusplus
}
#endif
#endif
