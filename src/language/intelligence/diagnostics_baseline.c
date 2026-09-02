/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/diagnostics_baseline.c
 *
 * PURPOSE:
 *   Implement track diagnostic baselines for compare-to-known-state workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/diagnostics_baseline.h"
#include <string.h>
/*
 * Initialise language intelligence diagnostics baseline from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_diagnostics_baseline_init(
    UmiLanguageIntelligenceDiagnosticsBaseline *entry,
    const char *key,
    uint64_t fingerprint,
    uint64_t now,
    uint64_t ttl)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return;
    (void)memset(entry, 0, sizeof(*entry));
    entry->struct_size = (uint32_t)sizeof(*entry);
    entry->api_version = UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_BASELINE_API_VERSION;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (key != NULL) (void)umi_language_intelligence_copy_text(
        entry->key, sizeof(entry->key), key);
    entry->fingerprint = fingerprint;
    entry->created_at = now;
    entry->expires_at = UINT64_MAX - now < ttl ? UINT64_MAX : now + ttl;
    entry->source_generation = 1U;
    entry->revision = 1U;
    entry->valid = key != NULL && key[0] != '\0';
}
/*
 * Provide the language intelligence diagnostics baseline is fresh operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_diagnostics_baseline_is_fresh(
    const UmiLanguageIntelligenceDiagnosticsBaseline *entry,
    uint64_t now,
    uint64_t expected_fingerprint)
{
    return entry != NULL && entry->valid != 0 &&
        entry->key[0] != '\0' &&
        entry->fingerprint == expected_fingerprint &&
        now >= entry->created_at && now <= entry->expires_at;
}
/*
 * Provide the language intelligence diagnostics baseline invalidate operation used by this
 * module and its client applications.
 */
void umi_language_intelligence_diagnostics_baseline_invalidate(UmiLanguageIntelligenceDiagnosticsBaseline *entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return;
    entry->valid = 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (entry->revision != UINT64_MAX) entry->revision += 1U;
}
