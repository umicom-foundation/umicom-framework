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
typedef struct UmiTestTreeModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestTreeModel;
UmiStatus umi_test_tree_model_init(UmiTestTreeModel *model,const char *id,const char *label);
UmiStatus umi_test_tree_model_set_active(UmiTestTreeModel *model,bool active);
UmiStatus umi_test_tree_model_set_count(UmiTestTreeModel *model,uint32_t item_count);
UmiStatus umi_test_tree_model_set_state(UmiTestTreeModel *model,UmiTestWorkbenchState state);
int umi_test_tree_model_valid(const UmiTestTreeModel *model);
#ifdef __cplusplus
}
#endif
#endif
