/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_failure_model.h
 *
 * PURPOSE:
 *   Model test failure model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_FAILURE_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_FAILURE_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestFailureModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestFailureModel;
UmiStatus umi_test_failure_model_init(UmiTestFailureModel *model,const char *id,const char *label);
UmiStatus umi_test_failure_model_set_active(UmiTestFailureModel *model,bool active);
UmiStatus umi_test_failure_model_set_count(UmiTestFailureModel *model,uint32_t item_count);
UmiStatus umi_test_failure_model_set_state(UmiTestFailureModel *model,UmiTestWorkbenchState state);
int umi_test_failure_model_valid(const UmiTestFailureModel *model);
#ifdef __cplusplus
}
#endif
#endif
