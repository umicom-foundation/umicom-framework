/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_path_policy.c
 *
 * PURPOSE:
 *   Verify workspace-relative exclusion components and duplicate template selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/scanner.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
/* Include the production coordinator once to inspect its private policy.
 * Shared normalization, comparisons and scanner dependencies stay real. */
#include "../../src/codeguard/scanner.c"
typedef struct PolicyCase {
    const char *name, *root, *visited;
    UmiStatus status;
    int excluded, duplicate;
} PolicyCase;
static const PolicyCase cases[] = {
    {"build-ancestor", "/build/projects/app", "/build/projects/app/src/main.c", UMI_STATUS_OK, 0, 1},
    {"vendor-ancestor", "/third_party/app", "/third_party/app/main.c", UMI_STATUS_OK, 0, 1},
    {"rules-ancestor", "/codeguard/rules/app", "/codeguard/rules/app/main.c", UMI_STATUS_OK, 0, 1},
    {"template-ancestor", "/templates/workspace/app", "/templates/workspace/app/main.c", UMI_STATUS_OK, 0, 1},
    {"dot", ".", "./src/main.c", UMI_STATUS_OK, 0, 1},
    {"parent-root", "..", "../src/main.c", UMI_STATUS_OK, 0, 1},
    {"collapsed-root", "nested/..", "./src/main.c", UMI_STATUS_OK, 0, 1},
    {"top-build", ".", "build/main.c", UMI_STATUS_OK, 1, 1},
    {"nested-build", "app", "app/sub/build/main.c", UMI_STATUS_OK, 1, 1},
    {"git", "app", "app/.git/main.c", UMI_STATUS_OK, 1, 1},
    {"cpack", "app", "app/_CPack_Packages/main.c", UMI_STATUS_OK, 1, 1},
    {"node", "app", "app/node_modules/main.c", UMI_STATUS_OK, 1, 1},
    {"vendor", "app", "app/third_party/main.c", UMI_STATUS_OK, 1, 1},
    {"rules", "app", "app/codeguard/rules/sample.c", UMI_STATUS_OK, 1, 1},
    {"nonadjacent-rules", "app", "app/codeguard/other/rules/sample.c", UMI_STATUS_OK, 0, 1},
    {"build-prefix", "app", "app/build-tools/main.c", UMI_STATUS_OK, 0, 1},
    {"build-file", "app", "app/src/build.c", UMI_STATUS_OK, 0, 1},
    {"rules-prefix", "app", "app/codeguard/rules-extra/main.c", UMI_STATUS_OK, 0, 1},
    {"template", "app", "app/templates/workspace/source.c", UMI_STATUS_OK, 0, 0},
    {"nested-template", "app", "app/sub/templates/workspace/source.c", UMI_STATUS_OK, 0, 0},
    {"template-prefix", "app", "app/templates/workspace-extra/source.c", UMI_STATUS_OK, 0, 1},
    {"template-nonadjacent", "app", "app/templates/other/workspace/source.c", UMI_STATUS_OK, 0, 1},
    {"mixed", "app", "app\\sub/build\\source.c", UMI_STATUS_OK, 1, 1},
    {"unicode", "caf\xc3\xa9", "caf\xc3\xa9/\xce\xb4/main.c", UMI_STATUS_OK, 0, 1},
    {"outside-prefix", "/project", "/project-other/main.c", UMI_STATUS_INVALID_STATE, 0, 0},
    {"outside-parent", "/project", "/elsewhere/main.c", UMI_STATUS_INVALID_STATE, 0, 0},
    {"same-root", "/project", "/project", UMI_STATUS_INVALID_STATE, 0, 0},
#ifdef _WIN32
    {"platform-case", "APP", "app/BUILD/main.c", UMI_STATUS_OK, 1, 1},
#else
    {"platform-case", "app", "app/BUILD/main.c", UMI_STATUS_OK, 0, 1},
#endif
};
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    char relative[UMI_PATH_CAPACITY];
    if (!strcmp(argv[1], "arguments")) {
        CHECK(umi_codeguard_scanner_relative_path(NULL, "a.c", relative, sizeof relative) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_scanner_relative_path(".", NULL, relative, sizeof relative) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_scanner_relative_path(".", "a.c", NULL, sizeof relative) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_scanner_relative_path(".", "a.c", relative, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_scanner_relative_path(".", "a.c", relative, 3U) == UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    }
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) {
        const PolicyCase *item = &cases[i];
        if (strcmp(argv[1], item->name)) continue;
        CHECK(umi_codeguard_scanner_relative_path(item->root, item->visited,
            relative, sizeof relative) == item->status);
        if (item->status == UMI_STATUS_OK) {
            /* Retain the pre-profile call as the default-policy expectation. */
#if 0
            CHECK(umi_codeguard_scanner_path_excluded(relative) == item->excluded);
#endif
            CHECK(umi_codeguard_scanner_path_excluded(relative, 0) == item->excluded);
            /* Expected policy is named independently of production matching. */
            int generated_directory = !strcmp(item->name, "top-build") ||
                !strcmp(item->name, "nested-build") || !strcmp(item->name, "cpack") ||
                !strcmp(item->name, "mixed");
#ifdef _WIN32
            generated_directory = generated_directory || !strcmp(item->name, "platform-case");
#endif
            CHECK(umi_codeguard_scanner_path_excluded(relative, 1) ==
                (generated_directory ? 0 : item->excluded));
            CHECK(umi_codeguard_scanner_duplicate_candidate(relative) == item->duplicate);
        }
        return 0;
    }
    return 2;
}
