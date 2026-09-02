/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/intelligence_session.c
 *
 * PURPOSE:
 *   Implement aggregate Framework-owned language-intelligence session health and activity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/intelligence_session.h"
#include <string.h>
/*
 * Initialise language intelligence intelligence session from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_intelligence_session_init(UmiLanguageIntelligenceIntelligenceSession *session, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    (void)memset(session, 0, sizeof(*session));
    session->struct_size = (uint32_t)sizeof(*session);
    session->api_version = UMI_LANGUAGE_INTELLIGENCE_INTELLIGENCE_SESSION_API_VERSION;
    session->state = UMI_LANGUAGE_INTELLIGENCE_STARTING;
    session->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_language_intelligence_copy_text(session->id, sizeof(session->id), id);
}
/*
 * Provide the language intelligence intelligence session begin request operation used by
 * this module and its client applications.
 */
void umi_language_intelligence_intelligence_session_begin_request(UmiLanguageIntelligenceIntelligenceSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->requests_started != UINT64_MAX) session->requests_started += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->revision != UINT64_MAX) session->revision += 1U;
    umi_language_intelligence_intelligence_session_refresh_state(session);
}
/*
 * Provide the language intelligence intelligence session complete request operation used
 * by this module and its client applications.
 */
void umi_language_intelligence_intelligence_session_complete_request(UmiLanguageIntelligenceIntelligenceSession *session, UmiStatus status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->requests_completed != UINT64_MAX) session->requests_completed += 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CANCELLED) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (session->requests_cancelled != UINT64_MAX) session->requests_cancelled += 1U;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (session->requests_failed != UINT64_MAX) session->requests_failed += 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->revision != UINT64_MAX) session->revision += 1U;
    umi_language_intelligence_intelligence_session_refresh_state(session);
}
/*
 * Provide the language intelligence intelligence session active requests operation used by
 * this module and its client applications.
 */
size_t umi_language_intelligence_intelligence_session_active_requests(const UmiLanguageIntelligenceIntelligenceSession *session)
{
    uint64_t active;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || session->requests_completed >= session->requests_started) return 0U;
    active = session->requests_started - session->requests_completed;
    return active > (uint64_t)SIZE_MAX ? SIZE_MAX : (size_t)active;
}
/*
 * Provide the language intelligence intelligence session refresh state operation used by
 * this module and its client applications.
 */
void umi_language_intelligence_intelligence_session_refresh_state(UmiLanguageIntelligenceIntelligenceSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->requests_failed >= 3U) session->state = UMI_LANGUAGE_INTELLIGENCE_DEGRADED;
    else /* Apply this branch only when its contract condition is satisfied. */ if (session->provider_count == 0U) session->state = UMI_LANGUAGE_INTELLIGENCE_STARTING;
    /* Use this fallback path when the earlier condition does not apply. */
    else session->state = UMI_LANGUAGE_INTELLIGENCE_READY;
}
