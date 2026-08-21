/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_clean_machine.c
 *
 * PURPOSE:
 *   Verify the clean machine contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/clean_machine.h"

int main(void)
{
    UmiTestRuntimeCleanMachine value;
    UmiTestRuntimeCleanMachine same;
    uint64_t revision;
    umi_test_runtime_clean_machine_init(&value, "test-runtime.clean_machine");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_clean_machine_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_clean_machine_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_clean_machine_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_clean_machine_set_requirement_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_clean_machine_set_missing_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_clean_machine_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.requirement_count == 7U);
    assert(value.missing_count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_clean_machine_init(&same, "test-runtime.clean_machine");
    assert(umi_test_runtime_clean_machine_same_identity(&value, &same));
    assert(umi_test_runtime_clean_machine_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
