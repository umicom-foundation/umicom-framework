/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/diagnostics_baseline.h
 *
 * PURPOSE:
 *   Track diagnostic baselines for compare-to-known-state workflows.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_BASELINE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_BASELINE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_BASELINE_API_VERSION 1U
/**
 * Represent the language intelligence diagnostics baseline data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceDiagnosticsBaseline {
    uint32_t struct_size;
    uint32_t api_version;
    char key[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint64_t fingerprint;
    uint64_t created_at;
    uint64_t expires_at;
    uint64_t source_generation;
    uint64_t revision;
    int valid;
} UmiLanguageIntelligenceDiagnosticsBaseline;
/**
 * Initialise language intelligence diagnostics baseline from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_diagnostics_baseline_init(
    UmiLanguageIntelligenceDiagnosticsBaseline *entry,
    const char *key,
    uint64_t fingerprint,
    uint64_t now,
    uint64_t ttl);
/**
 * Provide the language intelligence diagnostics baseline is fresh operation used by this
 * module and its client applications.
 */
int umi_language_intelligence_diagnostics_baseline_is_fresh(
    const UmiLanguageIntelligenceDiagnosticsBaseline *entry,
    uint64_t now,
    uint64_t expected_fingerprint);
/**
 * Provide the language intelligence diagnostics baseline invalidate operation used by this
 * module and its client applications.
 */
void umi_language_intelligence_diagnostics_baseline_invalidate(UmiLanguageIntelligenceDiagnosticsBaseline *entry);
#ifdef __cplusplus
}
#endif
#endif
