/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_search.h
 *
 * PURPOSE:
 *   Search ordered SDK/package roots and retain selection evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_SEARCH
#define UMICOM_SDK_RUNTIME_PACKAGE_SEARCH
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimePackageSearch
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t matched_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimePackageSearch;
void umi_sdk_runtime_package_search_init(UmiSdkRuntimePackageSearch *value,const char *id);
UmiStatus umi_sdk_runtime_package_search_validate(const UmiSdkRuntimePackageSearch *value);
UmiStatus umi_sdk_runtime_package_search_set_path(UmiSdkRuntimePackageSearch *value,const char *path);
UmiStatus umi_sdk_runtime_package_search_set_detail(UmiSdkRuntimePackageSearch *value,const char *detail);
UmiStatus umi_sdk_runtime_package_search_set_candidate_count(UmiSdkRuntimePackageSearch *value,uint64_t number);
UmiStatus umi_sdk_runtime_package_search_set_matched_count(UmiSdkRuntimePackageSearch *value,uint64_t number);
UmiStatus umi_sdk_runtime_package_search_set_state(UmiSdkRuntimePackageSearch *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_package_search_same_identity(const UmiSdkRuntimePackageSearch *left,const UmiSdkRuntimePackageSearch *right);
#ifdef __cplusplus
}
#endif
#endif
