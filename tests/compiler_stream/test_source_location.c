/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_stream/test_source_location.c
 * PURPOSE: Verify explicit compiler source resolution and refusal of ambiguous or escaped relative paths.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/diagnostic_location.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiBuildDiagnosticPage *page = calloc(1U, sizeof *page);
    CHECK(page != NULL);
    char root[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY], generated[UMI_PATH_CAPACITY];
    CHECK(umi_fs_current_directory(root, sizeof root) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "source", page->source_directory, sizeof page->source_directory) ==
          UMI_STATUS_OK);
    CHECK(umi_path_join(root, "build", page->build_directory, sizeof page->build_directory) ==
          UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(page->source_directory) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(page->build_directory) == UMI_STATUS_OK);
    CHECK(umi_path_join(page->source_directory, "sample.c", file, sizeof file) == UMI_STATUS_OK);
    CHECK(umi_path_join(page->build_directory, "sample.c", generated, sizeof generated) ==
          UMI_STATUS_OK);
    page->progress.operation_id = 4U;
    page->count = page->retained_count = 1U;
    page->items[0].line = 3U;
    page->items[0].column = 0U;
    strcpy(page->items[0].file, "sample.c");
    UmiStatus expected = UMI_STATUS_OK;
    const char *mode = argv[1];
    if (strcmp(mode, "build") == 0)
    {
        CHECK(umi_fs_write_text(generated, "one\ntwo\nthree\n") == UMI_STATUS_OK);
    }
    else
        CHECK(umi_fs_write_text(file, "one\ntwo\nthree\n") == UMI_STATUS_OK);
    if (strcmp(mode, "absolute") == 0)
        strcpy(page->items[0].file, file);
    else if (strcmp(mode, "ambiguous") == 0)
    {
        CHECK(umi_fs_write_text(generated, "different source\n") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    else if (strcmp(mode, "same-root") == 0)
        strcpy(page->build_directory, page->source_directory);
    else if (strcmp(mode, "no-line") == 0)
    {
        page->items[0].line = 0U;
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(mode, "uri") == 0)
    {
        strcpy(page->items[0].file, "https://example.invalid/sample.c");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "pseudo") == 0)
    {
        strcpy(page->items[0].file, "<command-line>");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "unterminated") == 0)
    {
        memset(page->items[0].file, 'x', sizeof page->items[0].file);
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "missing") == 0)
    {
        strcpy(page->items[0].file, "absent.c");
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(mode, "escape") == 0)
    {
        char outside[UMI_PATH_CAPACITY];
        CHECK(umi_path_join(root, "outside.c", outside, sizeof outside) == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(outside, "outside\n") == UMI_STATUS_OK);
        strcpy(page->items[0].file, "../outside.c");
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(mode, "generated-source") == 0)
    {
        strcpy(page->items[0].file, "../source/sample.c");
    }
    else if (strcmp(mode, "bounds") == 0)
    {
        page->count = UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY + 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
#ifdef _WIN32
    if (strcmp(mode, "drive-relative") == 0)
    {
        strcpy(page->items[0].file, "C:sample.c");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "alternate-stream") == 0)
    {
        strcpy(page->items[0].file, "C:/source/sample.c:other");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "network") == 0)
    {
        strcpy(page->items[0].file, "\\\\server\\share\\sample.c");
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
#endif
    UmiBuildDiagnosticLocation output, before;
    memset(&output, 0x3a, sizeof output);
    before = output;
    CHECK(UmiBuildDiagnosticResolveLocation(page, 0U, &output) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(output.line == 3U && output.column == 1U);
        CHECK(umi_path_equal(output.path, strcmp(mode, "build") == 0 ? generated : file));
    }
    else
        CHECK(memcmp(&output, &before, sizeof output) == 0);
    free(page);
    return 0;
}
