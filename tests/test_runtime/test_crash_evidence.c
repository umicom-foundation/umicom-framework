/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_crash_evidence.c
 *
 * PURPOSE:
 *   Verify the crash evidence contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/crash_evidence.h"

int main(void)
{
    UmiTestRuntimeCrashEvidence value;
    UmiTestRuntimeCrashEvidence same;
    uint64_t revision;
    umi_test_runtime_crash_evidence_init(&value, "test-runtime.crash_evidence");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_crash_evidence_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_crash_evidence_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_crash_evidence_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_crash_evidence_set_native_status(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_crash_evidence_set_thread_id(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_crash_evidence_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.native_status == 7U);
    assert(value.thread_id == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_crash_evidence_init(&same, "test-runtime.crash_evidence");
    assert(umi_test_runtime_crash_evidence_same_identity(&value, &same));
    assert(umi_test_runtime_crash_evidence_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
