/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_tree_model.h
 *
 * PURPOSE:
 *   Model test tree model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_TREE_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_TREE_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test tree model data shared with callers of this public contract.
 */
typedef struct UmiTestTreeModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestTreeModel;
/**
 * Initialise test tree model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_tree_model_init(UmiTestTreeModel *model,const char *id,const char *label);
/**
 * Exercise test tree model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_tree_model_set_active(UmiTestTreeModel *model,bool active);
/**
 * Return the number of records represented by test tree model set without changing their
 * state.
 */
UmiStatus umi_test_tree_model_set_count(UmiTestTreeModel *model,uint32_t item_count);
/**
 * Exercise test tree model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_tree_model_set_state(UmiTestTreeModel *model,UmiTestWorkbenchState state);
/**
 * Check that test tree model satisfies its contract before another service relies on it.
 */
int umi_test_tree_model_valid(const UmiTestTreeModel *model);
#ifdef __cplusplus
}
#endif
#endif
