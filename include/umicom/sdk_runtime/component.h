/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/component.h
 *
 * PURPOSE:
 *   Describe one installed Framework component and its stable identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMPONENT
#define UMICOM_SDK_RUNTIME_COMPONENT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeComponent {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t abi_version;
    uint64_t package_version;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeComponent;
void umi_sdk_runtime_component_init(UmiSdkRuntimeComponent *value, const char *id);
UmiStatus umi_sdk_runtime_component_validate(const UmiSdkRuntimeComponent *value);
UmiStatus umi_sdk_runtime_component_set_path(UmiSdkRuntimeComponent *value, const char *path);
UmiStatus umi_sdk_runtime_component_set_detail(UmiSdkRuntimeComponent *value, const char *detail);
UmiStatus umi_sdk_runtime_component_set_abi_version(UmiSdkRuntimeComponent *value, uint64_t number);
UmiStatus umi_sdk_runtime_component_set_package_version(UmiSdkRuntimeComponent *value, uint64_t number);
UmiStatus umi_sdk_runtime_component_set_state(UmiSdkRuntimeComponent *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_component_same_identity(const UmiSdkRuntimeComponent *left, const UmiSdkRuntimeComponent *right);
#ifdef __cplusplus
}
#endif
#endif
