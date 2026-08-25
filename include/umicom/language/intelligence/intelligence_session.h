/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/intelligence_session.h
 *
 * PURPOSE:
 *   Aggregate Framework-owned language-intelligence session health and activity.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_INTELLIGENCE_SESSION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_INTELLIGENCE_SESSION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_INTELLIGENCE_SESSION_API_VERSION 1U
typedef struct UmiLanguageIntelligenceIntelligenceSession {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    size_t provider_count;
    uint64_t requests_started;
    uint64_t requests_completed;
    uint64_t requests_failed;
    uint64_t requests_cancelled;
    UmiLanguageIntelligenceState state;
    uint64_t revision;
} UmiLanguageIntelligenceIntelligenceSession;
void umi_language_intelligence_intelligence_session_init(UmiLanguageIntelligenceIntelligenceSession *session, const char *id);
void umi_language_intelligence_intelligence_session_begin_request(UmiLanguageIntelligenceIntelligenceSession *session);
void umi_language_intelligence_intelligence_session_complete_request(UmiLanguageIntelligenceIntelligenceSession *session, UmiStatus status);
size_t umi_language_intelligence_intelligence_session_active_requests(const UmiLanguageIntelligenceIntelligenceSession *session);
void umi_language_intelligence_intelligence_session_refresh_state(UmiLanguageIntelligenceIntelligenceSession *session);
#ifdef __cplusplus
}
#endif
#endif
