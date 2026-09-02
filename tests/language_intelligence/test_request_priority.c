/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_request_priority.c
 * PURPOSE: Focused regression test for request priority.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_priority.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceRequestPriorityInput left;
    UmiLanguageIntelligenceRequestPriorityInput right;
    umi_language_intelligence_request_priority_input_init(&left);
    umi_language_intelligence_request_priority_input_init(&right);
    left.base_priority = 10U;
    left.exact_match = 1U;
    left.health = 10U;
    right.base_priority = 20U;
    CHECK(umi_language_intelligence_request_priority_score(&left) > umi_language_intelligence_request_priority_score(&right));
    CHECK(umi_language_intelligence_request_priority_prefer(&left, &right) != 0);
    return 0;
}
