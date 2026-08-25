/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/diagnostics_deduplication.c
 *
 * PURPOSE:
 *   Implement detect duplicate diagnostics using stable fingerprints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/diagnostics_deduplication.h"
int umi_language_intelligence_diagnostics_deduplication_capabilities_satisfied(
    const UmiLanguageIntelligenceDiagnosticsDeduplicationInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
UmiLanguageIntelligenceDiagnosticsDeduplicationDecision umi_language_intelligence_diagnostics_deduplication_evaluate(
    const UmiLanguageIntelligenceDiagnosticsDeduplicationInput *input)
{
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_DENY;
    if (!umi_language_intelligence_diagnostics_deduplication_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_DENY;
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_DENY;
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_DEDUPLICATION_ALLOW;
}
