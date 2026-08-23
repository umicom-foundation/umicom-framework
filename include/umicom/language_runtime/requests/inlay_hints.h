/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/inlay_hints.h
 *
 * PURPOSE:
 *   Request textDocument/inlayHint for a visible range.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_INLAY_HINTS_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_INLAY_HINTS_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_inlay_hints(UmiLanguageRuntimeServer*s,const char*d,const char*u,const UmiLanguageRuntimeRange*r,uint64_t*out);
#ifdef __cplusplus
}
#endif
#endif
