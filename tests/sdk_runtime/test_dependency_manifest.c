/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_dependency_manifest.c
 *
 * PURPOSE:
 *   Verify the dependency manifest contract and revision behaviour.
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
#include "umicom/sdk_runtime/dependency_manifest.h"
int main(void)
{
    UmiSdkRuntimeDependencyManifest value; UmiSdkRuntimeDependencyManifest same; uint64_t revision;
    umi_sdk_runtime_dependency_manifest_init(&value, "sdk-runtime.dependency_manifest");
    assert(umi_sdk_runtime_dependency_manifest_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_dependency_manifest_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_manifest_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_manifest_set_dependency_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_manifest_set_missing_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_dependency_manifest_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.dependency_count == 3U && value.missing_count == 5U);
    umi_sdk_runtime_dependency_manifest_init(&same, "sdk-runtime.dependency_manifest");
    assert(umi_sdk_runtime_dependency_manifest_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
