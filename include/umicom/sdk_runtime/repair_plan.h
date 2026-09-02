/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/repair_plan.h
 *
 * PURPOSE:
 *   Plan repair of missing or incompatible installed runtime components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_REPAIR_PLAN
#define UMICOM_SDK_RUNTIME_REPAIR_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime repair plan data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeRepairPlan
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t action_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeRepairPlan;
/**
 * Initialise sdk runtime repair plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_repair_plan_init(UmiSdkRuntimeRepairPlan *value,const char *id);
/**
 * Check that sdk runtime repair plan satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_repair_plan_validate(const UmiSdkRuntimeRepairPlan *value);
/**
 * Provide the sdk runtime repair plan set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_repair_plan_set_path(UmiSdkRuntimeRepairPlan *value,const char *path);
/**
 * Provide the sdk runtime repair plan set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_repair_plan_set_detail(UmiSdkRuntimeRepairPlan *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime repair plan set action without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_repair_plan_set_action_count(UmiSdkRuntimeRepairPlan *value,uint64_t number);
/**
 * Provide the sdk runtime repair plan set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_repair_plan_set_generation(UmiSdkRuntimeRepairPlan *value,uint64_t number);
/**
 * Provide the sdk runtime repair plan set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_repair_plan_set_state(UmiSdkRuntimeRepairPlan *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime repair plan same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_repair_plan_same_identity(const UmiSdkRuntimeRepairPlan *left,const UmiSdkRuntimeRepairPlan *right);
#ifdef __cplusplus
}
#endif
#endif
