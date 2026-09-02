/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_tree_filter.h
 *
 * PURPOSE:
 *   Model test tree filter state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_TREE_FILTER_H
#define UMICOM_TEST_WORKBENCH_TEST_TREE_FILTER_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test tree filter data shared with callers of this public contract.
 */
typedef struct UmiTestTreeFilter {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestTreeFilter;
/**
 * Initialise test tree filter from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_tree_filter_init(UmiTestTreeFilter *model,const char *id,const char *label);
/**
 * Exercise test tree filter set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_tree_filter_set_active(UmiTestTreeFilter *model,bool active);
/**
 * Return the number of records represented by test tree filter set without changing their
 * state.
 */
UmiStatus umi_test_tree_filter_set_count(UmiTestTreeFilter *model,uint32_t item_count);
/**
 * Exercise test tree filter set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_tree_filter_set_state(UmiTestTreeFilter *model,UmiTestWorkbenchState state);
/**
 * Check that test tree filter satisfies its contract before another service relies on it.
 */
int umi_test_tree_filter_valid(const UmiTestTreeFilter *model);
#ifdef __cplusplus
}
#endif
#endif
