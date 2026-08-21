/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_runtime_environment.c
 *
 * PURPOSE:
 *   Verify the runtime environment contract and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/runtime_environment.h"
int main(void)
{
    UmiSdkRuntimeRuntimeEnvironment value; UmiSdkRuntimeRuntimeEnvironment same; uint64_t revision;
    umi_sdk_runtime_runtime_environment_init(&value, "sdk-runtime.runtime_environment");
    assert(umi_sdk_runtime_runtime_environment_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_runtime_environment_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_environment_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_environment_set_entry_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_environment_set_path_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_environment_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.entry_count == 3U && value.path_count == 5U);
    umi_sdk_runtime_runtime_environment_init(&same, "sdk-runtime.runtime_environment");
    assert(umi_sdk_runtime_runtime_environment_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
