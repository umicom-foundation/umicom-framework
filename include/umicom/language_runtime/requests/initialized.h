/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/initialized.h
 *
 * PURPOSE:
 *   Send LSP initialized lifecycle message.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_INITIALIZED_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_INITIALIZED_H
#include "umicom/language_runtime/server.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_initialized(UmiLanguageRuntimeServer*s);
#ifdef __cplusplus
}
#endif
#endif
