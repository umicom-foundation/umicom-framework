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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_RESULT_FRESHNESS_H
#define UMICOM_LANGUAGE_INTELLIGENCE_RESULT_FRESHNESS_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_RESULT_FRESHNESS_API_VERSION 1U
/**
 * Represent the language intelligence result freshness data shared with callers of this
 * public contract.
 */
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
/**
 * Initialise language intelligence result freshness from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_result_freshness_init(
    UmiLanguageIntelligenceResultFreshness *entry,
    const char *key,
    uint64_t fingerprint,
    uint64_t now,
    uint64_t ttl);
/**
 * Provide the language intelligence result freshness is fresh operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_result_freshness_is_fresh(
    const UmiLanguageIntelligenceResultFreshness *entry,
    uint64_t now,
    uint64_t expected_fingerprint);
/**
 * Provide the language intelligence result freshness invalidate operation used by this
 * module and its client applications.
 */
void umi_language_intelligence_result_freshness_invalidate(UmiLanguageIntelligenceResultFreshness *entry);
#ifdef __cplusplus
}
#endif
#endif
