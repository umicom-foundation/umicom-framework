/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_inline_value.c
 * PURPOSE: Focused regression test for inline value.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/inline_value.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceInlineValue value;
    UmiLanguageIntelligenceInlineValue other;
    umi_language_intelligence_inline_value_init(&value, "inline_value.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_inline_value_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_inline_value_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_inline_value_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_inline_value_init(&other, "inline_value.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_inline_value_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
