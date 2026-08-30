/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_install_probe.c
 *
 * PURPOSE:
 *   Verify the install probe contract and revision behaviour.
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
#include "umicom/sdk_runtime/install_probe.h"
int main(void)
{
    UmiSdkRuntimeInstallProbe value; UmiSdkRuntimeInstallProbe same; uint64_t revision;
    umi_sdk_runtime_install_probe_init(&value, "sdk-runtime.install_probe");
    assert(umi_sdk_runtime_install_probe_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_install_probe_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_install_probe_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_install_probe_set_checked_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_install_probe_set_missing_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_install_probe_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.checked_count == 3U && value.missing_count == 5U);
    umi_sdk_runtime_install_probe_init(&same, "sdk-runtime.install_probe");
    assert(umi_sdk_runtime_install_probe_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
