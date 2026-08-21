/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_command.c
 *
 * PURPOSE:
 *   Verify the command contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/command.h"

int main(void)
{
    UmiTestRuntimeCommand value;
    UmiTestRuntimeCommand same;
    uint64_t revision;
    umi_test_runtime_command_init(&value, "test-runtime.command");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_command_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_command_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_command_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_command_set_kind(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_command_set_sequence(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_command_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.kind == 7U);
    assert(value.sequence == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_command_init(&same, "test-runtime.command");
    assert(umi_test_runtime_command_same_identity(&value, &same));
    assert(umi_test_runtime_command_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
