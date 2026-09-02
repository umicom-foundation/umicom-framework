/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/client_session.c
 *
 * PURPOSE:
 *   Implement the client session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework language client lifecycle. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/language/client_session.h"
#include <stdio.h>
#include <string.h>

/*
 * Provide the transition allowed operation used by this module and its client
 * applications.
 */
static int transition_allowed(UmiLanguageClientState current,
                              UmiLanguageClientState next)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (next == UMI_LANGUAGE_CLIENT_FAILED) return 1;
    /* Select the behaviour associated with the requested command or state value. */
    switch (current) {
        case UMI_LANGUAGE_CLIENT_STOPPED: return next == UMI_LANGUAGE_CLIENT_STARTING;
        case UMI_LANGUAGE_CLIENT_STARTING: return next == UMI_LANGUAGE_CLIENT_INITIALIZING;
        case UMI_LANGUAGE_CLIENT_INITIALIZING: return next == UMI_LANGUAGE_CLIENT_READY;
        case UMI_LANGUAGE_CLIENT_READY: return next == UMI_LANGUAGE_CLIENT_STOPPING;
        case UMI_LANGUAGE_CLIENT_STOPPING: return next == UMI_LANGUAGE_CLIENT_STOPPED;
        case UMI_LANGUAGE_CLIENT_FAILED: return next == UMI_LANGUAGE_CLIENT_STOPPED || next == UMI_LANGUAGE_CLIENT_STARTING;
        default: return 0;
    }
}

/*
 * Initialise language client session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_client_session_init(UmiLanguageClientSession *session,
                                           const char *session_id,
                                           const char *profile_id,
                                           const char *root_uri)
{
    int a, b, c;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || session_id == NULL || profile_id == NULL ||
        root_uri == NULL || session_id[0] == '\0' || profile_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(session, 0, sizeof(*session));
    a = snprintf(session->session_id, sizeof(session->session_id), "%s", session_id);
    b = snprintf(session->profile_id, sizeof(session->profile_id), "%s", profile_id);
    c = snprintf(session->root_uri, sizeof(session->root_uri), "%s", root_uri);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (a < 0 || b < 0 || c < 0 || (size_t)a >= sizeof(session->session_id) ||
        (size_t)b >= sizeof(session->profile_id) ||
        (size_t)c >= sizeof(session->root_uri)) return UMI_STATUS_CAPACITY_EXCEEDED;
    session->state = UMI_LANGUAGE_CLIENT_STOPPED;
    session->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the language client session transition operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_client_session_transition(
    UmiLanguageClientSession *session, UmiLanguageClientState next_state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->state == next_state) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!transition_allowed(session->state, next_state)) return UMI_STATUS_INVALID_STATE;
    session->state = next_state;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (next_state == UMI_LANGUAGE_CLIENT_FAILED) session->failures += 1U;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the language client session record request operation used by this module and its
 * client applications.
 */
void umi_language_client_session_record_request(UmiLanguageClientSession *session){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session!=NULL){session->requests_sent+=1U;session->revision+=1U;}}
/*
 * Provide the language client session record response operation used by this module and
 * its client applications.
 */
void umi_language_client_session_record_response(UmiLanguageClientSession *session){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session!=NULL){session->responses_received+=1U;session->revision+=1U;}}
/*
 * Provide the language client session record notification operation used by this module
 * and its client applications.
 */
void umi_language_client_session_record_notification(UmiLanguageClientSession *session){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(session!=NULL){session->notifications_received+=1U;session->revision+=1U;}}

/*
 * Provide the language client state text operation used by this module and its client
 * applications.
 */
const char *umi_language_client_state_text(UmiLanguageClientState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_LANGUAGE_CLIENT_STOPPED: return "stopped";
        case UMI_LANGUAGE_CLIENT_STARTING: return "starting";
        case UMI_LANGUAGE_CLIENT_INITIALIZING: return "initializing";
        case UMI_LANGUAGE_CLIENT_READY: return "ready";
        case UMI_LANGUAGE_CLIENT_STOPPING: return "stopping";
        case UMI_LANGUAGE_CLIENT_FAILED: return "failed";
        default: return "unknown";
    }
}
