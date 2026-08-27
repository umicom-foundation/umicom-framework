/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/acceptance_rule.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/acceptance_rule.h"

UmiApplicationProductionAcceptanceRule
umi_application_production_acceptance_rule_default(void)
{
    UmiApplicationProductionAcceptanceRule rule = {
        1, 1, 1, 1, 1, 1
    };
    return rule;
}

UmiStatus umi_application_production_acceptance_rule_validate(
    const UmiApplicationProductionAcceptanceRule *rule)
{
    if (rule == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!rule->require_manifest && !rule->require_layout_projection &&
        !rule->require_capabilities && !rule->require_tests &&
        !rule->require_evidence)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

