/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_relocation.c
 *
 * PURPOSE:
 *   Verify the relocation SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/relocation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeRelocation value,same;
    umi_sdk_runtime_relocation_init(&value,"sdk-runtime.relocation");
    assert(umi_sdk_runtime_relocation_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_relocation_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_relocation_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_relocation_set_checked_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_relocation_set_absolute_path_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_relocation_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_relocation_init(&same,"sdk-runtime.relocation");
    assert(umi_sdk_runtime_relocation_same_identity(&value,&same));
    return 0;
    }
