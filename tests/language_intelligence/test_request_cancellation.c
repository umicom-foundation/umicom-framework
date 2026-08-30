/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_request_cancellation.c
 * PURPOSE: Focused regression test for request cancellation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/request_cancellation.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceRequestCancellation value;
    umi_language_intelligence_request_cancellation_init(&value, "request_cancellation");
    CHECK(umi_language_intelligence_request_cancellation_transition(&value, UMI_LANGUAGE_INTELLIGENCE_STARTING, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_request_cancellation_transition(&value, UMI_LANGUAGE_INTELLIGENCE_READY, 0) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_request_cancellation_is_usable(&value) != 0);
    umi_language_intelligence_request_cancellation_record_failure(&value);
    CHECK(value.state == UMI_LANGUAGE_INTELLIGENCE_DEGRADED);
    return 0;
}
