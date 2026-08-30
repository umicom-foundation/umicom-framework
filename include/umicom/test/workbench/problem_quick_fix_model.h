/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/problem_quick_fix_model.h
 *
 * PURPOSE:
 *   Model problem quick fix model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_PROBLEM_QUICK_FIX_MODEL_H
#define UMICOM_TEST_WORKBENCH_PROBLEM_QUICK_FIX_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProblemQuickFixModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiProblemQuickFixModel;
UmiStatus umi_problem_quick_fix_model_init(UmiProblemQuickFixModel *model,const char *id,const char *label);
UmiStatus umi_problem_quick_fix_model_set_active(UmiProblemQuickFixModel *model,bool active);
UmiStatus umi_problem_quick_fix_model_set_count(UmiProblemQuickFixModel *model,uint32_t item_count);
UmiStatus umi_problem_quick_fix_model_set_state(UmiProblemQuickFixModel *model,UmiTestWorkbenchState state);
int umi_problem_quick_fix_model_valid(const UmiProblemQuickFixModel *model);
#ifdef __cplusplus
}
#endif
#endif
