/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_acceptance_rule.c
 *
 * PURPOSE:
 *   Implement the test acceptance rule behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | acceptance_rule | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiApplicationProductionAcceptanceRule rule = umi_application_production_acceptance_rule_default();
    assert(umi_application_production_acceptance_rule_validate(&rule) == UMI_STATUS_OK);
    assert(rule.require_manifest && rule.require_evidence);
    return 0;
}

