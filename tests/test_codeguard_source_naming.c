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

#include <assert.h>
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

    umi_codeguard_result_destroy(result);
    return 0;
}
