/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_runtime_policy.c
 *
 * PURPOSE:
 *   Verify the runtime policy contract and revision behaviour.
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
#include "umicom/sdk_runtime/runtime_policy.h"
int main(void)
{
    UmiSdkRuntimeRuntimePolicy value; UmiSdkRuntimeRuntimePolicy same; uint64_t revision;
    umi_sdk_runtime_runtime_policy_init(&value, "sdk-runtime.runtime_policy");
    assert(umi_sdk_runtime_runtime_policy_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_runtime_policy_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_policy_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_policy_set_allow_source(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_policy_set_require_installed(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_policy_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.allow_source == 3U && value.require_installed == 5U);
    umi_sdk_runtime_runtime_policy_init(&same, "sdk-runtime.runtime_policy");
    assert(umi_sdk_runtime_runtime_policy_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
