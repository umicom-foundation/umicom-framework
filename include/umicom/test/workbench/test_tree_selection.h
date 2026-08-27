/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_tree_selection.h
 *
 * PURPOSE:
 *   Model test tree selection state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_TREE_SELECTION_H
#define UMICOM_TEST_WORKBENCH_TEST_TREE_SELECTION_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestTreeSelection {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestTreeSelection;
UmiStatus umi_test_tree_selection_init(UmiTestTreeSelection *model,const char *id,const char *label);
UmiStatus umi_test_tree_selection_set_active(UmiTestTreeSelection *model,bool active);
UmiStatus umi_test_tree_selection_set_count(UmiTestTreeSelection *model,uint32_t item_count);
UmiStatus umi_test_tree_selection_set_state(UmiTestTreeSelection *model,UmiTestWorkbenchState state);
int umi_test_tree_selection_valid(const UmiTestTreeSelection *model);
#ifdef __cplusplus
}
#endif
#endif
