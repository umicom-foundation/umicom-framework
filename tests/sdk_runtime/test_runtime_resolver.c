/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_runtime_resolver.c
 *
 * PURPOSE:
 *   Verify the runtime resolver contract and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/runtime_resolver.h"
int main(void)
{
    UmiSdkRuntimeRuntimeResolver value; UmiSdkRuntimeRuntimeResolver same; uint64_t revision;
    umi_sdk_runtime_runtime_resolver_init(&value, "sdk-runtime.runtime_resolver");
    assert(umi_sdk_runtime_runtime_resolver_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_runtime_resolver_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_resolver_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_resolver_set_resolved_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_resolver_set_missing_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_resolver_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.resolved_count == 3U && value.missing_count == 5U);
    umi_sdk_runtime_runtime_resolver_init(&same, "sdk-runtime.runtime_resolver");
    assert(umi_sdk_runtime_runtime_resolver_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
