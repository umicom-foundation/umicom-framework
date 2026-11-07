/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_language_filter.c
 *
 * PURPOSE:
 *   Verify reusable language inference and native-dialog filter data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include "umicom/document/document.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDocumentLanguageIdentity language;
    UmiDocumentFileFilterSet filters;
    assert(umi_document_language_detect("engine.c", &language) == UMI_STATUS_OK);
    assert(strcmp(language.language_id, "c") == 0);
    assert(umi_document_language_detect("native.S", &language) == UMI_STATUS_OK);
    assert(strcmp(language.language_id, "asm") == 0);
    assert(umi_document_file_filters_default(&filters) == UMI_STATUS_OK);
    /* The original five-category expectation is retained to explain the
     * compatible append of language, hardware and build-file categories. */
#if 0
    assert(filters.count == 5U);
#endif
    assert(filters.count == 8U);
    assert(strcmp(filters.filters[4].name, "All files") == 0);
    assert(umi_document_file_filter_matches(&filters.filters[0], "example.uc"));
    assert(umi_document_file_filter_matches(&filters.filters[5], "example.zig"));
    assert(umi_document_file_filter_matches(&filters.filters[6], "CPU.VHD"));
    assert(umi_document_file_filter_matches(&filters.filters[7], "src/CMakeLists.txt"));
    assert(umi_document_file_filter_matches(&filters.filters[0], "engine.c"));
    assert(!umi_document_file_filter_matches(&filters.filters[0], "readme.md"));
    (void)printf("document language/filter passed\n");
    return 0;
}
