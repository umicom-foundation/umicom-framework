/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_timeout_policy.c
 *
 * PURPOSE:
 *   Verify the timeout policy contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/timeout_policy.h"

int main(void)
{
    UmiTestRuntimeTimeoutPolicy value;
    UmiTestRuntimeTimeoutPolicy same;
    uint64_t revision;
    umi_test_runtime_timeout_policy_init(&value, "test-runtime.timeout_policy");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_timeout_policy_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_timeout_policy_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_timeout_policy_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_timeout_policy_set_start_timeout_ms(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_timeout_policy_set_execution_timeout_ms(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_timeout_policy_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.start_timeout_ms == 7U);
    assert(value.execution_timeout_ms == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_timeout_policy_init(&same, "test-runtime.timeout_policy");
    assert(umi_test_runtime_timeout_policy_same_identity(&value, &same));
    assert(umi_test_runtime_timeout_policy_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
