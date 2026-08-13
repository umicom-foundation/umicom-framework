/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/client_session.h
 * PURPOSE: Track one provider-neutral language-server lifecycle.
 * Created by: Sammy Hegab | Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LANGUAGE_CLIENT_SESSION_H
#define UMICOM_LANGUAGE_CLIENT_SESSION_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiLanguageClientState {
    UMI_LANGUAGE_CLIENT_STOPPED = 0,
    UMI_LANGUAGE_CLIENT_STARTING = 1,
    UMI_LANGUAGE_CLIENT_INITIALIZING = 2,
    UMI_LANGUAGE_CLIENT_READY = 3,
    UMI_LANGUAGE_CLIENT_STOPPING = 4,
    UMI_LANGUAGE_CLIENT_FAILED = 5
} UmiLanguageClientState;

typedef struct UmiLanguageClientSession {
    char session_id[128];
    char profile_id[128];
    char root_uri[1024];
    UmiLanguageClientState state;
    uint64_t requests_sent;
    uint64_t responses_received;
    uint64_t notifications_received;
    uint64_t failures;
    uint64_t revision;
} UmiLanguageClientSession;

UmiStatus umi_language_client_session_init(UmiLanguageClientSession *session,
                                           const char *session_id,
                                           const char *profile_id,
                                           const char *root_uri);
UmiStatus umi_language_client_session_transition(
    UmiLanguageClientSession *session, UmiLanguageClientState next_state);
void umi_language_client_session_record_request(UmiLanguageClientSession *session);
void umi_language_client_session_record_response(UmiLanguageClientSession *session);
void umi_language_client_session_record_notification(UmiLanguageClientSession *session);
const char *umi_language_client_state_text(UmiLanguageClientState state);

#ifdef __cplusplus
}
#endif
#endif
