/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_event.c
 *
 * PURPOSE:
 *   Verify the event contract and revision behaviour.
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
#include "umicom/sdk_runtime/event.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeEvent value; UmiSdkRuntimeEvent same; uint64_t revision;
    umi_sdk_runtime_event_init(&value, "sdk-runtime.event");
    assert(umi_sdk_runtime_event_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_event_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_event_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_event_set_kind(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_event_set_sequence(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_event_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.kind == 3U && value.sequence == 5U);
    umi_sdk_runtime_event_init(&same, "sdk-runtime.event");
    assert(umi_sdk_runtime_event_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
