/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/shutdown.h
 *
 * PURPOSE:
 *   Send LSP shutdown lifecycle message.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_SHUTDOWN_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_SHUTDOWN_H
#include "umicom/language_runtime/server.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_shutdown(UmiLanguageRuntimeServer*s,uint64_t*out_id);
#ifdef __cplusplus
}
#endif
#endif
