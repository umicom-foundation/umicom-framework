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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_ITEM_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_ITEM_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestItemModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestItemModel;
UmiStatus umi_test_item_model_init(UmiTestItemModel *model,const char *id,const char *label);
UmiStatus umi_test_item_model_set_active(UmiTestItemModel *model,bool active);
UmiStatus umi_test_item_model_set_count(UmiTestItemModel *model,uint32_t item_count);
UmiStatus umi_test_item_model_set_state(UmiTestItemModel *model,UmiTestWorkbenchState state);
int umi_test_item_model_valid(const UmiTestItemModel *model);
#ifdef __cplusplus
}
#endif
#endif
