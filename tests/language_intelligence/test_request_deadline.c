/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_request_deadline.c
 * PURPOSE: Focused regression test for request deadline.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_deadline.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceRequestDeadline budget;
    umi_language_intelligence_request_deadline_init(&budget, 10U);
    CHECK(umi_language_intelligence_request_deadline_consume(&budget, 4U) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_request_deadline_remaining(&budget) == 6U);
    CHECK(umi_language_intelligence_request_deadline_consume(&budget, 7U) == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_language_intelligence_request_deadline_reset(&budget);
    CHECK(umi_language_intelligence_request_deadline_remaining(&budget) == 10U);
    return 0;
}
