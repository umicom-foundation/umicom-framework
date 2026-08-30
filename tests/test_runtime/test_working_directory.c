/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_working_directory.c
 *
 * PURPOSE:
 *   Verify the working directory contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/working_directory.h"

int main(void)
{
    UmiTestRuntimeWorkingDirectory value;
    UmiTestRuntimeWorkingDirectory same;
    uint64_t revision;
    umi_test_runtime_working_directory_init(&value, "test-runtime.working_directory");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_working_directory_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_working_directory_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_working_directory_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_working_directory_set_exists(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_working_directory_set_generation(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_working_directory_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.exists == 7U);
    assert(value.generation == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_working_directory_init(&same, "test-runtime.working_directory");
    assert(umi_test_runtime_working_directory_same_identity(&value, &same));
    assert(umi_test_runtime_working_directory_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
