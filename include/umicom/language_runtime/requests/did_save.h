/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/did_save.h
 *
 * PURPOSE:
 *   Send textDocument/didSave document synchronization notification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_DID_SAVE_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_DID_SAVE_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Write language runtime request did in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_language_runtime_request_did_save(UmiLanguageRuntimeServer*s,const char*u,const char*text);
#ifdef __cplusplus
}
#endif
#endif
