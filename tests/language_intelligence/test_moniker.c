/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_moniker.c
 * PURPOSE: Focused regression test for moniker.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/moniker.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceMoniker value;
    UmiLanguageIntelligenceMoniker other;
    umi_language_intelligence_moniker_init(&value, "moniker.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_moniker_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_moniker_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_moniker_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_moniker_init(&other, "moniker.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_moniker_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
