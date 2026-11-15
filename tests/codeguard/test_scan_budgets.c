/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_scan_budgets.c
 *
 * PURPOSE:
 *   Verify that every scan phase enforces its own byte budget before publishing candidates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/scanner.h"
#include "umicom/codeguard/duplicate.h"
#include "../../src/codeguard/source_reader.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static unsigned opens, grow_at;
static uint64_t limits[5];
static int empty_input, binary_input;
static UmiStatus NativeBudgetInput(const char *path, uint64_t maximum,
    UmiInputChunkVisitor visitor, void *context, uint64_t *out_size)
{
    (void)path;
    ++opens;
    if (opens <= 5U) limits[opens - 1U] = maximum;
    *out_size = 0U;
    const unsigned char *data = (const unsigned char *)"x\n";
    size_t size = empty_input ? 0U : 2U;
    if (grow_at == opens) { data = (const unsigned char *)"xxxx\n"; size = 5U; }
    if (binary_input) { data = (const unsigned char *)"x\0"; size = 2U; }
    if (size > maximum) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = size ? visitor(data, size, context) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK) *out_size = size;
    return status;
}

/* Use real scan phases and ownership, replacing only the native byte source.
 * Each call models a fresh open. Growth therefore tests a later pass's actual
 * admission boundary, not a cached directory size or preflight observation. */
#define UmiInputFileVisit NativeBudgetInput
#include "../../src/codeguard/source_reader.c"
#undef UmiInputFileVisit
#include "../../src/codeguard/source.c"
#include "../../src/codeguard/normalizer.c"
#include "../../src/codeguard/duplicate.c"
#include "../../src/codeguard/architecture.c"
#include "../../src/codeguard/scanner.c"

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *name = argv[1];
    static const char *cases[] = {"all-passes", "grow-scan", "grow-measure",
        "grow-header", "grow-studio", "grow-hash", "zero-empty", "zero-nonempty",
        "disabled-language", "no-architecture", "no-duplicates", "binary-text",
        "cancel-before", "direct-architecture", "direct-duplicate"};
    size_t selected = 0U;
    while (selected < sizeof cases / sizeof cases[0] && strcmp(cases[selected], name)) ++selected;
    if (selected == sizeof cases / sizeof cases[0]) return 2;
    UmiCodeGuardConfig config = umi_codeguard_config_default(argv[2]);
    config.profile.max_file_bytes = 2U;
    config.profile.max_include_count = SIZE_MAX;
    config.profile.max_source_lines = SIZE_MAX;
    config.profile.scan_source_names = 0;
    config.profile.scan_architecture = 1;
    config.profile.scan_duplicates = 1;
    UmiFileInfo file = {.kind = UMI_FILE_KIND_REGULAR};
    int length = snprintf(file.path, sizeof file.path,
        "%s/framework/include/applications/studio/sample.c", argv[2]);
    CHECK(length > 0 && (size_t)length < sizeof file.path);
    UmiCancellationToken *token = NULL;
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    UmiCodeGuardResult *result = NULL;
    CHECK(umi_codeguard_result_create(8U, &result) == UMI_STATUS_OK);
    UmiCodeGuardDuplicateSet *duplicates = NULL;
    CHECK(umi_codeguard_duplicate_set_create(&duplicates) == UMI_STATUS_OK);
    if (!strncmp(name, "grow-", 5U)) grow_at = (unsigned)selected;
    if (!strncmp(name, "zero-", 5U)) {
        config.profile.max_file_bytes = 0U;
        empty_input = !strcmp(name, "zero-empty");
    }
    if (!strcmp(name, "disabled-language")) config.profile.scan_c = 0;
    if (!strcmp(name, "no-architecture")) config.profile.scan_architecture = 0;
    if (!strcmp(name, "no-duplicates")) config.profile.scan_duplicates = 0;
    if (!strcmp(name, "binary-text")) binary_input = 1;
    if (!strcmp(name, "cancel-before")) umi_cancellation_token_request(token);
    ScanState state = {.config = &config, .duplicates = duplicates,
        .result = result, .cancellation = token, .status = UMI_STATUS_OK};
    UmiStatus status;
    if (!strcmp(name, "direct-architecture")) {
        grow_at = 1U;
        status = umi_codeguard_architecture_scan_file(NULL, file.path, &config.profile, result);
    } else if (!strcmp(name, "direct-duplicate")) {
        grow_at = 1U;
        status = umi_codeguard_duplicate_set_add_bounded(duplicates, file.path, 2U, token);
    } else status = umi_codeguard_scanner_visit(&file, &state);

    unsigned expected_opens = 5U;
    if (grow_at) {
        expected_opens = grow_at;
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (!strcmp(name, "zero-nonempty")) {
        expected_opens = 1U;
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (!strcmp(name, "binary-text")) {
        expected_opens = 1U;
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(name, "cancel-before")) {
        expected_opens = 0U;
        CHECK(status == UMI_STATUS_CANCELLED);
    } else {
        CHECK(status == UMI_STATUS_OK);
        if (!strcmp(name, "disabled-language")) expected_opens = 0U;
        if (!strcmp(name, "no-architecture")) expected_opens = 2U;
        if (!strcmp(name, "no-duplicates")) expected_opens = 4U;
    }
    CHECK(opens == expected_opens);
    for (unsigned i = 0U; i < opens; ++i) CHECK(limits[i] == config.profile.max_file_bytes);
    CHECK(umi_codeguard_result_count(result) == 0U);
    size_t expected_candidates = status == UMI_STATUS_OK &&
        config.profile.scan_c && config.profile.scan_duplicates ? 1U : 0U;
    CHECK(umi_codeguard_duplicate_set_count(duplicates) == expected_candidates);
    umi_codeguard_duplicate_set_destroy(duplicates);
    umi_codeguard_result_destroy(result);
    umi_cancellation_token_destroy(token);
    return 0;
}
