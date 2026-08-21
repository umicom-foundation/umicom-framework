/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_generation.h
 *
 * PURPOSE:
 *   Identify immutable package generations used by applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_GENERATION
#define UMICOM_SDK_RUNTIME_PACKAGE_GENERATION
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimePackageGeneration
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t generation;
    uint64_t source_revision;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimePackageGeneration;
void umi_sdk_runtime_package_generation_init(UmiSdkRuntimePackageGeneration *value,const char *id);
UmiStatus umi_sdk_runtime_package_generation_validate(const UmiSdkRuntimePackageGeneration *value);
UmiStatus umi_sdk_runtime_package_generation_set_path(UmiSdkRuntimePackageGeneration *value,const char *path);
UmiStatus umi_sdk_runtime_package_generation_set_detail(UmiSdkRuntimePackageGeneration *value,const char *detail);
UmiStatus umi_sdk_runtime_package_generation_set_generation(UmiSdkRuntimePackageGeneration *value,uint64_t number);
UmiStatus umi_sdk_runtime_package_generation_set_source_revision(UmiSdkRuntimePackageGeneration *value,uint64_t number);
UmiStatus umi_sdk_runtime_package_generation_set_state(UmiSdkRuntimePackageGeneration *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_package_generation_same_identity(const UmiSdkRuntimePackageGeneration *left,const UmiSdkRuntimePackageGeneration *right);
#ifdef __cplusplus
}
#endif
#endif
