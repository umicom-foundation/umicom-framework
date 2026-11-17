/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_native_language_support.c
 *
 * PURPOSE:
 *   Verify native language detection, selection, commenting and non-wildcard file discovery agree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/languages.h"
#include "umicom/document/file_filter.h"
#include "umicom/editor/comment_style.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
int main(void)
{
    static const struct { const char *file, *id, *prefix; } rows[] = {
        {"example.c", "c", "//"}, {"example.cpp", "cpp", "//"},
        {"example.rs", "rust", "//"}, {"example.zig", "zig", "//"},
        {"example.d", "d", "//"}, {"example.di", "d", "//"},
        {"example.f90", "fortran", "!"}, {"example.F95", "fortran", "!"},
        {"example.f03", "fortran", "!"}, {"example.F08", "fortran", "!"},
        {"example.odin", "odin", "//"}, {"example.nim", "nim", "#"},
        {"example.NIMS", "nim", "#"}
    };
    UmiDocumentFileFilterSet filters;
    CHECK(umi_document_file_filters_default(&filters) == UMI_STATUS_OK);
    CHECK(filters.count <= UMI_DOCUMENT_FILE_FILTER_MAX);
    for (size_t r = 0; r < sizeof rows / sizeof rows[0]; ++r) {
        UmiDocumentLanguageIdentity inferred, selected;
        const UmiEditorLineCommentStyle *style = NULL;
        CHECK(umi_document_language_detect(rows[r].file, &inferred) == UMI_STATUS_OK);
        CHECK(!strcmp(inferred.language_id, rows[r].id));
        CHECK(UmiDocumentLanguageChoiceIdentity(rows[r].id, &selected) == UMI_STATUS_OK);
        CHECK(!strcmp(inferred.language_id, selected.language_id));
        CHECK(UmiEditorLineCommentStyleFind(rows[r].id, &style) == UMI_STATUS_OK);
        CHECK(!strcmp(style->prefix, rows[r].prefix));
        int discoverable = 0;
        for (size_t f = 0; f < filters.count; ++f) {
            CHECK(filters.filters[f].pattern_count <= UMI_DOCUMENT_FILE_FILTER_PATTERN_MAX);
            if (strcmp(filters.filters[f].name, "All files"))
                discoverable |= umi_document_file_filter_matches(&filters.filters[f], rows[r].file);
        }
        CHECK(discoverable);
    }
    /* Unsupported source formats stay explicit: fixed-form Fortran and a
     * directory name ending in .nim must not enable free-form comment edits. */
    UmiDocumentLanguageIdentity id;
    CHECK(umi_document_language_detect("legacy.f", &id) == UMI_STATUS_OK);
    CHECK(!strcmp(id.language_id, "plain"));
    CHECK(umi_document_language_detect("folder.nim/noextension", &id) == UMI_STATUS_OK);
    CHECK(!strcmp(id.language_id, "plain"));
    CHECK(umi_document_language_detect("function.S", &id) == UMI_STATUS_OK);
    CHECK(!strcmp(id.language_id, "asm"));
    return 0;
}
