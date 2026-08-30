/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_code_lens.c
 * PURPOSE: Focused regression test for code lens.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/code_lens.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceCodeLens value;
    UmiLanguageIntelligenceCodeLens other;
    umi_language_intelligence_code_lens_init(&value, "code_lens.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_code_lens_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_code_lens_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_code_lens_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_code_lens_init(&other, "code_lens.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_code_lens_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
