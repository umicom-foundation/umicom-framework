/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/loader_plan.h
 *
 * PURPOSE:
 *   Describe runtime load order without exposing platform loader handles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_LOADER_PLAN
#define UMICOM_SDK_RUNTIME_LOADER_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeLoaderPlan {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeLoaderPlan;
void umi_sdk_runtime_loader_plan_init(UmiSdkRuntimeLoaderPlan *value, const char *id);
UmiStatus umi_sdk_runtime_loader_plan_validate(const UmiSdkRuntimeLoaderPlan *value);
UmiStatus umi_sdk_runtime_loader_plan_set_path(UmiSdkRuntimeLoaderPlan *value, const char *path);
UmiStatus umi_sdk_runtime_loader_plan_set_detail(UmiSdkRuntimeLoaderPlan *value, const char *detail);
UmiStatus umi_sdk_runtime_loader_plan_set_entry_count(UmiSdkRuntimeLoaderPlan *value, uint64_t number);
UmiStatus umi_sdk_runtime_loader_plan_set_generation(UmiSdkRuntimeLoaderPlan *value, uint64_t number);
UmiStatus umi_sdk_runtime_loader_plan_set_state(UmiSdkRuntimeLoaderPlan *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_loader_plan_same_identity(const UmiSdkRuntimeLoaderPlan *left, const UmiSdkRuntimeLoaderPlan *right);
#ifdef __cplusplus
}
#endif
#endif
