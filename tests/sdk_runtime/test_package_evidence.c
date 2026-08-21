/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_package_evidence.c
 *
 * PURPOSE:
 *   Verify the package evidence contract and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/package_evidence.h"
int main(void)
{
    UmiSdkRuntimePackageEvidence value; UmiSdkRuntimePackageEvidence same; uint64_t revision;
    umi_sdk_runtime_package_evidence_init(&value, "sdk-runtime.package_evidence");
    assert(umi_sdk_runtime_package_evidence_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_package_evidence_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_evidence_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_evidence_set_passed_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_evidence_set_failed_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_evidence_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.passed_count == 3U && value.failed_count == 5U);
    umi_sdk_runtime_package_evidence_init(&same, "sdk-runtime.package_evidence");
    assert(umi_sdk_runtime_package_evidence_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
