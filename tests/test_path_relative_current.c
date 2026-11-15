/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_path_relative_current.c
 *
 * PURPOSE:
 *   Verify current-directory lexical paths without inventing parent traversal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/path.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
typedef struct RelativeCase { const char *name, *base, *target, *expected; } RelativeCase;
static const RelativeCase cases[] = {
    {"dot-child", ".", "src/main.c", "src/main.c"},
    {"dot-prefixed", "./", "./src/main.c", "src/main.c"},
    {"empty-base", "", "src/main.c", "src/main.c"},
    {"collapsed-base", "folder/..", "src/main.c", "src/main.c"},
    {"dot-dot", ".", ".", "."},
    {"empty-empty", "", "", "."},
    {"child-dot", "src", ".", ".."},
    {"child-empty", "src", "", ".."},
    {"nested-dot", "src/nested", ".", "../.."},
    {"child-collapsed", "src", "folder/..", ".."},
    {"ordinary-child", "project", "project/src/main.c", "src/main.c"},
    {"ordinary-parent", "project/src", "project/main.c", "../main.c"},
    {"parent-base", "..", "../src/main.c", "src/main.c"},
    {"nested-parent-base", "../..", "../../src/main.c", "src/main.c"},
    {"sibling-prefix", "project", "project-other/main.c", "../project-other/main.c"},
    {"mixed-separators", "project\\src", "project/src\\nested/file.c", "nested/file.c"},
    {"unicode", ".", "caf\xc3\xa9/\xce\xb4.c", "caf\xc3\xa9/\xce\xb4.c"},
    {"root-child", "/project", "/project/src/main.c", "src/main.c"}
};
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    char output[UMI_PATH_CAPACITY];
    if (!strcmp(argv[1], "arguments")) {
        CHECK(umi_path_relative(NULL, ".", output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_path_relative(".", NULL, output, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_path_relative(".", ".", NULL, sizeof output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_path_relative(".", ".", output, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (!strcmp(argv[1], "capacity")) {
        CHECK(umi_path_relative(".", "source.c", output, sizeof "source.c" - 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_path_relative(".", "source.c", output, sizeof "source.c") == UMI_STATUS_OK);
        CHECK(!strcmp(output, "source.c"));
        CHECK(umi_path_relative(".", ".", output, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_path_relative(".", ".", output, 2U) == UMI_STATUS_OK);
        CHECK(!strcmp(output, "."));
        return 0;
    }
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) {
        if (strcmp(argv[1], cases[i].name)) continue;
        CHECK(umi_path_relative(cases[i].base, cases[i].target, output, sizeof output) == UMI_STATUS_OK);
        char expected[UMI_PATH_CAPACITY];
        CHECK(umi_path_normalise(cases[i].expected, expected, sizeof expected) == UMI_STATUS_OK);
        CHECK(!strcmp(output, expected));
        return 0;
    }
    return 2;
}
