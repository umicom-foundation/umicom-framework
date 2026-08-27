/* Umicom Framework application production test | portfolio | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include "test_fixture.h"
#include "umicom/application/experience_catalogue.h"
int main(void) {
    UmiApplicationProductionPortfolio *portfolio = calloc(1U, sizeof(*portfolio));
    assert(portfolio != NULL);
    assert(umi_application_production_portfolio_build(umi_test_application_production_all_available, NULL, portfolio) == UMI_STATUS_OK);
    assert(portfolio->count == umi_application_experience_catalogue_count());
    assert(umi_application_production_portfolio_find(portfolio, "org.umicom.trader") != NULL);
    free(portfolio);
    return 0;
}
