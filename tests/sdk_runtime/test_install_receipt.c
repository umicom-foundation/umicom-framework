/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_install_receipt.c
 *
 * PURPOSE:
 *   Verify the install receipt SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/install_receipt.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeInstallReceipt value,same;
    umi_sdk_runtime_install_receipt_init(&value,"sdk-runtime.install_receipt");
    assert(umi_sdk_runtime_install_receipt_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_install_receipt_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_install_receipt_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_install_receipt_set_entry_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_install_receipt_set_generation(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_install_receipt_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_install_receipt_init(&same,"sdk-runtime.install_receipt");
    assert(umi_sdk_runtime_install_receipt_same_identity(&value,&same));
    return 0;
    }
