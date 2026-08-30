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
typedef struct UmiSdkRuntimeRollbackPlan
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t action_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeRollbackPlan;
void umi_sdk_runtime_rollback_plan_init(UmiSdkRuntimeRollbackPlan *value,const char *id);
UmiStatus umi_sdk_runtime_rollback_plan_validate(const UmiSdkRuntimeRollbackPlan *value);
UmiStatus umi_sdk_runtime_rollback_plan_set_path(UmiSdkRuntimeRollbackPlan *value,const char *path);
UmiStatus umi_sdk_runtime_rollback_plan_set_detail(UmiSdkRuntimeRollbackPlan *value,const char *detail);
UmiStatus umi_sdk_runtime_rollback_plan_set_action_count(UmiSdkRuntimeRollbackPlan *value,uint64_t number);
UmiStatus umi_sdk_runtime_rollback_plan_set_generation(UmiSdkRuntimeRollbackPlan *value,uint64_t number);
UmiStatus umi_sdk_runtime_rollback_plan_set_state(UmiSdkRuntimeRollbackPlan *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_rollback_plan_same_identity(const UmiSdkRuntimeRollbackPlan *left,const UmiSdkRuntimeRollbackPlan *right);
#ifdef __cplusplus
}
#endif
#endif
