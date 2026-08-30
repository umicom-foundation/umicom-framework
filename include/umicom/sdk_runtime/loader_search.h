/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/loader_search.h
 *
 * PURPOSE:
 *   Search runtime dependencies using explicit platform rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_LOADER_SEARCH
#define UMICOM_SDK_RUNTIME_LOADER_SEARCH
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeLoaderSearch
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t resolved_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeLoaderSearch;
void umi_sdk_runtime_loader_search_init(UmiSdkRuntimeLoaderSearch *value,const char *id);
UmiStatus umi_sdk_runtime_loader_search_validate(const UmiSdkRuntimeLoaderSearch *value);
UmiStatus umi_sdk_runtime_loader_search_set_path(UmiSdkRuntimeLoaderSearch *value,const char *path);
UmiStatus umi_sdk_runtime_loader_search_set_detail(UmiSdkRuntimeLoaderSearch *value,const char *detail);
UmiStatus umi_sdk_runtime_loader_search_set_candidate_count(UmiSdkRuntimeLoaderSearch *value,uint64_t number);
UmiStatus umi_sdk_runtime_loader_search_set_resolved_count(UmiSdkRuntimeLoaderSearch *value,uint64_t number);
UmiStatus umi_sdk_runtime_loader_search_set_state(UmiSdkRuntimeLoaderSearch *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_loader_search_same_identity(const UmiSdkRuntimeLoaderSearch *left,const UmiSdkRuntimeLoaderSearch *right);
#ifdef __cplusplus
}
#endif
#endif
