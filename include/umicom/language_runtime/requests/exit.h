/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/exit.h
 *
 * PURPOSE:
 *   Send LSP exit lifecycle message.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_EXIT_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_EXIT_H
#include "umicom/language_runtime/server.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime request exit operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_request_exit(UmiLanguageRuntimeServer*s);
#ifdef __cplusplus
}
#endif
#endif
