/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/rollback_plan.h
 *
 * PURPOSE:
 *   Plan runtime rollback to a known compatible package generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_ROLLBACK_PLAN
#define UMICOM_SDK_RUNTIME_ROLLBACK_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime rollback plan data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeRollbackPlan
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t action_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeRollbackPlan;
/**
 * Initialise sdk runtime rollback plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_rollback_plan_init(UmiSdkRuntimeRollbackPlan *value,const char *id);
/**
 * Check that sdk runtime rollback plan satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_rollback_plan_validate(const UmiSdkRuntimeRollbackPlan *value);
/**
 * Provide the sdk runtime rollback plan set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_rollback_plan_set_path(UmiSdkRuntimeRollbackPlan *value,const char *path);
/**
 * Provide the sdk runtime rollback plan set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_rollback_plan_set_detail(UmiSdkRuntimeRollbackPlan *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime rollback plan set action without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_rollback_plan_set_action_count(UmiSdkRuntimeRollbackPlan *value,uint64_t number);
/**
 * Provide the sdk runtime rollback plan set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_rollback_plan_set_generation(UmiSdkRuntimeRollbackPlan *value,uint64_t number);
/**
 * Provide the sdk runtime rollback plan set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_rollback_plan_set_state(UmiSdkRuntimeRollbackPlan *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime rollback plan same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_rollback_plan_same_identity(const UmiSdkRuntimeRollbackPlan *left,const UmiSdkRuntimeRollbackPlan *right);
#ifdef __cplusplus
}
#endif
#endif
