/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_retry_policy.c
 *
 * PURPOSE:
 *   Verify the retry policy contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/retry_policy.h"

int main(void)
{
    UmiTestRuntimeRetryPolicy value;
    UmiTestRuntimeRetryPolicy same;
    uint64_t revision;
    umi_test_runtime_retry_policy_init(&value, "test-runtime.retry_policy");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_retry_policy_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_retry_policy_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_retry_policy_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_retry_policy_set_maximum_attempts(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_retry_policy_set_backoff_ms(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_retry_policy_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.maximum_attempts == 7U);
    assert(value.backoff_ms == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_retry_policy_init(&same, "test-runtime.retry_policy");
    assert(umi_test_runtime_retry_policy_same_identity(&value, &same));
    assert(umi_test_runtime_retry_policy_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
