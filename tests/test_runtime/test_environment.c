/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_environment.c
 *
 * PURPOSE:
 *   Verify the environment contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/environment.h"

int main(void)
{
    UmiTestRuntimeEnvironment value;
    UmiTestRuntimeEnvironment same;
    uint64_t revision;
    umi_test_runtime_environment_init(&value, "test-runtime.environment");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_environment_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_environment_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_environment_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_environment_set_entry_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_environment_set_generation(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_environment_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.entry_count == 7U);
    assert(value.generation == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_environment_init(&same, "test-runtime.environment");
    assert(umi_test_runtime_environment_same_identity(&value, &same));
    assert(umi_test_runtime_environment_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
