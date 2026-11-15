/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_profile_selection.c
 *
 * PURPOSE:
 *   Verify language and generated-source options using real scanner traversal and duplicate collection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/scanner.h"
#include "umicom/codeguard/source.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
typedef struct SelectionCase {
    const char *name;
    int c, cpp, headers, generated, duplicates;
    size_t expected;
    const char *direct;
} SelectionCase;
/* Each fixture source contains one identical line. With duplicate comparison
 * disabled, a deliberately short line limit makes admission observable. In
 * duplicate cases, turn that rule off and count pair evidence instead. */
static const SelectionCase cases[] = {
    {"default", 1, 1, 1, 0, 0, 3U, NULL},
    {"generated", 1, 1, 1, 1, 0, 5U, NULL},
    {"c-only", 1, 0, 0, 0, 0, 1U, NULL},
    {"cpp-only", 0, 1, 0, 0, 0, 1U, NULL},
    {"headers-only", 0, 0, 1, 0, 0, 1U, NULL},
    {"none", 0, 0, 0, 0, 0, 0U, NULL},
    {"generated-none", 0, 0, 0, 1, 0, 0U, NULL},
    {"nonzero", -1, -1, -1, -1, 0, 5U, NULL},
    {"direct-c", 1, 0, 0, 0, 0, 1U, "main.c"},
    {"direct-disabled-c", 0, 1, 1, 0, 0, 0U, "missing.c"},
    {"direct-disabled-cpp", 1, 0, 1, 0, 0, 0U, "missing.cpp"},
    {"direct-disabled-header", 1, 1, 0, 0, 0, 0U, "missing.h"},
    {"direct-generated", 1, 0, 0, 0, 0, 1U, "build/generated.c"},
    {"duplicates-default", 1, 1, 1, 0, 1, 3U, NULL},
    {"duplicates-without-c", 0, 1, 1, 0, 1, 1U, NULL},
    {"duplicates-none", 0, 0, 0, 1, 1, 0U, NULL},
    {"duplicates-generated", 1, 1, 1, 1, 1, 10U, NULL}
};
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const SelectionCase *selected = NULL;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(argv[1], cases[i].name)) selected = &cases[i];
    if (selected == NULL) return 2;
    UmiCodeGuardConfig config = umi_codeguard_config_default(argv[2]);
    config.profile.scan_c = selected->c;
    config.profile.scan_cpp = selected->cpp;
    config.profile.scan_headers = selected->headers;
    config.profile.scan_generated = selected->generated;
    config.profile.scan_duplicates = selected->duplicates;
    config.profile.scan_source_names = 0;
    config.profile.scan_architecture = 0;
    config.profile.max_line_length = selected->duplicates ? 240U : 1U;
    UmiCodeGuardResult *result = NULL;
    CHECK(umi_codeguard_result_create(16U, &result) == UMI_STATUS_OK);
    UmiStatus status;
    if (selected->direct != NULL) {
        char path[UMI_PATH_CAPACITY];
        CHECK(umi_path_join(argv[2], selected->direct, path, sizeof path) == UMI_STATUS_OK);
        status = umi_codeguard_scan_file(&config, path, result);
    } else status = umi_codeguard_scan(&config, result);
    CHECK(status == UMI_STATUS_OK);
    CHECK(umi_codeguard_result_count(result) == selected->expected);
    for (size_t i = 0U; i < selected->expected; ++i) {
        const UmiCodeGuardFinding *finding = umi_codeguard_result_at(result, i);
        CHECK(finding != NULL);
        CHECK(!strcmp(finding->rule_id, selected->duplicates ?
            "CODEGUARD-DUP-001" : "CODEGUARD-QUALITY-LINE-001"));
        if (!selected->c)
            CHECK(!umi_codeguard_source_supported(finding->path, 0, 0));
    }
    umi_codeguard_result_destroy(result);
    return 0;
}
