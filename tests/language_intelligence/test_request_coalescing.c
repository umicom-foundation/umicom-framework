/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_request_coalescing.c
 * PURPOSE: Focused regression test for request coalescing.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_coalescing.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceRequestCoalescingInput input = {0};
    input.required_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_NAVIGATION;
    input.available_flags = UMI_LANGUAGE_INTELLIGENCE_CAP_ALL;
    input.threshold = 10U;
    input.risk = 2U;
    CHECK(umi_language_intelligence_request_coalescing_capabilities_satisfied(&input) != 0);
    CHECK(umi_language_intelligence_request_coalescing_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_ALLOW);
    input.stale_age = 20U;
    CHECK(umi_language_intelligence_request_coalescing_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_ALLOW_WITH_REFRESH);
    input.hard_block = 1;
    CHECK(umi_language_intelligence_request_coalescing_evaluate(&input) == UMI_LANGUAGE_INTELLIGENCE_REQUEST_COALESCING_DENY);
    return 0;
}
