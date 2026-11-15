/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_workspace_exclusions.c
 *
 * PURPOSE:
 *   Verify recursive source selection is independent of ancestor build directory names.
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
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1];
    const char *root = argv[2];
    char selected[UMI_PATH_CAPACITY];
    if (!strcmp(name, "relative")) root = "build/projects/app";
    else if (!strcmp(name, "relative-trailing")) root = "./build/projects/app/";
    else if (!strcmp(name, "dot")) root = ".";
    else if (!strcmp(name, "explicit-build-root")) {
        CHECK(umi_path_join(root, "build", selected, sizeof selected) == UMI_STATUS_OK);
        root = selected;
    } else if (strcmp(name, "absolute") && strcmp(name, "direct-file")) return 2;
    UmiCodeGuardConfig config = umi_codeguard_config_default(root);
    config.profile.scan_architecture = 0;
    config.profile.scan_duplicates = 0;
    config.profile.scan_source_names = 0;
    config.profile.max_line_length = 1U;
    UmiCodeGuardResult *result = NULL;
    CHECK(umi_codeguard_result_create(16U, &result) == UMI_STATUS_OK);
    UmiStatus status;
    if (!strcmp(name, "direct-file")) {
        CHECK(umi_path_join(root, "build/generated.c", selected, sizeof selected) == UMI_STATUS_OK);
        status = umi_codeguard_scan_file(&config, selected, result);
    } else status = umi_codeguard_scan(&config, result);
    CHECK(status == UMI_STATUS_OK);
    size_t expected = (!strcmp(name, "explicit-build-root") || !strcmp(name, "direct-file")) ? 1U : 8U;
    CHECK(umi_codeguard_result_count(result) == expected);
    for (size_t i = 0U; i < expected; ++i) {
        const UmiCodeGuardFinding *item = umi_codeguard_result_at(result, i);
        CHECK(item != NULL && !strcmp(item->rule_id, "CODEGUARD-QUALITY-LINE-001"));
        CHECK(item->line == 1U);
    }
    umi_codeguard_result_destroy(result);
    return 0;
}
