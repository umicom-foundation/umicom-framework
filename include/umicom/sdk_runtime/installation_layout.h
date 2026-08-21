/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/installation_layout.h
 *
 * PURPOSE:
 *   Describe include, library, binary, resource and package locations of an installed Framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_INSTALLATION_LAYOUT
#define UMICOM_SDK_RUNTIME_INSTALLATION_LAYOUT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_sdk_runtime_installation_layout_init(UmiSdkRuntimeInstallationLayout *value, const char *id);
UmiStatus umi_sdk_runtime_installation_layout_validate(const UmiSdkRuntimeInstallationLayout *value);
UmiStatus umi_sdk_runtime_installation_layout_set_path(UmiSdkRuntimeInstallationLayout *value, const char *path);
UmiStatus umi_sdk_runtime_installation_layout_set_detail(UmiSdkRuntimeInstallationLayout *value, const char *detail);
UmiStatus umi_sdk_runtime_installation_layout_set_component_count(UmiSdkRuntimeInstallationLayout *value, uint64_t number);
UmiStatus umi_sdk_runtime_installation_layout_set_generation(UmiSdkRuntimeInstallationLayout *value, uint64_t number);
UmiStatus umi_sdk_runtime_installation_layout_set_state(UmiSdkRuntimeInstallationLayout *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_installation_layout_same_identity(const UmiSdkRuntimeInstallationLayout *left, const UmiSdkRuntimeInstallationLayout *right);
#ifdef __cplusplus
}
#endif
#endif
