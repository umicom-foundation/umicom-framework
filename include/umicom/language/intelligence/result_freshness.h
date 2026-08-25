/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/result_freshness.h
 *
 * PURPOSE:
 *   Track semantic result age and source generation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_RESULT_FRESHNESS_H
#define UMICOM_LANGUAGE_INTELLIGENCE_RESULT_FRESHNESS_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_RESULT_FRESHNESS_API_VERSION 1U
typedef struct UmiLanguageIntelligenceResultFreshness {
    uint32_t struct_size;
    uint32_t api_version;
    char key[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint64_t fingerprint;
    uint64_t created_at;
    uint64_t expires_at;
    uint64_t source_generation;
    uint64_t revision;
    int valid;
} UmiLanguageIntelligenceResultFreshness;
void umi_language_intelligence_result_freshness_init(
    UmiLanguageIntelligenceResultFreshness *entry,
    const char *key,
    uint64_t fingerprint,
    uint64_t now,
    uint64_t ttl);
int umi_language_intelligence_result_freshness_is_fresh(
    const UmiLanguageIntelligenceResultFreshness *entry,
    uint64_t now,
    uint64_t expected_fingerprint);
void umi_language_intelligence_result_freshness_invalidate(UmiLanguageIntelligenceResultFreshness *entry);
#ifdef __cplusplus
}
#endif
#endif
