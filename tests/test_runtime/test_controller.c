/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_controller.c
 *
 * PURPOSE:
 *   Verify the controller contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/controller.h"

int main(void)
{
    UmiTestRuntimeController value;
    UmiTestRuntimeController same;
    uint64_t revision;
    umi_test_runtime_controller_init(&value, "test-runtime.controller");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_controller_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_controller_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_controller_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_controller_set_state(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_controller_set_command_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_controller_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.state == 7U);
    assert(value.command_count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_controller_init(&same, "test-runtime.controller");
    assert(umi_test_runtime_controller_same_identity(&value, &same));
    assert(umi_test_runtime_controller_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
