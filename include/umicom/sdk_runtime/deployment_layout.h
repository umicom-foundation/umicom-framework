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
void umi_sdk_runtime_deployment_layout_init(UmiSdkRuntimeDeploymentLayout *value, const char *id);
UmiStatus umi_sdk_runtime_deployment_layout_validate(const UmiSdkRuntimeDeploymentLayout *value);
UmiStatus umi_sdk_runtime_deployment_layout_set_path(UmiSdkRuntimeDeploymentLayout *value, const char *path);
UmiStatus umi_sdk_runtime_deployment_layout_set_detail(UmiSdkRuntimeDeploymentLayout *value, const char *detail);
UmiStatus umi_sdk_runtime_deployment_layout_set_directory_count(UmiSdkRuntimeDeploymentLayout *value, uint64_t number);
UmiStatus umi_sdk_runtime_deployment_layout_set_generation(UmiSdkRuntimeDeploymentLayout *value, uint64_t number);
UmiStatus umi_sdk_runtime_deployment_layout_set_state(UmiSdkRuntimeDeploymentLayout *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_deployment_layout_same_identity(const UmiSdkRuntimeDeploymentLayout *left, const UmiSdkRuntimeDeploymentLayout *right);
#ifdef __cplusplus
}
#endif
#endif
