/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/installation_layout.h
 *
 * PURPOSE:
 *   Describe include, library, binary, resource and package locations of an installed Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_INSTALLATION_LAYOUT
#define UMICOM_SDK_RUNTIME_INSTALLATION_LAYOUT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime installation layout data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeInstallationLayout {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t component_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeInstallationLayout;
/**
 * Initialise sdk runtime installation layout from caller-provided values so later
 * operations receive a known state.
 */
void umi_sdk_runtime_installation_layout_init(UmiSdkRuntimeInstallationLayout *value, const char *id);
/**
 * Check that sdk runtime installation layout satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_installation_layout_validate(const UmiSdkRuntimeInstallationLayout *value);
/**
 * Provide the sdk runtime installation layout set path operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_installation_layout_set_path(UmiSdkRuntimeInstallationLayout *value, const char *path);
/**
 * Provide the sdk runtime installation layout set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_installation_layout_set_detail(UmiSdkRuntimeInstallationLayout *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime installation layout set
 * component without changing their state.
 */
UmiStatus umi_sdk_runtime_installation_layout_set_component_count(UmiSdkRuntimeInstallationLayout *value, uint64_t number);
/**
 * Provide the sdk runtime installation layout set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_installation_layout_set_generation(UmiSdkRuntimeInstallationLayout *value, uint64_t number);
/**
 * Provide the sdk runtime installation layout set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_installation_layout_set_state(UmiSdkRuntimeInstallationLayout *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime installation layout same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_installation_layout_same_identity(const UmiSdkRuntimeInstallationLayout *left, const UmiSdkRuntimeInstallationLayout *right);
#ifdef __cplusplus
}
#endif
#endif
