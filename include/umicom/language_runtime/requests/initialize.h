/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/requests/initialize.h
 *
 * PURPOSE:
 *   Send LSP initialize with practical Umicom IDE client capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_REQUEST_INITIALIZE_H
#define UMICOM_LANGUAGE_RUNTIME_REQUEST_INITIALIZE_H
#include "umicom/language_runtime/server.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_request_initialize(UmiLanguageRuntimeServer*s,const char*root_uri,uint64_t*out_id);
#ifdef __cplusplus
}
#endif
#endif
