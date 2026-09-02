/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/uninstall_plan.h
 *
 * PURPOSE:
 *   Plan removal of package-owned files without touching user data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_UNINSTALL_PLAN
#define UMICOM_SDK_RUNTIME_UNINSTALL_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime uninstall plan data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeUninstallPlan
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t action_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeUninstallPlan;
/**
 * Initialise sdk runtime uninstall plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_uninstall_plan_init(UmiSdkRuntimeUninstallPlan *value,const char *id);
/**
 * Check that sdk runtime uninstall plan satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_uninstall_plan_validate(const UmiSdkRuntimeUninstallPlan *value);
/**
 * Provide the sdk runtime uninstall plan set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_uninstall_plan_set_path(UmiSdkRuntimeUninstallPlan *value,const char *path);
/**
 * Provide the sdk runtime uninstall plan set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_uninstall_plan_set_detail(UmiSdkRuntimeUninstallPlan *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime uninstall plan set action
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_uninstall_plan_set_action_count(UmiSdkRuntimeUninstallPlan *value,uint64_t number);
/**
 * Provide the sdk runtime uninstall plan set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_uninstall_plan_set_generation(UmiSdkRuntimeUninstallPlan *value,uint64_t number);
/**
 * Provide the sdk runtime uninstall plan set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_uninstall_plan_set_state(UmiSdkRuntimeUninstallPlan *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime uninstall plan same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_uninstall_plan_same_identity(const UmiSdkRuntimeUninstallPlan *left,const UmiSdkRuntimeUninstallPlan *right);
#ifdef __cplusplus
}
#endif
#endif
