/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_run_progress.h
 *
 * PURPOSE:
 *   Model test run progress state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_RUN_PROGRESS_H
#define UMICOM_TEST_WORKBENCH_TEST_RUN_PROGRESS_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRunProgress {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRunProgress;
UmiStatus umi_test_run_progress_init(UmiTestRunProgress *model,const char *id,const char *label);
UmiStatus umi_test_run_progress_set_active(UmiTestRunProgress *model,bool active);
UmiStatus umi_test_run_progress_set_count(UmiTestRunProgress *model,uint32_t item_count);
UmiStatus umi_test_run_progress_set_state(UmiTestRunProgress *model,UmiTestWorkbenchState state);
int umi_test_run_progress_valid(const UmiTestRunProgress *model);
#ifdef __cplusplus
}
#endif
#endif
