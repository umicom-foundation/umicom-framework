/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_pattern_failures.c
 *
 * PURPOSE:
 *   Verify pattern matching preserves actual insertion failures and earlier findings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/pattern_scan.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static UmiStatus failure = UMI_STATUS_OK;
static size_t calls, fail_at;
static UmiStatus InsertFinding(UmiCodeGuardResult *result, const UmiCodeGuardFinding *finding)
{
    ++calls;
    if (calls == fail_at) return failure;
    return umi_codeguard_result_add(result, finding);
}
/* Only finding insertion is controlled. Registry lookup, match modes,
 * diagnostic coordinates and successful storage remain production code. */
#define umi_codeguard_result_add InsertFinding
#include "../../src/codeguard/pattern_scan.c"
#undef umi_codeguard_result_add

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    UmiCodeGuardRuleRegistry *registry = NULL;
    UmiCodeGuardResult *result = NULL;
    CHECK(umi_codeguard_rule_registry_create(&registry) == UMI_STATUS_OK);
    CHECK(umi_codeguard_result_create(4U, &result) == UMI_STATUS_OK);
    UmiCodeGuardRule rules[] = {
        {"TEST-RAW", "", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_MEMORY, 100U, UMI_CODEGUARD_MATCH_RAW, "token", "Raw match", "Review raw text"},
        {"TEST-CODE", "", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_MEMORY, 100U, UMI_CODEGUARD_MATCH_CODE, "token", "Code match", "Review code"},
        {"TEST-INCLUDE", "", UMI_CODEGUARD_MEDIUM, UMI_CODEGUARD_CATEGORY_MEMORY, 100U, UMI_CODEGUARD_MATCH_INCLUDE, "token", "Include match", "Review dependency"}
    };
    CHECK(umi_codeguard_rule_registry_add(registry, rules, 3U) == UMI_STATUS_OK);
    const char *raw = "  token", *code = "token";
    size_t expected_count = 2U, expected_calls = 2U;
    UmiStatus expected_status = UMI_STATUS_OK;
    if (!strcmp(name, "arguments")) {
        CHECK(umi_codeguard_pattern_scan_line(NULL, "x.c", 9U, raw, code, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_pattern_scan_line(registry, NULL, 9U, raw, code, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_pattern_scan_line(registry, "x.c", 9U, NULL, code, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_pattern_scan_line(registry, "x.c", 9U, raw, NULL, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_pattern_scan_line(registry, "x.c", 9U, raw, code, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls == 0U);
        umi_codeguard_rule_registry_destroy(registry);
        umi_codeguard_result_destroy(result);
        return 0;
    } else if (!strcmp(name, "raw-only")) {
        code = ""; expected_count = expected_calls = 1U;
    } else if (!strcmp(name, "code-only")) {
        raw = ""; expected_count = expected_calls = 1U;
    } else if (!strcmp(name, "include")) {
        code = "#include <token>"; expected_count = expected_calls = 3U;
    } else if (!strcmp(name, "no-match")) {
        raw = code = ""; expected_count = expected_calls = 0U;
    } else if (!strcmp(name, "allocation")) {
        failure = UMI_STATUS_OUT_OF_MEMORY; fail_at = 1U;
    } else if (!strcmp(name, "capacity")) {
        failure = UMI_STATUS_CAPACITY_EXCEEDED; fail_at = 1U;
    } else if (!strcmp(name, "io-status")) {
        failure = UMI_STATUS_IO_ERROR; fail_at = 1U;
    } else if (!strcmp(name, "second-failure")) {
        failure = UMI_STATUS_CAPACITY_EXCEEDED; fail_at = 2U;
    } else if (strcmp(name, "ordinary")) return 2;
    if (fail_at != 0U) {
        expected_status = failure;
        expected_count = fail_at - 1U;
        expected_calls = fail_at;
    }
    CHECK(umi_codeguard_pattern_scan_line(registry, "x.c", 9U, raw, code, result) == expected_status);
    CHECK(calls == expected_calls);
    CHECK(umi_codeguard_result_count(result) == expected_count);
    for (size_t i = 0U; i < expected_count; ++i) {
        const UmiCodeGuardFinding *finding = umi_codeguard_result_at(result, i);
        CHECK(finding != NULL && finding->line == 9U && !strcmp(finding->path, "x.c"));
        const char *expected_id = !strcmp(name, "code-only") ? "TEST-CODE" : rules[i].rule_id;
        CHECK(!strcmp(finding->rule_id, expected_id));
        size_t expected_column = !strcmp(expected_id, "TEST-RAW") ? 3U :
            (!strcmp(name, "include") ? 11U : 1U);
        CHECK(finding->column == expected_column);
    }
    umi_codeguard_rule_registry_destroy(registry);
    umi_codeguard_result_destroy(result);
    return 0;
}
