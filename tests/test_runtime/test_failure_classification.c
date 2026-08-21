/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_failure_classification.c
 *
 * PURPOSE:
 *   Verify the failure classification contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/failure_classification.h"

int main(void)
{
    UmiTestRuntimeFailureClassification value;
    UmiTestRuntimeFailureClassification same;
    uint64_t revision;
    umi_test_runtime_failure_classification_init(&value, "test-runtime.failure_classification");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_failure_classification_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_failure_classification_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_failure_classification_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_failure_classification_set_native_status(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_failure_classification_set_exit_code(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_failure_classification_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.native_status == 7U);
    assert(value.exit_code == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_failure_classification_init(&same, "test-runtime.failure_classification");
    assert(umi_test_runtime_failure_classification_same_identity(&value, &same));
    assert(umi_test_runtime_failure_classification_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
