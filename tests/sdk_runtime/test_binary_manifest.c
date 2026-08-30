/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_binary_manifest.c
 *
 * PURPOSE:
 *   Verify the binary manifest contract and revision behaviour.
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
#include "umicom/sdk_runtime/binary_manifest.h"
int main(void)
{
    UmiSdkRuntimeBinaryManifest value; UmiSdkRuntimeBinaryManifest same; uint64_t revision;
    umi_sdk_runtime_binary_manifest_init(&value, "sdk-runtime.binary_manifest");
    assert(umi_sdk_runtime_binary_manifest_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_binary_manifest_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_binary_manifest_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_binary_manifest_set_file_size(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_binary_manifest_set_checksum(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_binary_manifest_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.file_size == 3U && value.checksum == 5U);
    umi_sdk_runtime_binary_manifest_init(&same, "sdk-runtime.binary_manifest");
    assert(umi_sdk_runtime_binary_manifest_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
