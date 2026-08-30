/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/diagnostics_suppression.c
 *
 * PURPOSE:
 *   Implement evaluate explicit diagnostic suppression policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/diagnostics_suppression.h"
int umi_language_intelligence_diagnostics_suppression_capabilities_satisfied(
    const UmiLanguageIntelligenceDiagnosticsSuppressionInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
UmiLanguageIntelligenceDiagnosticsSuppressionDecision umi_language_intelligence_diagnostics_suppression_evaluate(
    const UmiLanguageIntelligenceDiagnosticsSuppressionInput *input)
{
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_DENY;
    if (!umi_language_intelligence_diagnostics_suppression_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_DENY;
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_DENY;
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_SUPPRESSION_ALLOW;
}
