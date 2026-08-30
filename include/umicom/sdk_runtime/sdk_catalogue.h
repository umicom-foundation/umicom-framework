/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/sdk_catalogue.h
 *
 * PURPOSE:
 *   Maintain reusable SDK profiles for first-party and external consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_SDK_CATALOGUE
#define UMICOM_SDK_RUNTIME_SDK_CATALOGUE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeSdkCatalogue {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t profile_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeSdkCatalogue;
void umi_sdk_runtime_sdk_catalogue_init(UmiSdkRuntimeSdkCatalogue *value, const char *id);
UmiStatus umi_sdk_runtime_sdk_catalogue_validate(const UmiSdkRuntimeSdkCatalogue *value);
UmiStatus umi_sdk_runtime_sdk_catalogue_set_path(UmiSdkRuntimeSdkCatalogue *value, const char *path);
UmiStatus umi_sdk_runtime_sdk_catalogue_set_detail(UmiSdkRuntimeSdkCatalogue *value, const char *detail);
UmiStatus umi_sdk_runtime_sdk_catalogue_set_profile_count(UmiSdkRuntimeSdkCatalogue *value, uint64_t number);
UmiStatus umi_sdk_runtime_sdk_catalogue_set_generation(UmiSdkRuntimeSdkCatalogue *value, uint64_t number);
UmiStatus umi_sdk_runtime_sdk_catalogue_set_state(UmiSdkRuntimeSdkCatalogue *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_sdk_catalogue_same_identity(const UmiSdkRuntimeSdkCatalogue *left, const UmiSdkRuntimeSdkCatalogue *right);
#ifdef __cplusplus
}
#endif
#endif
