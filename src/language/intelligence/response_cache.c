/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/response_cache.c
 *
 * PURPOSE:
 *   Implement cache provider results by stable key/fingerprint with explicit freshness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/response_cache.h"
#include <string.h>
void umi_language_intelligence_response_cache_init(
    UmiLanguageIntelligenceResponseCache *entry,
    const char *key,
    uint64_t fingerprint,
    uint64_t now,
    uint64_t ttl)
{
    if (entry == NULL) return;
    (void)memset(entry, 0, sizeof(*entry));
    entry->struct_size = (uint32_t)sizeof(*entry);
    entry->api_version = UMI_LANGUAGE_INTELLIGENCE_RESPONSE_CACHE_API_VERSION;
    if (key != NULL) (void)umi_language_intelligence_copy_text(
        entry->key, sizeof(entry->key), key);
    entry->fingerprint = fingerprint;
    entry->created_at = now;
    entry->expires_at = UINT64_MAX - now < ttl ? UINT64_MAX : now + ttl;
    entry->source_generation = 1U;
    entry->revision = 1U;
    entry->valid = key != NULL && key[0] != '\0';
}
int umi_language_intelligence_response_cache_is_fresh(
    const UmiLanguageIntelligenceResponseCache *entry,
    uint64_t now,
    uint64_t expected_fingerprint)
{
    return entry != NULL && entry->valid != 0 &&
        entry->key[0] != '\0' &&
        entry->fingerprint == expected_fingerprint &&
        now >= entry->created_at && now <= entry->expires_at;
}
void umi_language_intelligence_response_cache_invalidate(UmiLanguageIntelligenceResponseCache *entry)
{
    if (entry == NULL) return;
    entry->valid = 0;
    if (entry->revision != UINT64_MAX) entry->revision += 1U;
}
