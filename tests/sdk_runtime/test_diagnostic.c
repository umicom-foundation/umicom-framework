/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_diagnostic.c
 *
 * PURPOSE:
 *   Verify the diagnostic contract and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/diagnostic.h"
int main(void)
{
    UmiSdkRuntimeDiagnostic value; UmiSdkRuntimeDiagnostic same; uint64_t revision;
    umi_sdk_runtime_diagnostic_init(&value, "sdk-runtime.diagnostic");
    assert(umi_sdk_runtime_diagnostic_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_diagnostic_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_diagnostic_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_diagnostic_set_severity(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_diagnostic_set_sequence(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_diagnostic_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.severity == 3U && value.sequence == 5U);
    umi_sdk_runtime_diagnostic_init(&same, "sdk-runtime.diagnostic");
    assert(umi_sdk_runtime_diagnostic_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
