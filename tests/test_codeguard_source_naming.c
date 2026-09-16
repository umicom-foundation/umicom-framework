/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_source_naming.c
 *
 * PURPOSE:
 *   Verify that CodeGuard rejects version- and batch-labelled filenames while
 *   accepting descriptive names and ordinary architecture identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Keep every contract check and its setup active in optimised builds. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/codeguard/source_naming.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCodeGuardResult *result = NULL;
    const UmiCodeGuardFinding *finding;

    assert(umi_codeguard_source_name_classify("src/workspace_layout.c") ==
           UMI_CODEGUARD_SOURCE_NAME_OK);
    assert(umi_codeguard_source_name_classify("src/riscv64_lowering.c") ==
           UMI_CODEGUARD_SOURCE_NAME_OK);
    assert(umi_codeguard_source_name_classify("tests/test_feature_01.c") ==
           UMI_CODEGUARD_SOURCE_NAME_OK);
    assert(umi_codeguard_source_name_classify("src/workspace_layout_v3.c") ==
           UMI_CODEGUARD_SOURCE_NAME_VERSION_LABEL);
    assert(umi_codeguard_source_name_classify("docs/guide_0.5.0.md") ==
           UMI_CODEGUARD_SOURCE_NAME_VERSION_LABEL);
    assert(umi_codeguard_source_name_classify("docs/BATCH_24_GUIDE.md") ==
           UMI_CODEGUARD_SOURCE_NAME_BATCH_LABEL);
    assert(umi_codeguard_source_name_classify("docs/mb60_notes.md") ==
           UMI_CODEGUARD_SOURCE_NAME_BATCH_LABEL);

    assert(umi_codeguard_result_create(4U, &result) == UMI_STATUS_OK);
    assert(umi_codeguard_source_name_audit("src/workspace_layout.c", result) ==
           UMI_STATUS_OK);
    assert(umi_codeguard_result_count(result) == 0U);

    assert(umi_codeguard_source_name_audit("src/workspace_layout_v3.c",
                                           result) == UMI_STATUS_OK);
    finding = umi_codeguard_result_at(result, 0U);
    assert(finding != NULL);
    assert(strcmp(finding->rule_id, "CODEGUARD-NAME-VERSION-001") == 0);

    assert(umi_codeguard_source_name_audit("docs/BATCH_24_GUIDE.md", result) ==
           UMI_STATUS_OK);
    finding = umi_codeguard_result_at(result, 1U);
    assert(finding != NULL);
    assert(strcmp(finding->rule_id, "CODEGUARD-NAME-BATCH-001") == 0);

    /* Product identities and domain capabilities are not naming defects. */
    const char *const acceptedNames[] = {
        "src/application/experiences/studio.c", "src/trading/instrument.c",
        "tests/input_validation/test_input_validation.c",
        "tests/studio_integration.c", "adapters/ibkr/orders.c",
        "docs/learning/git-first-commit.html", "docs/review(copy).md",
        "src/null.c", "src/com10.c", "src/lpt0.c", "src/concurrent.c",
        "src/auxiliary.c", "C:\\umicom\\src\\editor.c", "third_party/v2/source.c",
        "tests/studio_trader_contracts_extra/test.c", "src/m.c", "src/v.c"
    };
    for (size_t index = 0U; index < sizeof(acceptedNames) / sizeof(acceptedNames[0]); ++index) {
        assert(umi_codeguard_source_name_classify(acceptedNames[index]) == UMI_CODEGUARD_SOURCE_NAME_OK);
    }
    struct NamingCase {
        const char *path;
        UmiCodeGuardSourceNameIssue issue;
        const char *rule;
    };
    const struct NamingCase namingCases[] = {
        {"tests/studio_trader_contracts/CMakeLists.txt", UMI_CODEGUARD_SOURCE_NAME_CONSUMER_LABEL, "CODEGUARD-NAME-CONSUMER-001"},
        {"framework\\tests\\STUDIO_TRADER_CONTRACTS\\test.c", UMI_CODEGUARD_SOURCE_NAME_CONSUMER_LABEL, "CODEGUARD-NAME-CONSUMER-001"},
        {"studio_trader_contracts/", UMI_CODEGUARD_SOURCE_NAME_CONSUMER_LABEL, "CODEGUARD-NAME-CONSUMER-001"},
        {"docs/guide(1).html", UMI_CODEGUARD_SOURCE_NAME_COPY_LABEL, "CODEGUARD-NAME-COPY-001"},
        {"docs/guide (12).md", UMI_CODEGUARD_SOURCE_NAME_COPY_LABEL, "CODEGUARD-NAME-COPY-001"},
        {"docs/guide(12)", UMI_CODEGUARD_SOURCE_NAME_COPY_LABEL, "CODEGUARD-NAME-COPY-001"},
        {"src/CON.c", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"src/NUL", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"docs/Aux.md", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"src/com1.c", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"src/LPT9.c", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"src/file.c ", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"src/file.", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"src/file?.c", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"src/file:c", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"src/file\nc", UMI_CODEGUARD_SOURCE_NAME_NON_PORTABLE, "CODEGUARD-NAME-PORTABILITY-001"},
        {"docs/guide_v2.md", UMI_CODEGUARD_SOURCE_NAME_VERSION_LABEL, "CODEGUARD-NAME-VERSION-001"},
        {"docs/batch24_notes.md", UMI_CODEGUARD_SOURCE_NAME_BATCH_LABEL, "CODEGUARD-NAME-BATCH-001"}
    };
    for (size_t index = 0U; index < sizeof(namingCases) / sizeof(namingCases[0]); ++index) {
        umi_codeguard_result_clear(result);
        assert(umi_codeguard_source_name_classify(namingCases[index].path) == namingCases[index].issue);
        assert(umi_codeguard_source_name_audit(namingCases[index].path, result) == UMI_STATUS_OK);
        assert(umi_codeguard_result_count(result) == 1U);
        finding = umi_codeguard_result_at(result, 0U);
        assert(finding != NULL);
        assert(strcmp(finding->path, namingCases[index].path) == 0);
        assert(strcmp(finding->rule_id, namingCases[index].rule) == 0);
        assert(finding->remediation[0] != '\0');
    }
    umi_codeguard_result_clear(result);
    assert(umi_codeguard_source_name_audit(NULL, result) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_codeguard_source_name_audit("", result) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_codeguard_source_name_audit("file.c", NULL) == UMI_STATUS_INVALID_ARGUMENT);
    char longPath[UMI_CODEGUARD_PATH_CAPACITY + 32U];
    memset(longPath, 'a', sizeof(longPath));
    memcpy(longPath + sizeof(longPath) - sizeof("_v2.c"), "_v2.c", sizeof("_v2.c"));
    assert(umi_codeguard_source_name_audit(longPath, result) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_codeguard_result_count(result) == 0U);
    assert(umi_codeguard_source_name_classify(NULL) == UMI_CODEGUARD_SOURCE_NAME_OK);
    assert(umi_codeguard_source_name_classify("") == UMI_CODEGUARD_SOURCE_NAME_OK);
    (void)puts("CodeGuard naming: legacy rules, capability scope, copies, portability and diagnostic boundaries passed.");

    umi_codeguard_result_destroy(result);
    return 0;
}
