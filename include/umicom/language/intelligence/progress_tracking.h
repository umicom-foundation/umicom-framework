/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/progress_tracking.h
 *
 * PURPOSE:
 *   Track bounded work progress and terminal state.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_PROGRESS_TRACKING_H
#define UMICOM_LANGUAGE_INTELLIGENCE_PROGRESS_TRACKING_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_PROGRESS_TRACKING_API_VERSION 1U
typedef struct UmiLanguageIntelligenceProgressTracking {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint64_t completed;
    uint64_t total;
    UmiLanguageIntelligenceState state;
    uint64_t revision;
} UmiLanguageIntelligenceProgressTracking;
void umi_language_intelligence_progress_tracking_init(UmiLanguageIntelligenceProgressTracking *progress, const char *id, uint64_t total);
UmiStatus umi_language_intelligence_progress_tracking_advance(UmiLanguageIntelligenceProgressTracking *progress, uint64_t amount);
uint32_t umi_language_intelligence_progress_tracking_basis_points(const UmiLanguageIntelligenceProgressTracking *progress);
void umi_language_intelligence_progress_tracking_finish(UmiLanguageIntelligenceProgressTracking *progress);
#ifdef __cplusplus
}
#endif
#endif
