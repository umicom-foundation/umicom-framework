/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_document_ownership.c
 * PURPOSE: Focused regression test for document ownership.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/document_ownership.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

int main(void)
{
    UmiLanguageIntelligenceDocumentOwnership mapping;
    umi_language_intelligence_document_ownership_init(&mapping);
    CHECK(umi_language_intelligence_document_ownership_set(&mapping, "c", "clangd", "workspace") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_document_ownership_validate(&mapping) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_document_ownership_matches(&mapping, "c", "workspace") != 0);
    CHECK(umi_language_intelligence_document_ownership_matches(&mapping, "cpp", "workspace") == 0);
    return 0;
}
