/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_version_contract.c
 *
 * PURPOSE:
 *   Verify the version contract contract and revision behaviour.
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
#include "umicom/sdk_runtime/version_contract.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeVersionContract value; UmiSdkRuntimeVersionContract same; uint64_t revision;
    umi_sdk_runtime_version_contract_init(&value, "sdk-runtime.version_contract");
    assert(umi_sdk_runtime_version_contract_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_version_contract_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_version_contract_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_version_contract_set_major(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_version_contract_set_minor(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_version_contract_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.major == 3U && value.minor == 5U);
    umi_sdk_runtime_version_contract_init(&same, "sdk-runtime.version_contract");
    assert(umi_sdk_runtime_version_contract_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
