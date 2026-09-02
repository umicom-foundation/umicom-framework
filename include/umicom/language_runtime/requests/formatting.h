/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/formatting.h
 *
 * PURPOSE:
 *   Request whole-document formatting with explicit tab/space policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_FORMATTING_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_FORMATTING_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime request formatting operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_formatting(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t tab_size,int spaces,uint64_t*out);
#ifdef __cplusplus
}
#endif
#endif
