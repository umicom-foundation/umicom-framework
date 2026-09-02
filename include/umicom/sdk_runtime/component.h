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
/**
 * Represent the sdk runtime component data shared with callers of this public contract.
 */
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
/**
 * Initialise sdk runtime component from caller-provided values so later operations receive
 * a known state.
 */
void umi_sdk_runtime_component_init(UmiSdkRuntimeComponent *value, const char *id);
/**
 * Check that sdk runtime component satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_sdk_runtime_component_validate(const UmiSdkRuntimeComponent *value);
/**
 * Provide the sdk runtime component set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_component_set_path(UmiSdkRuntimeComponent *value, const char *path);
/**
 * Provide the sdk runtime component set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_component_set_detail(UmiSdkRuntimeComponent *value, const char *detail);
/**
 * Provide the sdk runtime component set abi version operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_component_set_abi_version(UmiSdkRuntimeComponent *value, uint64_t number);
/**
 * Provide the sdk runtime component set package version operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_component_set_package_version(UmiSdkRuntimeComponent *value, uint64_t number);
/**
 * Provide the sdk runtime component set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_component_set_state(UmiSdkRuntimeComponent *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime component same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_component_same_identity(const UmiSdkRuntimeComponent *left, const UmiSdkRuntimeComponent *right);
#ifdef __cplusplus
}
#endif
#endif
