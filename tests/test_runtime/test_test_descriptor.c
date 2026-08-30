/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_test_descriptor.c
 *
 * PURPOSE:
 *   Verify the test descriptor contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/test_descriptor.h"

int main(void)
{
    UmiTestRuntimeTestDescriptor value;
    UmiTestRuntimeTestDescriptor same;
    uint64_t revision;
    umi_test_runtime_test_descriptor_init(&value, "test-runtime.test_descriptor");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_test_descriptor_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_test_descriptor_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_test_descriptor_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_test_descriptor_set_timeout_seconds(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_test_descriptor_set_processor_cost(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_test_descriptor_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.timeout_seconds == 7U);
    assert(value.processor_cost == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_test_descriptor_init(&same, "test-runtime.test_descriptor");
    assert(umi_test_runtime_test_descriptor_same_identity(&value, &same));
    assert(umi_test_runtime_test_descriptor_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
