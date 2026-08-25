/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_document_link.c
 * PURPOSE: Focused regression test for document link.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/document_link.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceDocumentLink value;
    UmiLanguageIntelligenceDocumentLink other;
    umi_language_intelligence_document_link_init(&value, "document_link.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_document_link_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_document_link_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_document_link_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_document_link_init(&other, "document_link.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_document_link_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
