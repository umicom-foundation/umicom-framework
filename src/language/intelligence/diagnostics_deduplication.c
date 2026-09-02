/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/diagnostics_deduplication.c
 *
 * PURPOSE:
 *   Implement detect duplicate diagnostics using stable fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/diagnostics_deduplication.h"
/*
 * Provide the language intelligence diagnostics deduplication capabilities satisfied
 * operation used by this module and its client applications.
 */
int umi_language_intelligence_diagnostics_deduplication_capabilities_satisfied(
    const UmiLanguageIntelligenceDiagnosticsDeduplicationInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
/*
 * Provide the language intelligence diagnostics deduplication evaluate operation used by
 * this module and its client applications.
 */
UmiLanguageIntelligenceDiagnosticsDeduplicationDecision umi_language_intelligence_diagnostics_deduplication_evaluate(
    const UmiLanguageIntelligenceDiagnosticsDeduplicationInput *input)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_language_intelligence_diagnostics_deduplication_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_ALLOW;
}
