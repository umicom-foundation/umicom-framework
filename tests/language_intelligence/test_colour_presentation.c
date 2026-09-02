/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_colour_presentation.c
 * PURPOSE: Focused regression test for colour presentation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/colour_presentation.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceColourPresentation value;
    UmiLanguageIntelligenceColourPresentation other;
    umi_language_intelligence_colour_presentation_init(&value, "colour_presentation.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_colour_presentation_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_colour_presentation_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_colour_presentation_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_colour_presentation_init(&other, "colour_presentation.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_colour_presentation_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
