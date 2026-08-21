/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_package_channel.c
 *
 * PURPOSE:
 *   Verify the package channel SDK/runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/package_channel.h"
int main(void)
{
    UmiSdkRuntimePackageChannel value,same;
    umi_sdk_runtime_package_channel_init(&value,"sdk-runtime.package_channel");
    assert(umi_sdk_runtime_package_channel_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_channel_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_channel_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_channel_set_priority(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_channel_set_generation(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_channel_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_package_channel_init(&same,"sdk-runtime.package_channel");
    assert(umi_sdk_runtime_package_channel_same_identity(&value,&same));
    return 0;
    }
