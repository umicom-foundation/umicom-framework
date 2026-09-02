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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_INTELLIGENCE_SESSION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_INTELLIGENCE_SESSION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_INTELLIGENCE_SESSION_API_VERSION 1U
/**
 * Represent the language intelligence intelligence session data shared with callers of
 * this public contract.
 */
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
/**
 * Initialise language intelligence intelligence session from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_intelligence_session_init(UmiLanguageIntelligenceIntelligenceSession *session, const char *id);
/**
 * Provide the language intelligence intelligence session begin request operation used by
 * this module and its client applications.
 */
void umi_language_intelligence_intelligence_session_begin_request(UmiLanguageIntelligenceIntelligenceSession *session);
/**
 * Provide the language intelligence intelligence session complete request operation used
 * by this module and its client applications.
 */
void umi_language_intelligence_intelligence_session_complete_request(UmiLanguageIntelligenceIntelligenceSession *session, UmiStatus status);
/**
 * Provide the language intelligence intelligence session active requests operation used by
 * this module and its client applications.
 */
size_t umi_language_intelligence_intelligence_session_active_requests(const UmiLanguageIntelligenceIntelligenceSession *session);
/**
 * Provide the language intelligence intelligence session refresh state operation used by
 * this module and its client applications.
 */
void umi_language_intelligence_intelligence_session_refresh_state(UmiLanguageIntelligenceIntelligenceSession *session);
#ifdef __cplusplus
}
#endif
#endif
