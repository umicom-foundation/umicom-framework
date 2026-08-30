/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_audit.c
 *
 * PURPOSE:
 *   Implement the test audit behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | audit | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include "test_fixture.h"
int main(void) {
    UmiApplicationProductionPortfolio *portfolio = calloc(1U, sizeof(*portfolio));
    UmiApplicationProductionGapAudit audit;
    assert(portfolio != NULL);
    assert(umi_application_production_portfolio_build(umi_test_application_production_all_available, NULL, portfolio) == UMI_STATUS_OK);
    assert(umi_application_production_gap_audit_build(portfolio, &audit) == UMI_STATUS_OK);
    assert(audit.application_count == portfolio->count);
    assert(audit.feature_count == audit.framework_feature_count + audit.application_feature_count + audit.external_adapter_feature_count);
    free(portfolio);
    return 0;
}

