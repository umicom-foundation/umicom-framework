/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_notebook_document.c
 * PURPOSE: Focused regression test for notebook document.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/notebook_document.h"


#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceNotebookDocument value;
    UmiLanguageIntelligenceNotebookDocument other;
    umi_language_intelligence_notebook_document_init(&value, "notebook_document.one");
    umi_language_intelligence_types_init_range(&value.range, 1U, 0U, 1U, 5U);
    CHECK(umi_language_intelligence_notebook_document_set_subject(&value, "subject") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_notebook_document_set_detail(&value, "detail") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_notebook_document_validate(&value) == UMI_STATUS_OK);
    umi_language_intelligence_notebook_document_init(&other, "notebook_document.one");
    umi_language_intelligence_types_init_range(&other.range, 0U, 0U, 0U, 0U);
    CHECK(umi_language_intelligence_notebook_document_same_identity(&value, &other) != 0);
    CHECK(value.revision >= 3U);
    return 0;
}
