/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/did_open.h
 *
 * PURPOSE:
 *   Send textDocument/didOpen document synchronization notification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_DID_OPEN_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_DID_OPEN_H
#include "umicom/language_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_did_open(UmiLanguageRuntimeServer*s,const char*u,const char*lang,uint64_t ver,const char*text);
#ifdef __cplusplus
}
#endif
#endif
