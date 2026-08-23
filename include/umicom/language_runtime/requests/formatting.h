/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/formatting.h
 *
 * PURPOSE:
 *   Request whole-document formatting with explicit tab/space policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_FORMATTING_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_FORMATTING_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_formatting(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t tab_size,int spaces,uint64_t*out);
#ifdef __cplusplus
}
#endif
#endif
