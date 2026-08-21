/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_bad_command_evidence.c
 *
 * PURPOSE:
 *   Verify the bad command evidence contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/bad_command_evidence.h"

int main(void)
{
    UmiTestRuntimeBadCommandEvidence value;
    UmiTestRuntimeBadCommandEvidence same;
    uint64_t revision;
    umi_test_runtime_bad_command_evidence_init(&value, "test-runtime.bad_command_evidence");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_bad_command_evidence_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_bad_command_evidence_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_bad_command_evidence_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_bad_command_evidence_set_candidate_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_bad_command_evidence_set_missing_dependency_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_bad_command_evidence_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.candidate_count == 7U);
    assert(value.missing_dependency_count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_bad_command_evidence_init(&same, "test-runtime.bad_command_evidence");
    assert(umi_test_runtime_bad_command_evidence_same_identity(&value, &same));
    assert(umi_test_runtime_bad_command_evidence_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
