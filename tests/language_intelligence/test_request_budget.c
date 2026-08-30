/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_request_budget.c
 * PURPOSE: Focused regression test for request budget.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_budget.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceRequestBudget budget;
    umi_language_intelligence_request_budget_init(&budget, 10U);
    CHECK(umi_language_intelligence_request_budget_consume(&budget, 4U) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_request_budget_remaining(&budget) == 6U);
    CHECK(umi_language_intelligence_request_budget_consume(&budget, 7U) == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_language_intelligence_request_budget_reset(&budget);
    CHECK(umi_language_intelligence_request_budget_remaining(&budget) == 10U);
    return 0;
}
