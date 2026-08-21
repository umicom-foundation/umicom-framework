/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/uninstall_plan.h
 *
 * PURPOSE:
 *   Plan removal of package-owned files without touching user data.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_UNINSTALL_PLAN
#define UMICOM_SDK_RUNTIME_UNINSTALL_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeUninstallPlan
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t action_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeUninstallPlan;
void umi_sdk_runtime_uninstall_plan_init(UmiSdkRuntimeUninstallPlan *value,const char *id);
UmiStatus umi_sdk_runtime_uninstall_plan_validate(const UmiSdkRuntimeUninstallPlan *value);
UmiStatus umi_sdk_runtime_uninstall_plan_set_path(UmiSdkRuntimeUninstallPlan *value,const char *path);
UmiStatus umi_sdk_runtime_uninstall_plan_set_detail(UmiSdkRuntimeUninstallPlan *value,const char *detail);
UmiStatus umi_sdk_runtime_uninstall_plan_set_action_count(UmiSdkRuntimeUninstallPlan *value,uint64_t number);
UmiStatus umi_sdk_runtime_uninstall_plan_set_generation(UmiSdkRuntimeUninstallPlan *value,uint64_t number);
UmiStatus umi_sdk_runtime_uninstall_plan_set_state(UmiSdkRuntimeUninstallPlan *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_uninstall_plan_same_identity(const UmiSdkRuntimeUninstallPlan *left,const UmiSdkRuntimeUninstallPlan *right);
#ifdef __cplusplus
}
#endif
#endif
