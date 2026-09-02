/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_intelligence/test_document_routing.c
 * PURPOSE: Focused regression test for document routing.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language/intelligence/document_routing.h"
#define CHECK(expression) do { if (!(expression)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiLanguageIntelligenceDocumentRouting mapping;
    umi_language_intelligence_document_routing_init(&mapping);
    CHECK(umi_language_intelligence_document_routing_set(&mapping, "c", "clangd", "workspace") == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_document_routing_validate(&mapping) == UMI_STATUS_OK);
    CHECK(umi_language_intelligence_document_routing_matches(&mapping, "c", "workspace") != 0);
    CHECK(umi_language_intelligence_document_routing_matches(&mapping, "cpp", "workspace") == 0);
    return 0;
}
