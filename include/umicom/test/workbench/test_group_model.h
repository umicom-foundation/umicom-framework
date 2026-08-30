/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_group_model.h
 *
 * PURPOSE:
 *   Model test group model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_GROUP_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_GROUP_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestGroupModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestGroupModel;
UmiStatus umi_test_group_model_init(UmiTestGroupModel *model,const char *id,const char *label);
UmiStatus umi_test_group_model_set_active(UmiTestGroupModel *model,bool active);
UmiStatus umi_test_group_model_set_count(UmiTestGroupModel *model,uint32_t item_count);
UmiStatus umi_test_group_model_set_state(UmiTestGroupModel *model,UmiTestWorkbenchState state);
int umi_test_group_model_valid(const UmiTestGroupModel *model);
#ifdef __cplusplus
}
#endif
#endif
