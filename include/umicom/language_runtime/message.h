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
UmiStatus umi_language_runtime_build_request(uint64_t id,const char*method,const char*params,char*out,size_t cap);
UmiStatus umi_language_runtime_build_notification(const char*method,const char*params,char*out,size_t cap);
UmiStatus umi_language_runtime_message_parse(const char*json,UmiLanguageRuntimeEnvelope*out);
#ifdef __cplusplus
}
#endif
#endif
