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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_QUALITY_BUDGET_MODEL_H
#define UMICOM_TEST_WORKBENCH_QUALITY_BUDGET_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the quality budget model data shared with callers of this public contract.
 */
typedef struct UmiQualityBudgetModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiQualityBudgetModel;
/**
 * Initialise quality budget model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_quality_budget_model_init(UmiQualityBudgetModel *model,const char *id,const char *label);
/**
 * Exercise quality budget model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_quality_budget_model_set_active(UmiQualityBudgetModel *model,bool active);
/**
 * Return the number of records represented by quality budget model set without changing
 * their state.
 */
UmiStatus umi_quality_budget_model_set_count(UmiQualityBudgetModel *model,uint32_t item_count);
/**
 * Exercise quality budget model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_quality_budget_model_set_state(UmiQualityBudgetModel *model,UmiTestWorkbenchState state);
/**
 * Check that quality budget model satisfies its contract before another service relies on
 * it.
 */
int umi_quality_budget_model_valid(const UmiQualityBudgetModel *model);
#ifdef __cplusplus
}
#endif
#endif
