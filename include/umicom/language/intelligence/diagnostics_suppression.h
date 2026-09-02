/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/diagnostics_suppression.h
 *
 * PURPOSE:
 *   Evaluate explicit diagnostic suppression policy.
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

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_API_VERSION 1U
/**
 * List the named language intelligence diagnostics suppression decision values accepted by
 * this public contract.
 */
typedef enum UmiLanguageIntelligenceDiagnosticsSuppressionDecision {
    UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_DENY = 0,
    UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_ALLOW = 1,
    UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_ALLOW_WITH_REFRESH = 2
} UmiLanguageIntelligenceDiagnosticsSuppressionDecision;
/**
 * Represent the language intelligence diagnostics suppression input data shared with
 * callers of this public contract.
 */
typedef struct UmiLanguageIntelligenceDiagnosticsSuppressionInput {
    UmiLanguageIntelligenceCapabilityFlags required_flags;
    UmiLanguageIntelligenceCapabilityFlags available_flags;
    uint32_t risk;
    uint32_t stale_age;
    uint32_t threshold;
    int hard_block;
    int explicit_override;
} UmiLanguageIntelligenceDiagnosticsSuppressionInput;
/**
 * Provide the language intelligence diagnostics suppression evaluate operation used by
 * this module and its client applications.
 */
UmiLanguageIntelligenceDiagnosticsSuppressionDecision umi_language_intelligence_diagnostics_suppression_evaluate(
    const UmiLanguageIntelligenceDiagnosticsSuppressionInput *input);
/**
 * Provide the language intelligence diagnostics suppression capabilities satisfied
 * operation used by this module and its client applications.
 */
int umi_language_intelligence_diagnostics_suppression_capabilities_satisfied(
    const UmiLanguageIntelligenceDiagnosticsSuppressionInput *input);
#ifdef __cplusplus
}
#endif
#endif
