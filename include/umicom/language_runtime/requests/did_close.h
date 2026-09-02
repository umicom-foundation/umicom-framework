/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/did_close.h
 *
 * PURPOSE:
 *   Send textDocument/didClose document synchronization notification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_DID_CLOSE_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_DID_CLOSE_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime request did close operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_did_close(UmiLanguageRuntimeServer*s,const char*u);
#ifdef __cplusplus
}
#endif
#endif
