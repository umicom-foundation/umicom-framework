/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/client_session.h
 *
 * PURPOSE:
 *   Publish the public client session contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LANGUAGE_CLIENT_SESSION_H
#define UMICOM_LANGUAGE_CLIENT_SESSION_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named language client state values accepted by this public contract.
 */
typedef enum UmiLanguageClientState {
    UMI_LANGUAGE_CLIENT_STOPPED = 0,
    UMI_LANGUAGE_CLIENT_STARTING = 1,
    UMI_LANGUAGE_CLIENT_INITIALIZING = 2,
    UMI_LANGUAGE_CLIENT_READY = 3,
    UMI_LANGUAGE_CLIENT_STOPPING = 4,
    UMI_LANGUAGE_CLIENT_FAILED = 5
} UmiLanguageClientState;

/**
 * Represent the language client session data shared with callers of this public contract.
 */
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

/**
 * Initialise language client session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_client_session_init(UmiLanguageClientSession *session,
                                           const char *session_id,
                                           const char *profile_id,
                                           const char *root_uri);
/**
 * Provide the language client session transition operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_client_session_transition(
    UmiLanguageClientSession *session, UmiLanguageClientState next_state);
/**
 * Provide the language client session record request operation used by this module and its
 * client applications.
 */
void umi_language_client_session_record_request(UmiLanguageClientSession *session);
/**
 * Provide the language client session record response operation used by this module and
 * its client applications.
 */
void umi_language_client_session_record_response(UmiLanguageClientSession *session);
/**
 * Provide the language client session record notification operation used by this module
 * and its client applications.
 */
void umi_language_client_session_record_notification(UmiLanguageClientSession *session);
/**
 * Provide the language client state text operation used by this module and its client
 * applications.
 */
const char *umi_language_client_state_text(UmiLanguageClientState state);

#ifdef __cplusplus
}
#endif
#endif
