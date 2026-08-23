/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/code_action.h
 *
 * PURPOSE:
 *   Request quick fixes/refactor code actions for a document range.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_CODE_ACTION_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_CODE_ACTION_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_code_action(UmiLanguageRuntimeServer*s,const char*d,const char*u,const UmiLanguageRuntimeRange*r,uint64_t*out);
#ifdef __cplusplus
}
#endif
#endif
