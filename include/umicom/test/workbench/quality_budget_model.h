/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/quality_budget_model.h
 *
 * PURPOSE:
 *   Model quality budget model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_QUALITY_BUDGET_MODEL_H
#define UMICOM_TEST_WORKBENCH_QUALITY_BUDGET_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiQualityBudgetModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiQualityBudgetModel;
UmiStatus umi_quality_budget_model_init(UmiQualityBudgetModel *model,const char *id,const char *label);
UmiStatus umi_quality_budget_model_set_active(UmiQualityBudgetModel *model,bool active);
UmiStatus umi_quality_budget_model_set_count(UmiQualityBudgetModel *model,uint32_t item_count);
UmiStatus umi_quality_budget_model_set_state(UmiQualityBudgetModel *model,UmiTestWorkbenchState state);
int umi_quality_budget_model_valid(const UmiQualityBudgetModel *model);
#ifdef __cplusplus
}
#endif
#endif
