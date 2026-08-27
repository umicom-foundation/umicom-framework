/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/problem_navigation_model.h
 *
 * PURPOSE:
 *   Model problem navigation model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_PROBLEM_NAVIGATION_MODEL_H
#define UMICOM_TEST_WORKBENCH_PROBLEM_NAVIGATION_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProblemNavigationModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiProblemNavigationModel;
UmiStatus umi_problem_navigation_model_init(UmiProblemNavigationModel *model,const char *id,const char *label);
UmiStatus umi_problem_navigation_model_set_active(UmiProblemNavigationModel *model,bool active);
UmiStatus umi_problem_navigation_model_set_count(UmiProblemNavigationModel *model,uint32_t item_count);
UmiStatus umi_problem_navigation_model_set_state(UmiProblemNavigationModel *model,UmiTestWorkbenchState state);
int umi_problem_navigation_model_valid(const UmiProblemNavigationModel *model);
#ifdef __cplusplus
}
#endif
#endif
