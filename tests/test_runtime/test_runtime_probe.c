/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_runtime_probe.c
 *
 * PURPOSE:
 *   Verify the runtime probe contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/runtime_probe.h"

int main(void)
{
    UmiTestRuntimeRuntimeProbe value;
    UmiTestRuntimeRuntimeProbe same;
    uint64_t revision;
    umi_test_runtime_runtime_probe_init(&value, "test-runtime.runtime_probe");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_runtime_probe_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_runtime_probe_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_probe_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_probe_set_probe_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_probe_set_failure_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_probe_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.probe_count == 7U);
    assert(value.failure_count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_runtime_probe_init(&same, "test-runtime.runtime_probe");
    assert(umi_test_runtime_runtime_probe_same_identity(&value, &same));
    assert(umi_test_runtime_runtime_probe_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
