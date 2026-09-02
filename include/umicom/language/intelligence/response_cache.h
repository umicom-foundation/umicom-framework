/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/response_cache.h
 *
 * PURPOSE:
 *   Cache provider results by stable key/fingerprint with explicit freshness.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_RESPONSE_CACHE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_RESPONSE_CACHE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_RESPONSE_CACHE_API_VERSION 1U
/**
 * Represent the language intelligence response cache data shared with callers of this
 * public contract.
 */
typedef struct UmiLanguageIntelligenceResponseCache {
    uint32_t struct_size;
    uint32_t api_version;
    char key[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint64_t fingerprint;
    uint64_t created_at;
    uint64_t expires_at;
    uint64_t source_generation;
    uint64_t revision;
    int valid;
} UmiLanguageIntelligenceResponseCache;
/**
 * Initialise language intelligence response cache from caller-provided values so later
 * operations receive a known state.
 */
void umi_language_intelligence_response_cache_init(
    UmiLanguageIntelligenceResponseCache *entry,
    const char *key,
    uint64_t fingerprint,
    uint64_t now,
    uint64_t ttl);
/**
 * Provide the language intelligence response cache is fresh operation used by this module
 * and its client applications.
 */
int umi_language_intelligence_response_cache_is_fresh(
    const UmiLanguageIntelligenceResponseCache *entry,
    uint64_t now,
    uint64_t expected_fingerprint);
/**
 * Provide the language intelligence response cache invalidate operation used by this
 * module and its client applications.
 */
void umi_language_intelligence_response_cache_invalidate(UmiLanguageIntelligenceResponseCache *entry);
#ifdef __cplusplus
}
#endif
#endif
