/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_request_throttle.c
 * PURPOSE: Focused regression test for request throttle.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_throttle.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceRequestThrottle budget;
    umi_language_intelligence_request_throttle_init(&budget, 10U);
    CHECK(umi_language_intelligence_request_throttle_consume(&budget, 4U) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_request_throttle_remaining(&budget) == 6U);
    CHECK(umi_language_intelligence_request_throttle_consume(&budget, 7U) == UMI_STATUS_CAPACITY_EXCEEDED);
    umi_language_intelligence_request_throttle_reset(&budget);
    CHECK(umi_language_intelligence_request_throttle_remaining(&budget) == 10U);
    return 0;
}
