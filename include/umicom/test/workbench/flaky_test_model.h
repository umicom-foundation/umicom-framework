/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/flaky_test_model.h
 *
 * PURPOSE:
 *   Model flaky test model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_FLAKY_TEST_MODEL_H
#define UMICOM_TEST_WORKBENCH_FLAKY_TEST_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFlakyTestModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiFlakyTestModel;
UmiStatus umi_flaky_test_model_init(UmiFlakyTestModel *model,const char *id,const char *label);
UmiStatus umi_flaky_test_model_set_active(UmiFlakyTestModel *model,bool active);
UmiStatus umi_flaky_test_model_set_count(UmiFlakyTestModel *model,uint32_t item_count);
UmiStatus umi_flaky_test_model_set_state(UmiFlakyTestModel *model,UmiTestWorkbenchState state);
int umi_flaky_test_model_valid(const UmiFlakyTestModel *model);
#ifdef __cplusplus
}
#endif
#endif
