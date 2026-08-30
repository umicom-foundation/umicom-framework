/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_dependency_probe.c
 *
 * PURPOSE:
 *   Verify the dependency probe contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/dependency_probe.h"

int main(void)
{
    UmiTestRuntimeDependencyProbe value;
    UmiTestRuntimeDependencyProbe same;
    uint64_t revision;
    umi_test_runtime_dependency_probe_init(&value, "test-runtime.dependency_probe");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_dependency_probe_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_dependency_probe_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_probe_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_probe_set_dependency_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_probe_set_missing_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_probe_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.dependency_count == 7U);
    assert(value.missing_count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_dependency_probe_init(&same, "test-runtime.dependency_probe");
    assert(umi_test_runtime_dependency_probe_same_identity(&value, &same));
    assert(umi_test_runtime_dependency_probe_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
