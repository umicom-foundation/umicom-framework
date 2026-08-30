/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_root.h
 *
 * PURPOSE:
 *   Describe package prefix resolution without relying on source checkout layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_ROOT
#define UMICOM_SDK_RUNTIME_PACKAGE_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimePackageRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t selected;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimePackageRoot;
void umi_sdk_runtime_package_root_init(UmiSdkRuntimePackageRoot *value,const char *id);
UmiStatus umi_sdk_runtime_package_root_validate(const UmiSdkRuntimePackageRoot *value);
UmiStatus umi_sdk_runtime_package_root_set_path(UmiSdkRuntimePackageRoot *value,const char *path);
UmiStatus umi_sdk_runtime_package_root_set_detail(UmiSdkRuntimePackageRoot *value,const char *detail);
UmiStatus umi_sdk_runtime_package_root_set_candidate_count(UmiSdkRuntimePackageRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_package_root_set_selected(UmiSdkRuntimePackageRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_package_root_set_state(UmiSdkRuntimePackageRoot *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_package_root_same_identity(const UmiSdkRuntimePackageRoot *left,const UmiSdkRuntimePackageRoot *right);
#ifdef __cplusplus
}
#endif
#endif
