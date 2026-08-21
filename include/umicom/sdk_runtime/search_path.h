/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/search_path.h
 *
 * PURPOSE:
 *   Plan deterministic runtime library and resource search locations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_SEARCH_PATH
#define UMICOM_SDK_RUNTIME_SEARCH_PATH
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeSearchPath {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t path_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeSearchPath;
void umi_sdk_runtime_search_path_init(UmiSdkRuntimeSearchPath *value, const char *id);
UmiStatus umi_sdk_runtime_search_path_validate(const UmiSdkRuntimeSearchPath *value);
UmiStatus umi_sdk_runtime_search_path_set_path(UmiSdkRuntimeSearchPath *value, const char *path);
UmiStatus umi_sdk_runtime_search_path_set_detail(UmiSdkRuntimeSearchPath *value, const char *detail);
UmiStatus umi_sdk_runtime_search_path_set_path_count(UmiSdkRuntimeSearchPath *value, uint64_t number);
UmiStatus umi_sdk_runtime_search_path_set_generation(UmiSdkRuntimeSearchPath *value, uint64_t number);
UmiStatus umi_sdk_runtime_search_path_set_state(UmiSdkRuntimeSearchPath *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_search_path_same_identity(const UmiSdkRuntimeSearchPath *left, const UmiSdkRuntimeSearchPath *right);
#ifdef __cplusplus
}
#endif
#endif
