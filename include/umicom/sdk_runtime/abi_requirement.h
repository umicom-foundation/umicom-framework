/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/abi_requirement.h
 *
 * PURPOSE:
 *   Describe minimum and maximum ABI compatibility accepted by a consumer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_ABI_REQUIREMENT
#define UMICOM_SDK_RUNTIME_ABI_REQUIREMENT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeAbiRequirement {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t minimum_abi;
    uint64_t maximum_abi;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeAbiRequirement;
void umi_sdk_runtime_abi_requirement_init(UmiSdkRuntimeAbiRequirement *value, const char *id);
UmiStatus umi_sdk_runtime_abi_requirement_validate(const UmiSdkRuntimeAbiRequirement *value);
UmiStatus umi_sdk_runtime_abi_requirement_set_path(UmiSdkRuntimeAbiRequirement *value, const char *path);
UmiStatus umi_sdk_runtime_abi_requirement_set_detail(UmiSdkRuntimeAbiRequirement *value, const char *detail);
UmiStatus umi_sdk_runtime_abi_requirement_set_minimum_abi(UmiSdkRuntimeAbiRequirement *value, uint64_t number);
UmiStatus umi_sdk_runtime_abi_requirement_set_maximum_abi(UmiSdkRuntimeAbiRequirement *value, uint64_t number);
UmiStatus umi_sdk_runtime_abi_requirement_set_state(UmiSdkRuntimeAbiRequirement *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_abi_requirement_same_identity(const UmiSdkRuntimeAbiRequirement *left, const UmiSdkRuntimeAbiRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
