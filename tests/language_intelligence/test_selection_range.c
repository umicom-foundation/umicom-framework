/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_selection_range.c
 * PURPOSE: Focused regression test for selection range.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/selection_range.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceSelectionRange value;
    UmiLanguageIntelligenceRange primary;
    UmiLanguageIntelligenceRange parent;
    umi_language_intelligence_selection_range_init(&value, "file:///test.c");
    umi_language_intelligence_types_init_range(&parent, 1U, 0U, 10U, 0U);
    umi_language_intelligence_types_init_range(&primary, 2U, 0U, 3U, 5U);
    CHECK(umi_language_intelligence_selection_range_set_ranges(&value, &primary, &parent) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_selection_range_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_selection_range_is_nested(&value) != 0);
    return 0;
}
