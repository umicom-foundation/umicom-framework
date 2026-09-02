/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_sdk_profile.c
 *
 * PURPOSE:
 *   Verify the sdk profile contract and revision behaviour.
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
#include "umicom/sdk_runtime/sdk_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeSdkProfile value; UmiSdkRuntimeSdkProfile same; uint64_t revision;
    umi_sdk_runtime_sdk_profile_init(&value, "sdk-runtime.sdk_profile");
    assert(umi_sdk_runtime_sdk_profile_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_sdk_profile_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_sdk_profile_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_sdk_profile_set_required_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_sdk_profile_set_optional_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_sdk_profile_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.required_count == 3U && value.optional_count == 5U);
    umi_sdk_runtime_sdk_profile_init(&same, "sdk-runtime.sdk_profile");
    assert(umi_sdk_runtime_sdk_profile_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
