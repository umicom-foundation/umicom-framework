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
void umi_language_intelligence_intelligence_session_init(UmiLanguageIntelligenceIntelligenceSession *session, const char *id)
{
    if (session == NULL) return;
    (void)memset(session, 0, sizeof(*session));
    session->struct_size = (uint32_t)sizeof(*session);
    session->api_version = UMI_LANGUAGE_INTELLIGENCE_INTELLIGENCE_SESSION_API_VERSION;
    session->state = UMI_LANGUAGE_INTELLIGENCE_STARTING;
    session->revision = 1U;
    if (id != NULL) (void)umi_language_intelligence_copy_text(session->id, sizeof(session->id), id);
}
void umi_language_intelligence_intelligence_session_begin_request(UmiLanguageIntelligenceIntelligenceSession *session)
{
    if (session == NULL) return;
    if (session->requests_started != UINT64_MAX) session->requests_started += 1U;
    if (session->revision != UINT64_MAX) session->revision += 1U;
    umi_language_intelligence_intelligence_session_refresh_state(session);
}
void umi_language_intelligence_intelligence_session_complete_request(UmiLanguageIntelligenceIntelligenceSession *session, UmiStatus status)
{
    if (session == NULL) return;
    if (session->requests_completed != UINT64_MAX) session->requests_completed += 1U;
    if (status == UMI_STATUS_CANCELLED) {
        if (session->requests_cancelled != UINT64_MAX) session->requests_cancelled += 1U;
    } else if (status != UMI_STATUS_OK) {
        if (session->requests_failed != UINT64_MAX) session->requests_failed += 1U;
    }
    if (session->revision != UINT64_MAX) session->revision += 1U;
    umi_language_intelligence_intelligence_session_refresh_state(session);
}
size_t umi_language_intelligence_intelligence_session_active_requests(const UmiLanguageIntelligenceIntelligenceSession *session)
{
    uint64_t active;
    if (session == NULL || session->requests_completed >= session->requests_started) return 0U;
    active = session->requests_started - session->requests_completed;
    return active > (uint64_t)SIZE_MAX ? SIZE_MAX : (size_t)active;
}
void umi_language_intelligence_intelligence_session_refresh_state(UmiLanguageIntelligenceIntelligenceSession *session)
{
    if (session == NULL) return;
    if (session->requests_failed >= 3U) session->state = UMI_LANGUAGE_INTELLIGENCE_DEGRADED;
    else if (session->provider_count == 0U) session->state = UMI_LANGUAGE_INTELLIGENCE_STARTING;
    else session->state = UMI_LANGUAGE_INTELLIGENCE_READY;
}
