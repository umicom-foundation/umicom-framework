/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/version_contract.h
 *
 * PURPOSE:
 *   Retain package, schema and ABI compatibility requirements independently.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_VERSION_CONTRACT
#define UMICOM_SDK_RUNTIME_VERSION_CONTRACT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeVersionContract {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t major;
    uint64_t minor;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeVersionContract;
void umi_sdk_runtime_version_contract_init(UmiSdkRuntimeVersionContract *value, const char *id);
UmiStatus umi_sdk_runtime_version_contract_validate(const UmiSdkRuntimeVersionContract *value);
UmiStatus umi_sdk_runtime_version_contract_set_path(UmiSdkRuntimeVersionContract *value, const char *path);
UmiStatus umi_sdk_runtime_version_contract_set_detail(UmiSdkRuntimeVersionContract *value, const char *detail);
UmiStatus umi_sdk_runtime_version_contract_set_major(UmiSdkRuntimeVersionContract *value, uint64_t number);
UmiStatus umi_sdk_runtime_version_contract_set_minor(UmiSdkRuntimeVersionContract *value, uint64_t number);
UmiStatus umi_sdk_runtime_version_contract_set_state(UmiSdkRuntimeVersionContract *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_version_contract_same_identity(const UmiSdkRuntimeVersionContract *left, const UmiSdkRuntimeVersionContract *right);
#ifdef __cplusplus
}
#endif
#endif
