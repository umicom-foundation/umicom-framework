/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_runtime_dependency.c
 *
 * PURPOSE:
 *   Verify the runtime dependency contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/runtime_dependency.h"

int main(void)
{
    UmiTestRuntimeRuntimeDependency value;
    UmiTestRuntimeRuntimeDependency same;
    uint64_t revision;
    umi_test_runtime_runtime_dependency_init(&value, "test-runtime.runtime_dependency");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_runtime_dependency_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_runtime_dependency_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_dependency_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_dependency_set_required(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_dependency_set_resolved(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_dependency_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.required == 7U);
    assert(value.resolved == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_runtime_dependency_init(&same, "test-runtime.runtime_dependency");
    assert(umi_test_runtime_runtime_dependency_same_identity(&value, &same));
    assert(umi_test_runtime_runtime_dependency_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
