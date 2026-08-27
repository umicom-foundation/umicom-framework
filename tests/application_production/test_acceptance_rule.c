/* Umicom Framework application production test | acceptance_rule | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiApplicationProductionAcceptanceRule rule = umi_application_production_acceptance_rule_default();
    assert(umi_application_production_acceptance_rule_validate(&rule) == UMI_STATUS_OK);
    assert(rule.require_manifest && rule.require_evidence);
    return 0;
}

