/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/provider_recovery.c
 *
 * PURPOSE:
 *   Implement decide bounded provider restart/recovery actions after failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_recovery.h"
/*
 * Provide the language intelligence provider recovery capabilities satisfied operation
 * used by this module and its client applications.
 */
int umi_language_intelligence_provider_recovery_capabilities_satisfied(
    const UmiLanguageIntelligenceProviderRecoveryInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
/*
 * Provide the language intelligence provider recovery evaluate operation used by this
 * module and its client applications.
 */
UmiLanguageIntelligenceProviderRecoveryDecision umi_language_intelligence_provider_recovery_evaluate(
    const UmiLanguageIntelligenceProviderRecoveryInput *input)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_language_intelligence_provider_recovery_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_DENY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_ALLOW;
}
