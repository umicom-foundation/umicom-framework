/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_component.c
 *
 * PURPOSE:
 *   Verify the component contract and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/component.h"
int main(void)
{
    UmiSdkRuntimeComponent value; UmiSdkRuntimeComponent same; uint64_t revision;
    umi_sdk_runtime_component_init(&value, "sdk-runtime.component");
    assert(umi_sdk_runtime_component_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_component_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_component_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_component_set_abi_version(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_component_set_package_version(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_component_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.abi_version == 3U && value.package_version == 5U);
    umi_sdk_runtime_component_init(&same, "sdk-runtime.component");
    assert(umi_sdk_runtime_component_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
