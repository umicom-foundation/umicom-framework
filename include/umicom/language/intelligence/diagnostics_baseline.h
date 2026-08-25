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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_BASELINE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_BASELINE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_BASELINE_API_VERSION 1U
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
void umi_language_intelligence_diagnostics_baseline_init(
    UmiLanguageIntelligenceDiagnosticsBaseline *entry,
    const char *key,
    uint64_t fingerprint,
    uint64_t now,
    uint64_t ttl);
int umi_language_intelligence_diagnostics_baseline_is_fresh(
    const UmiLanguageIntelligenceDiagnosticsBaseline *entry,
    uint64_t now,
    uint64_t expected_fingerprint);
void umi_language_intelligence_diagnostics_baseline_invalidate(UmiLanguageIntelligenceDiagnosticsBaseline *entry);
#ifdef __cplusplus
}
#endif
#endif
