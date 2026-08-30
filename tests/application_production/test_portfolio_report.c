/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_portfolio_report.c
 *
 * PURPOSE:
 *   Implement the test portfolio report behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | portfolio_report | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include "test_fixture.h"
int main(void) {
    UmiApplicationProductionPortfolio *portfolio = calloc(1U, sizeof(*portfolio));
    UmiApplicationProductionPortfolioReport report;
    assert(portfolio != NULL);
    assert(umi_application_production_portfolio_build(umi_test_application_production_all_available, NULL, portfolio) == UMI_STATUS_OK);
    assert(umi_application_production_portfolio_report_build(portfolio, &report) == UMI_STATUS_OK);
    assert(report.application_count == portfolio->count);
    assert(report.panel_count > 0U && report.feature_count > 0U);
    free(portfolio);
    return 0;
}

