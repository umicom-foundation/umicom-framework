/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/message.h
 *
 * PURPOSE:
 *   Build and classify JSON-RPC 2.0 requests, responses and notifications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_MESSAGE_H
#define UMICOM_LANGUAGE_RUNTIME_MESSAGE_H
#include "umicom/language_runtime/json.h"
#include "umicom/language_runtime/json_writer.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime build request operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_build_request(uint64_t id,const char*method,const char*params,char*out,size_t cap);
/**
 * Provide the language runtime build notification operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_build_notification(const char*method,const char*params,char*out,size_t cap);
/**
 * Read language runtime message into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_language_runtime_message_parse(const char*json,UmiLanguageRuntimeEnvelope*out);
#ifdef __cplusplus
}
#endif
#endif
