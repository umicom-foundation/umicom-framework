/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/workspace_symbols.h
 *
 * PURPOSE:
 *   Send workspace/symbol for Go to Symbol / Search Everywhere.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_WORKSPACE_SYMBOLS_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_WORKSPACE_SYMBOLS_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_workspace_symbols(UmiLanguageRuntimeServer*s,const char*query,uint64_t*out_id);
#ifdef __cplusplus
}
#endif
#endif
