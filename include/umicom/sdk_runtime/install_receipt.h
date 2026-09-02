/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/install_receipt.h
 *
 * PURPOSE:
 *   Retain installed component receipts for repair and uninstall planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_INSTALL_RECEIPT
#define UMICOM_SDK_RUNTIME_INSTALL_RECEIPT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime install receipt data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeInstallReceipt
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeInstallReceipt;
/**
 * Initialise sdk runtime install receipt from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_install_receipt_init(UmiSdkRuntimeInstallReceipt *value,const char *id);
/**
 * Check that sdk runtime install receipt satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_install_receipt_validate(const UmiSdkRuntimeInstallReceipt *value);
/**
 * Provide the sdk runtime install receipt set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_install_receipt_set_path(UmiSdkRuntimeInstallReceipt *value,const char *path);
/**
 * Provide the sdk runtime install receipt set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_install_receipt_set_detail(UmiSdkRuntimeInstallReceipt *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime install receipt set entry
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_install_receipt_set_entry_count(UmiSdkRuntimeInstallReceipt *value,uint64_t number);
/**
 * Provide the sdk runtime install receipt set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_install_receipt_set_generation(UmiSdkRuntimeInstallReceipt *value,uint64_t number);
/**
 * Provide the sdk runtime install receipt set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_install_receipt_set_state(UmiSdkRuntimeInstallReceipt *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime install receipt same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_install_receipt_same_identity(const UmiSdkRuntimeInstallReceipt *left,const UmiSdkRuntimeInstallReceipt *right);
#ifdef __cplusplus
}
#endif
#endif
