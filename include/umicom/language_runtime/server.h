/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/server.h
 *
 * PURPOSE:
 *   Own one real LSP JSON-RPC connection over a persistent transport.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiLanguageRuntimeServer UmiLanguageRuntimeServer;
typedef struct UmiLanguageRuntimeServerSnapshot{char server_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];char profile_id[UMI_LANGUAGE_RUNTIME_ID_CAPACITY];char root_uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];UmiLanguageRuntimeServerState state;uint64_t next_request_id;size_t pending_requests;uint64_t messages_sent,messages_received,bytes_sent,bytes_received,revision;}UmiLanguageRuntimeServerSnapshot;
UmiStatus umi_language_runtime_server_start(const char*server_id,const UmiLanguageServerProfile*profile,const char*root_uri,const char*working_directory,UmiLanguageRuntimeServer**out);
UmiStatus umi_language_runtime_server_create_with_transport(const char*server_id,const UmiLanguageServerProfile*profile,const char*root_uri,UmiLanguageRuntimeTransport*transport,UmiLanguageRuntimeServer**out);
void umi_language_runtime_server_destroy(UmiLanguageRuntimeServer*s);
UmiStatus umi_language_runtime_server_transition(UmiLanguageRuntimeServer*s,UmiLanguageRuntimeServerState state);
UmiStatus umi_language_runtime_server_send_request(UmiLanguageRuntimeServer*s,const char*method,const char*params,const char*document_id,uint64_t*out_id);
UmiStatus umi_language_runtime_server_send_notification(UmiLanguageRuntimeServer*s,const char*method,const char*params);
UmiStatus umi_language_runtime_server_receive(UmiLanguageRuntimeServer*s,uint32_t timeout_ms,UmiLanguageRuntimeEnvelope*out);
UmiStatus umi_language_runtime_server_stop(UmiLanguageRuntimeServer*s,uint32_t timeout_ms);
int umi_language_runtime_server_is_running(UmiLanguageRuntimeServer*s);
UmiStatus umi_language_runtime_server_snapshot(const UmiLanguageRuntimeServer*s,UmiLanguageRuntimeServerSnapshot*out);
#ifdef __cplusplus
}
#endif
#endif
