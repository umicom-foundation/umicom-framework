/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_export_target.c
 *
 * PURPOSE:
 *   Verify the export target contract and revision behaviour.
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
#include "umicom/sdk_runtime/export_target.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeExportTarget value; UmiSdkRuntimeExportTarget same; uint64_t revision;
    umi_sdk_runtime_export_target_init(&value, "sdk-runtime.export_target");
    assert(umi_sdk_runtime_export_target_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_export_target_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_export_target_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_export_target_set_dependency_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_export_target_set_generation(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_export_target_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.dependency_count == 3U && value.generation == 5U);
    umi_sdk_runtime_export_target_init(&same, "sdk-runtime.export_target");
    assert(umi_sdk_runtime_export_target_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
