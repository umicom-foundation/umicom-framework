/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_executable_probe.c
 *
 * PURPOSE:
 *   Verify the executable probe contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/executable_probe.h"

int main(void)
{
    UmiTestRuntimeExecutableProbe value;
    UmiTestRuntimeExecutableProbe same;
    uint64_t revision;
    umi_test_runtime_executable_probe_init(&value, "test-runtime.executable_probe");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_executable_probe_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_executable_probe_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_executable_probe_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_executable_probe_set_exists(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_executable_probe_set_launchable(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_executable_probe_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.exists == 7U);
    assert(value.launchable == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_executable_probe_init(&same, "test-runtime.executable_probe");
    assert(umi_test_runtime_executable_probe_same_identity(&value, &same));
    assert(umi_test_runtime_executable_probe_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
