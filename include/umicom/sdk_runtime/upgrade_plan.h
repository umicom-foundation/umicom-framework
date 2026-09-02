/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/upgrade_plan.h
 *
 * PURPOSE:
 *   Plan SDK/runtime upgrades with ABI and rollback evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_UPGRADE_PLAN
#define UMICOM_SDK_RUNTIME_UPGRADE_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime upgrade plan data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeUpgradePlan
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t action_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeUpgradePlan;
/**
 * Initialise sdk runtime upgrade plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_upgrade_plan_init(UmiSdkRuntimeUpgradePlan *value,const char *id);
/**
 * Check that sdk runtime upgrade plan satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_upgrade_plan_validate(const UmiSdkRuntimeUpgradePlan *value);
/**
 * Provide the sdk runtime upgrade plan set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_upgrade_plan_set_path(UmiSdkRuntimeUpgradePlan *value,const char *path);
/**
 * Provide the sdk runtime upgrade plan set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_upgrade_plan_set_detail(UmiSdkRuntimeUpgradePlan *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime upgrade plan set action without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_upgrade_plan_set_action_count(UmiSdkRuntimeUpgradePlan *value,uint64_t number);
/**
 * Provide the sdk runtime upgrade plan set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_upgrade_plan_set_generation(UmiSdkRuntimeUpgradePlan *value,uint64_t number);
/**
 * Provide the sdk runtime upgrade plan set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_upgrade_plan_set_state(UmiSdkRuntimeUpgradePlan *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime upgrade plan same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_upgrade_plan_same_identity(const UmiSdkRuntimeUpgradePlan *left,const UmiSdkRuntimeUpgradePlan *right);
#ifdef __cplusplus
}
#endif
#endif
