/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_language_filter.c
 * PURPOSE: Verify reusable language inference and native-dialog filter data.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiDocumentLanguageIdentity language;
    UmiDocumentFileFilterSet filters;
    assert(umi_document_language_detect("engine.c", &language) == UMI_STATUS_OK);
    assert(strcmp(language.language_id, "c") == 0);
    assert(umi_document_language_detect("native.S", &language) == UMI_STATUS_OK);
    assert(strcmp(language.language_id, "asm") == 0);
    assert(umi_document_file_filters_default(&filters) == UMI_STATUS_OK);
    assert(filters.count == 5U);
    assert(umi_document_file_filter_matches(&filters.filters[0], "engine.c"));
    assert(!umi_document_file_filter_matches(&filters.filters[0], "readme.md"));
    (void)printf("document language/filter passed\n");
    return 0;
}
