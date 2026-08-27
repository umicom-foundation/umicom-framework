/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_debug_plan.h
 *
 * PURPOSE:
 *   Model test debug plan state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_DEBUG_PLAN_H
#define UMICOM_TEST_WORKBENCH_TEST_DEBUG_PLAN_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestDebugPlan {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestDebugPlan;
UmiStatus umi_test_debug_plan_init(UmiTestDebugPlan *model,const char *id,const char *label);
UmiStatus umi_test_debug_plan_set_active(UmiTestDebugPlan *model,bool active);
UmiStatus umi_test_debug_plan_set_count(UmiTestDebugPlan *model,uint32_t item_count);
UmiStatus umi_test_debug_plan_set_state(UmiTestDebugPlan *model,UmiTestWorkbenchState state);
int umi_test_debug_plan_valid(const UmiTestDebugPlan *model);
#ifdef __cplusplus
}
#endif
#endif
