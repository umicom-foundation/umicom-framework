/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_timeout_evidence.c
 *
 * PURPOSE:
 *   Verify the timeout evidence contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/timeout_evidence.h"

int main(void)
{
    UmiTestRuntimeTimeoutEvidence value;
    UmiTestRuntimeTimeoutEvidence same;
    uint64_t revision;
    umi_test_runtime_timeout_evidence_init(&value, "test-runtime.timeout_evidence");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_timeout_evidence_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_timeout_evidence_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_timeout_evidence_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_timeout_evidence_set_elapsed_ms(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_timeout_evidence_set_budget_ms(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_timeout_evidence_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.elapsed_ms == 7U);
    assert(value.budget_ms == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_timeout_evidence_init(&same, "test-runtime.timeout_evidence");
    assert(umi_test_runtime_timeout_evidence_same_identity(&value, &same));
    assert(umi_test_runtime_timeout_evidence_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
