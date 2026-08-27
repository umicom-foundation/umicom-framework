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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_TREE_FILTER_H
#define UMICOM_TEST_WORKBENCH_TEST_TREE_FILTER_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestTreeFilter {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestTreeFilter;
UmiStatus umi_test_tree_filter_init(UmiTestTreeFilter *model,const char *id,const char *label);
UmiStatus umi_test_tree_filter_set_active(UmiTestTreeFilter *model,bool active);
UmiStatus umi_test_tree_filter_set_count(UmiTestTreeFilter *model,uint32_t item_count);
UmiStatus umi_test_tree_filter_set_state(UmiTestTreeFilter *model,UmiTestWorkbenchState state);
int umi_test_tree_filter_valid(const UmiTestTreeFilter *model);
#ifdef __cplusplus
}
#endif
#endif
