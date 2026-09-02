/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_item_model.h
 *
 * PURPOSE:
 *   Model test item model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_ITEM_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_ITEM_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test item model data shared with callers of this public contract.
 */
typedef struct UmiTestItemModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestItemModel;
/**
 * Initialise test item model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_item_model_init(UmiTestItemModel *model,const char *id,const char *label);
/**
 * Exercise test item model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_item_model_set_active(UmiTestItemModel *model,bool active);
/**
 * Return the number of records represented by test item model set without changing their
 * state.
 */
UmiStatus umi_test_item_model_set_count(UmiTestItemModel *model,uint32_t item_count);
/**
 * Exercise test item model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_item_model_set_state(UmiTestItemModel *model,UmiTestWorkbenchState state);
/**
 * Check that test item model satisfies its contract before another service relies on it.
 */
int umi_test_item_model_valid(const UmiTestItemModel *model);
#ifdef __cplusplus
}
#endif
#endif
