/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/repair_plan.h
 *
 * PURPOSE:
 *   Plan repair of missing or incompatible installed runtime components.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_REPAIR_PLAN
#define UMICOM_SDK_RUNTIME_REPAIR_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeRepairPlan
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t action_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeRepairPlan;
void umi_sdk_runtime_repair_plan_init(UmiSdkRuntimeRepairPlan *value,const char *id);
UmiStatus umi_sdk_runtime_repair_plan_validate(const UmiSdkRuntimeRepairPlan *value);
UmiStatus umi_sdk_runtime_repair_plan_set_path(UmiSdkRuntimeRepairPlan *value,const char *path);
UmiStatus umi_sdk_runtime_repair_plan_set_detail(UmiSdkRuntimeRepairPlan *value,const char *detail);
UmiStatus umi_sdk_runtime_repair_plan_set_action_count(UmiSdkRuntimeRepairPlan *value,uint64_t number);
UmiStatus umi_sdk_runtime_repair_plan_set_generation(UmiSdkRuntimeRepairPlan *value,uint64_t number);
UmiStatus umi_sdk_runtime_repair_plan_set_state(UmiSdkRuntimeRepairPlan *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_repair_plan_same_identity(const UmiSdkRuntimeRepairPlan *left,const UmiSdkRuntimeRepairPlan *right);
#ifdef __cplusplus
}
#endif
#endif
