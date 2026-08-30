/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_platform_profile.c
 *
 * PURPOSE:
 *   Verify the platform profile contract and revision behaviour.
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
#include "umicom/sdk_runtime/platform_profile.h"
int main(void)
{
    UmiSdkRuntimePlatformProfile value; UmiSdkRuntimePlatformProfile same; uint64_t revision;
    umi_sdk_runtime_platform_profile_init(&value, "sdk-runtime.platform_profile");
    assert(umi_sdk_runtime_platform_profile_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_platform_profile_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_platform_profile_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_platform_profile_set_architecture_bits(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_platform_profile_set_dependency_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_platform_profile_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.architecture_bits == 3U && value.dependency_count == 5U);
    umi_sdk_runtime_platform_profile_init(&same, "sdk-runtime.platform_profile");
    assert(umi_sdk_runtime_platform_profile_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
