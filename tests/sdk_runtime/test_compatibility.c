/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_compatibility.c
 *
 * PURPOSE:
 *   Verify the compatibility contract and revision behaviour.
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
#include "umicom/sdk_runtime/compatibility.h"
int main(void)
{
    UmiSdkRuntimeCompatibility value; UmiSdkRuntimeCompatibility same; uint64_t revision;
    umi_sdk_runtime_compatibility_init(&value, "sdk-runtime.compatibility");
    assert(umi_sdk_runtime_compatibility_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_compatibility_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_compatibility_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_compatibility_set_compatible(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_compatibility_set_reason_code(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_compatibility_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.compatible == 3U && value.reason_code == 5U);
    umi_sdk_runtime_compatibility_init(&same, "sdk-runtime.compatibility");
    assert(umi_sdk_runtime_compatibility_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
