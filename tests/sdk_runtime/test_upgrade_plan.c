/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_upgrade_plan.c
 *
 * PURPOSE:
 *   Verify the upgrade plan SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/upgrade_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeUpgradePlan value,same;
    umi_sdk_runtime_upgrade_plan_init(&value,"sdk-runtime.upgrade_plan");
    assert(umi_sdk_runtime_upgrade_plan_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_upgrade_plan_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_upgrade_plan_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_upgrade_plan_set_action_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_upgrade_plan_set_generation(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_upgrade_plan_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_upgrade_plan_init(&same,"sdk-runtime.upgrade_plan");
    assert(umi_sdk_runtime_upgrade_plan_same_identity(&value,&same));
    return 0;
    }
