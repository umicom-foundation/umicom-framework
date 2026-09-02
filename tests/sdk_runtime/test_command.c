/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_command.c
 *
 * PURPOSE:
 *   Verify the command contract and revision behaviour.
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
#include "umicom/sdk_runtime/command.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeCommand value; UmiSdkRuntimeCommand same; uint64_t revision;
    umi_sdk_runtime_command_init(&value, "sdk-runtime.command");
    assert(umi_sdk_runtime_command_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_command_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_command_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_command_set_kind(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_command_set_sequence(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_command_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.kind == 3U && value.sequence == 5U);
    umi_sdk_runtime_command_init(&same, "sdk-runtime.command");
    assert(umi_sdk_runtime_command_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
