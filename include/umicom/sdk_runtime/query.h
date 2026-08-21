/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/query.h
 *
 * PURPOSE:
 *   Describe read-only SDK/runtime queries and paging.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_QUERY
#define UMICOM_SDK_RUNTIME_QUERY
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeQuery {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t offset;
    uint64_t limit;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeQuery;
void umi_sdk_runtime_query_init(UmiSdkRuntimeQuery *value, const char *id);
UmiStatus umi_sdk_runtime_query_validate(const UmiSdkRuntimeQuery *value);
UmiStatus umi_sdk_runtime_query_set_path(UmiSdkRuntimeQuery *value, const char *path);
UmiStatus umi_sdk_runtime_query_set_detail(UmiSdkRuntimeQuery *value, const char *detail);
UmiStatus umi_sdk_runtime_query_set_offset(UmiSdkRuntimeQuery *value, uint64_t number);
UmiStatus umi_sdk_runtime_query_set_limit(UmiSdkRuntimeQuery *value, uint64_t number);
UmiStatus umi_sdk_runtime_query_set_state(UmiSdkRuntimeQuery *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_query_same_identity(const UmiSdkRuntimeQuery *left, const UmiSdkRuntimeQuery *right);
#ifdef __cplusplus
}
#endif
#endif
