/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_linked_editing.c
 * PURPOSE: Focused regression test for linked editing.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/linked_editing.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceLinkedEditing value;
    UmiLanguageIntelligenceRange primary;
    UmiLanguageIntelligenceRange parent;
    umi_language_intelligence_linked_editing_init(&value, "file:///test.c");
    umi_language_intelligence_types_init_range(&parent, 1U, 0U, 10U, 0U);
    umi_language_intelligence_types_init_range(&primary, 2U, 0U, 3U, 5U);
    CHECK(umi_language_intelligence_linked_editing_set_ranges(&value, &primary, &parent) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_linked_editing_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_linked_editing_is_nested(&value) != 0);
    return 0;
}
