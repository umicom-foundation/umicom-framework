/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_request_scheduler.c
 * PURPOSE: Focused regression test for request scheduler.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_scheduler.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceRequestScheduler value;
    UmiLanguageIntelligenceRequestScheduler other;
    umi_language_intelligence_request_scheduler_init(&value, "request_scheduler.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_request_scheduler_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_request_scheduler_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_request_scheduler_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_request_scheduler_init(&other, "request_scheduler.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_request_scheduler_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
