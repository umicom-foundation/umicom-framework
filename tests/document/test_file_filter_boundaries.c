/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_file_filter_boundaries.c
 *
 * PURPOSE:
 *   Verify bounded file-filter records, basename matching and multilingual defaults.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/file_filter.h"
#include "umicom/platform/path.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; \
} } while (0)

/* Compare defined fields so structure padding cannot affect a rejection check. */
static int FilterEqual(const UmiDocumentFileFilter *left, const UmiDocumentFileFilter *right)
{
    return left->pattern_count == right->pattern_count &&
        memcmp(left->name, right->name, sizeof left->name) == 0 &&
        memcmp(left->patterns, right->patterns, sizeof left->patterns) == 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiDocumentFileFilter filter = {0}, before;
    const char *name = argv[1];
    if (!strcmp(name, "malformed-count")) {
        filter.pattern_count = SIZE_MAX;
        CHECK(!umi_document_file_filter_matches(&filter, "main.c"));
        before = filter;
        CHECK(umi_document_file_filter_add_pattern(&filter, "*.c") == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(FilterEqual(&before, &filter));
    } else if (!strcmp(name, "unterminated-pattern")) {
        strcpy(filter.patterns[0], "*");
        memset(filter.patterns[1], 'x', sizeof filter.patterns[1]);
        filter.pattern_count = 2U;
        CHECK(!umi_document_file_filter_matches(&filter, "main.c"));
    } else if (!strcmp(name, "bounded-add")) {
        char pattern[UMI_DOCUMENT_FILE_FILTER_TEXT_CAPACITY];
        memset(pattern, 'x', sizeof pattern);
        before = filter;
        CHECK(umi_document_file_filter_add_pattern(&filter, pattern) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(FilterEqual(&before, &filter));
        pattern[sizeof pattern - 1U] = '\0';
        CHECK(umi_document_file_filter_add_pattern(&filter, pattern) == UMI_STATUS_OK);
        CHECK(filter.pattern_count == 1U && !strcmp(filter.patterns[0], pattern));
    } else if (!strcmp(name, "full-capacity")) {
        for (size_t i = 0U; i < UMI_DOCUMENT_FILE_FILTER_PATTERN_MAX; ++i)
            CHECK(umi_document_file_filter_add_pattern(&filter, "*.c") == UMI_STATUS_OK);
        before = filter;
        CHECK(umi_document_file_filter_add_pattern(&filter, "*") == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(FilterEqual(&before, &filter));
        CHECK(umi_document_file_filter_matches(&filter, "main.c"));
    } else if (!strcmp(name, "overlap")) {
        strcpy(filter.patterns[0], "*.c");
        CHECK(umi_document_file_filter_add_pattern(&filter, filter.patterns[0]) == UMI_STATUS_OK);
        CHECK(umi_document_file_filter_add_pattern(&filter, filter.patterns[0]) == UMI_STATUS_OK);
        CHECK(filter.pattern_count == 2U && !strcmp(filter.patterns[1], "*.c"));
    } else if (!strcmp(name, "bounded-name")) {
        char filename[UMI_PATH_CAPACITY];
        memset(filename, 'x', sizeof filename);
        CHECK(umi_document_file_filter_add_pattern(&filter, "*") == UMI_STATUS_OK);
        CHECK(!umi_document_file_filter_matches(&filter, filename));
        filename[sizeof filename - 1U] = '\0';
        CHECK(umi_document_file_filter_matches(&filter, filename));
    } else if (!strcmp(name, "basename")) {
        CHECK(umi_document_file_filter_add_pattern(&filter, "*.c") == UMI_STATUS_OK);
        CHECK(umi_document_file_filter_matches(&filter, "C:\\code dir\\MAIN.C"));
        CHECK(umi_document_file_filter_matches(&filter, "/src/main.c"));
        CHECK(!umi_document_file_filter_matches(&filter, "/directory.c/noextension"));
        CHECK(!umi_document_file_filter_matches(&filter, "/directory.c/"));
        CHECK(!umi_document_file_filter_matches(&filter, "main.c.bak"));
    } else if (!strcmp(name, "literal")) {
        CHECK(umi_document_file_filter_add_pattern(&filter, "CMakeLists.txt") == UMI_STATUS_OK);
        CHECK(umi_document_file_filter_matches(&filter, "project/CMakeLists.txt"));
        CHECK(umi_document_file_filter_matches(&filter, "project/cmakelists.txt"));
        CHECK(!umi_document_file_filter_matches(&filter, "project/MyCMakeLists.txt"));
    } else if (!strcmp(name, "wildcards")) {
        CHECK(umi_document_file_filter_add_pattern(&filter, "*.*") == UMI_STATUS_OK);
        CHECK(umi_document_file_filter_matches(&filter, "README"));
        CHECK(!umi_document_file_filter_matches(&filter, ""));
        CHECK(!umi_document_file_filter_matches(&filter, "/"));
    } else if (!strcmp(name, "defaults")) {
        UmiDocumentFileFilterSet set;
        CHECK(umi_document_file_filters_default(&set) == UMI_STATUS_OK);
        CHECK(set.count == 8U && !strcmp(set.filters[4].name, "All files"));
        const char *sources[] = {"first.uc", "app.zig", "app.d", "app.dart", "app.pas",
            "app.php", "cpu.vhd", "cpu.sv", "app.java", "app.kt", "CMakeLists.txt"};
        for (size_t n = 0U; n < sizeof sources / sizeof sources[0]; ++n) {
            int selected = 0;
            for (size_t i = 0U; i < set.count; ++i)
                if (i != 4U) selected |= umi_document_file_filter_matches(&set.filters[i], sources[n]);
            CHECK(selected);
        }
    } else if (!strcmp(name, "arguments")) {
        CHECK(!umi_document_file_filter_matches(NULL, "x.c"));
        CHECK(!umi_document_file_filter_matches(&filter, NULL));
        CHECK(umi_document_file_filter_add_pattern(NULL, "*") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_document_file_filter_add_pattern(&filter, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_document_file_filter_add_pattern(&filter, "") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_document_file_filters_default(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
