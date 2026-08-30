/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_rerun_plan.h
 *
 * PURPOSE:
 *   Model test rerun plan state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_RERUN_PLAN_H
#define UMICOM_TEST_WORKBENCH_TEST_RERUN_PLAN_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRerunPlan {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRerunPlan;
UmiStatus umi_test_rerun_plan_init(UmiTestRerunPlan *model,const char *id,const char *label);
UmiStatus umi_test_rerun_plan_set_active(UmiTestRerunPlan *model,bool active);
UmiStatus umi_test_rerun_plan_set_count(UmiTestRerunPlan *model,uint32_t item_count);
UmiStatus umi_test_rerun_plan_set_state(UmiTestRerunPlan *model,UmiTestWorkbenchState state);
int umi_test_rerun_plan_valid(const UmiTestRerunPlan *model);
#ifdef __cplusplus
}
#endif
#endif
