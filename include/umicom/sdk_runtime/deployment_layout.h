/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/deployment_layout.h
 *
 * PURPOSE:
 *   Describe relocatable runtime deployment directories and resource roots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_DEPLOYMENT_LAYOUT
#define UMICOM_SDK_RUNTIME_DEPLOYMENT_LAYOUT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime deployment layout data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeDeploymentLayout {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t directory_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeDeploymentLayout;
/**
 * Initialise sdk runtime deployment layout from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_deployment_layout_init(UmiSdkRuntimeDeploymentLayout *value, const char *id);
/**
 * Check that sdk runtime deployment layout satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_deployment_layout_validate(const UmiSdkRuntimeDeploymentLayout *value);
/**
 * Provide the sdk runtime deployment layout set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_path(UmiSdkRuntimeDeploymentLayout *value, const char *path);
/**
 * Provide the sdk runtime deployment layout set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_detail(UmiSdkRuntimeDeploymentLayout *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime deployment layout set directory
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_directory_count(UmiSdkRuntimeDeploymentLayout *value, uint64_t number);
/**
 * Provide the sdk runtime deployment layout set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_generation(UmiSdkRuntimeDeploymentLayout *value, uint64_t number);
/**
 * Provide the sdk runtime deployment layout set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_state(UmiSdkRuntimeDeploymentLayout *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime deployment layout same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_deployment_layout_same_identity(const UmiSdkRuntimeDeploymentLayout *left, const UmiSdkRuntimeDeploymentLayout *right);
#ifdef __cplusplus
}
#endif
#endif
