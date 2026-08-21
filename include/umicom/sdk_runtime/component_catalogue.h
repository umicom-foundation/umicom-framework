/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/component_catalogue.h
 *
 * PURPOSE:
 *   Maintain installed component inventory and compatibility evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMPONENT_CATALOGUE
#define UMICOM_SDK_RUNTIME_COMPONENT_CATALOGUE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeComponentCatalogue {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t component_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeComponentCatalogue;
void umi_sdk_runtime_component_catalogue_init(UmiSdkRuntimeComponentCatalogue *value, const char *id);
UmiStatus umi_sdk_runtime_component_catalogue_validate(const UmiSdkRuntimeComponentCatalogue *value);
UmiStatus umi_sdk_runtime_component_catalogue_set_path(UmiSdkRuntimeComponentCatalogue *value, const char *path);
UmiStatus umi_sdk_runtime_component_catalogue_set_detail(UmiSdkRuntimeComponentCatalogue *value, const char *detail);
UmiStatus umi_sdk_runtime_component_catalogue_set_component_count(UmiSdkRuntimeComponentCatalogue *value, uint64_t number);
UmiStatus umi_sdk_runtime_component_catalogue_set_generation(UmiSdkRuntimeComponentCatalogue *value, uint64_t number);
UmiStatus umi_sdk_runtime_component_catalogue_set_state(UmiSdkRuntimeComponentCatalogue *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_component_catalogue_same_identity(const UmiSdkRuntimeComponentCatalogue *left, const UmiSdkRuntimeComponentCatalogue *right);
#ifdef __cplusplus
}
#endif
#endif
