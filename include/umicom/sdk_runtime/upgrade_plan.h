/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/upgrade_plan.h
 *
 * PURPOSE:
 *   Plan SDK/runtime upgrades with ABI and rollback evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_UPGRADE_PLAN
#define UMICOM_SDK_RUNTIME_UPGRADE_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeUpgradePlan
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t action_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeUpgradePlan;
void umi_sdk_runtime_upgrade_plan_init(UmiSdkRuntimeUpgradePlan *value,const char *id);
UmiStatus umi_sdk_runtime_upgrade_plan_validate(const UmiSdkRuntimeUpgradePlan *value);
UmiStatus umi_sdk_runtime_upgrade_plan_set_path(UmiSdkRuntimeUpgradePlan *value,const char *path);
UmiStatus umi_sdk_runtime_upgrade_plan_set_detail(UmiSdkRuntimeUpgradePlan *value,const char *detail);
UmiStatus umi_sdk_runtime_upgrade_plan_set_action_count(UmiSdkRuntimeUpgradePlan *value,uint64_t number);
UmiStatus umi_sdk_runtime_upgrade_plan_set_generation(UmiSdkRuntimeUpgradePlan *value,uint64_t number);
UmiStatus umi_sdk_runtime_upgrade_plan_set_state(UmiSdkRuntimeUpgradePlan *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_upgrade_plan_same_identity(const UmiSdkRuntimeUpgradePlan *left,const UmiSdkRuntimeUpgradePlan *right);
#ifdef __cplusplus
}
#endif
#endif
