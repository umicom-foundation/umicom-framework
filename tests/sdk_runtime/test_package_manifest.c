/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_package_manifest.c
 *
 * PURPOSE:
 *   Verify the package manifest contract and revision behaviour.
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
#include "umicom/sdk_runtime/package_manifest.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimePackageManifest value; UmiSdkRuntimePackageManifest same; uint64_t revision;
    umi_sdk_runtime_package_manifest_init(&value, "sdk-runtime.package_manifest");
    assert(umi_sdk_runtime_package_manifest_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_package_manifest_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_manifest_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_manifest_set_component_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_manifest_set_dependency_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_manifest_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.component_count == 3U && value.dependency_count == 5U);
    umi_sdk_runtime_package_manifest_init(&same, "sdk-runtime.package_manifest");
    assert(umi_sdk_runtime_package_manifest_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
