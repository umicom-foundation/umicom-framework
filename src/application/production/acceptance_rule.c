/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/acceptance_rule.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/acceptance_rule.h"

/*
 * Provide the application production acceptance rule default operation used by this module
 * and its client applications.
 */
UmiApplicationProductionAcceptanceRule
umi_application_production_acceptance_rule_default(void)
{
    UmiApplicationProductionAcceptanceRule rule = {
        1, 1, 1, 1, 1, 1
    };
    return rule;
}

/*
 * Check that application production acceptance rule satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_production_acceptance_rule_validate(
    const UmiApplicationProductionAcceptanceRule *rule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rule == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!rule->require_manifest && !rule->require_layout_projection &&
        !rule->require_capabilities && !rule->require_tests &&
        !rule->require_evidence)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

