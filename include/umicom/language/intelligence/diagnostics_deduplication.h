/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/diagnostics_deduplication.h
 *
 * PURPOSE:
 *   Detect duplicate diagnostics using stable fingerprints.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_API_VERSION 1U
/**
 * List the named language intelligence diagnostics deduplication decision values accepted
 * by this public contract.
 */
typedef enum UmiLanguageIntelligenceDiagnosticsDeduplicationDecision {
    UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceDiagnosticsDeduplicationDecision;
/**
 * Represent the language intelligence diagnostics deduplication input data shared with
 * callers of this public contract.
 */
typedef struct UmiLanguageIntelligenceDiagnosticsDeduplicationInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceDiagnosticsDeduplicationInput;
/**
 * Provide the language intelligence diagnostics deduplication evaluate operation used by
 * this module and its client applications.
 */
UmiLanguageIntelligenceDiagnosticsDeduplicationDecision umi_language_intelligence_diagnostics_deduplication_evaluate(
    const UmiLanguageIntelligenceDiagnosticsDeduplicationInput *input);
/**
 * Provide the language intelligence diagnostics deduplication capabilities satisfied
 * operation used by this module and its client applications.
 */
int umi_language_intelligence_diagnostics_deduplication_capabilities_satisfied(
    const UmiLanguageIntelligenceDiagnosticsDeduplicationInput *input);
#ifdef __cplusplus
}
#endif
#endif
