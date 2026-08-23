/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/did_close.h
 *
 * PURPOSE:
 *   Send textDocument/didClose document synchronization notification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_DID_CLOSE_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_DID_CLOSE_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_did_close(UmiLanguageRuntimeServer*s,const char*u);
#ifdef __cplusplus
}
#endif
#endif
