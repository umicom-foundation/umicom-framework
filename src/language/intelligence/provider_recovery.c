/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language/intelligence/provider_recovery.c
 *
 * PURPOSE:
 *   Implement decide bounded provider restart/recovery actions after failure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/provider_recovery.h"
int umi_language_intelligence_provider_recovery_capabilities_satisfied(
    const UmiLanguageIntelligenceProviderRecoveryInput *input)
{
    return input != NULL &&
        (input->available_flags & input->required_flags) == input->required_flags;
}
UmiLanguageIntelligenceProviderRecoveryDecision umi_language_intelligence_provider_recovery_evaluate(
    const UmiLanguageIntelligenceProviderRecoveryInput *input)
{
    if (input == NULL || input->hard_block != 0)
        return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_DENY;
    if (!umi_language_intelligence_provider_recovery_capabilities_satisfied(input) && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_DENY;
    if (input->risk > input->threshold && input->explicit_override == 0)
        return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_DENY;
    if (input->stale_age > input->threshold)
        return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_ALLOW_WITH_REFRESH;
    return UMI_LANGUAGE_INTELLIGENCE_PROVIDER_RECOVERY_ALLOW;
}
