/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_intelligence_session.c
 * PURPOSE: Focused regression test for intelligence session.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/intelligence_session.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceIntelligenceSession session;
    umi_language_intelligence_intelligence_session_init(&session, "workspace");
    session.provider_count = 2U;
    umi_language_intelligence_intelligence_session_begin_request(&session);
    CHECK(umi_language_intelligence_intelligence_session_active_requests(&session) == 1U);
    umi_language_intelligence_intelligence_session_complete_request(&session, UMI_STATUS_OK);
    CHECK(umi_language_intelligence_intelligence_session_active_requests(&session) == 0U);
    CHECK(session.state == UMI_LANGUAGE_INTELLIGENCE_READY);
    return 0;
}
