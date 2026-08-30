/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_package_validator.c
 *
 * PURPOSE:
 *   Verify the package validator contract and revision behaviour.
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
#include "umicom/sdk_runtime/package_validator.h"
int main(void)
{
    UmiSdkRuntimePackageValidator value; UmiSdkRuntimePackageValidator same; uint64_t revision;
    umi_sdk_runtime_package_validator_init(&value, "sdk-runtime.package_validator");
    assert(umi_sdk_runtime_package_validator_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_package_validator_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_validator_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_validator_set_checked_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_validator_set_failure_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_validator_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.checked_count == 3U && value.failure_count == 5U);
    umi_sdk_runtime_package_validator_init(&same, "sdk-runtime.package_validator");
    assert(umi_sdk_runtime_package_validator_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
