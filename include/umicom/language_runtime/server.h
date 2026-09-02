/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/server.h
 *
 * PURPOSE:
 *   Own one real LSP JSON-RPC connection over a persistent transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_SERVER_H
#define UMICOM_LANGUAGE_RUNTIME_SERVER_H
#include "umicom/language/client_session.h"
#include "umicom/language/server_profile.h"
#include "umicom/language_runtime/arguments.h"
#include "umicom/language_runtime/framing.h"
#include "umicom/language_runtime/message.h"
#include "umicom/language_runtime/pending.h"
#include "umicom/language_runtime/transport.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language runtime server data shared with callers of this public contract.
 */
typedef struct UmiLanguageRuntimeServer UmiLanguageRuntimeServer;
/**
 * Represent the language runtime server snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageRuntimeServerSnapshot{char server_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];char profile_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];char root_uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];UmiLanguageRuntimeServerState state;uint64_t next_request_id;size_t pending_requests;uint64_t messages_sent,messages_received,bytes_sent,bytes_received,revision;}UmiLanguageRuntimeServerSnapshot;
/**
 * Provide the language runtime server start operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_server_start(const char*server_id,const UmiLanguageServerProfile*profile,const char*root_uri,const char*working_directory,UmiLanguageRuntimeServer**out);
/**
 * Provide the language runtime server create with transport operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_server_create_with_transport(const char*server_id,const UmiLanguageServerProfile*profile,const char*root_uri,UmiLanguageRuntimeTransport*transport,UmiLanguageRuntimeServer**out);
/**
 * Release or reset state held by language runtime server so the same storage can be reused
 * safely.
 */
void umi_language_runtime_server_destroy(UmiLanguageRuntimeServer*s);
/**
 * Provide the language runtime server transition operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_server_transition(UmiLanguageRuntimeServer*s,UmiLanguageRuntimeServerState state);
/**
 * Provide the language runtime server send request operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_server_send_request(UmiLanguageRuntimeServer*s,const char*method,const char*params,const char*document_id,uint64_t*out_id);
/**
 * Provide the language runtime server send notification operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_server_send_notification(UmiLanguageRuntimeServer*s,const char*method,const char*params);
/**
 * Provide the language runtime server receive operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_server_receive(UmiLanguageRuntimeServer*s,uint32_t timeout_ms,UmiLanguageRuntimeEnvelope*out);
/**
 * Provide the language runtime server stop operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_server_stop(UmiLanguageRuntimeServer*s,uint32_t timeout_ms);
/**
 * Provide the language runtime server is running operation used by this module and its
 * client applications.
 */
int umi_language_runtime_server_is_running(UmiLanguageRuntimeServer*s);
/**
 * Provide the language runtime server snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_server_snapshot(const UmiLanguageRuntimeServer*s,UmiLanguageRuntimeServerSnapshot*out);
#ifdef __cplusplus
}
#endif
#endif
