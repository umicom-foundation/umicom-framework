/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_suite_model.h
 *
 * PURPOSE:
 *   Model test suite model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_SUITE_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_SUITE_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestSuiteModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestSuiteModel;
UmiStatus umi_test_suite_model_init(UmiTestSuiteModel *model,const char *id,const char *label);
UmiStatus umi_test_suite_model_set_active(UmiTestSuiteModel *model,bool active);
UmiStatus umi_test_suite_model_set_count(UmiTestSuiteModel *model,uint32_t item_count);
UmiStatus umi_test_suite_model_set_state(UmiTestSuiteModel *model,UmiTestWorkbenchState state);
int umi_test_suite_model_valid(const UmiTestSuiteModel *model);
#ifdef __cplusplus
}
#endif
#endif
