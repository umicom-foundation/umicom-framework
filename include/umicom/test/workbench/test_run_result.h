/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_run_result.h
 *
 * PURPOSE:
 *   Model test run result state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_RUN_RESULT_H
#define UMICOM_TEST_WORKBENCH_TEST_RUN_RESULT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRunResult {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRunResult;
UmiStatus umi_test_run_result_init(UmiTestRunResult *model,const char *id,const char *label);
UmiStatus umi_test_run_result_set_active(UmiTestRunResult *model,bool active);
UmiStatus umi_test_run_result_set_count(UmiTestRunResult *model,uint32_t item_count);
UmiStatus umi_test_run_result_set_state(UmiTestRunResult *model,UmiTestWorkbenchState state);
int umi_test_run_result_valid(const UmiTestRunResult *model);
#ifdef __cplusplus
}
#endif
#endif
