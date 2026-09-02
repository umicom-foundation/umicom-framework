/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_file_watch_registration.c
 * PURPOSE: Focused regression test for file watch registration.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/file_watch_registration.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceFileWatchRegistration value;
    UmiLanguageIntelligenceFileWatchRegistration other;
    umi_language_intelligence_file_watch_registration_init(&value, "file_watch_registration.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_file_watch_registration_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_file_watch_registration_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_file_watch_registration_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_file_watch_registration_init(&other, "file_watch_registration.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_file_watch_registration_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
