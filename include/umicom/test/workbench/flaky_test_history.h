/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/flaky_test_history.h
 *
 * PURPOSE:
 *   Model flaky test history state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_FLAKY_TEST_HISTORY_H
#define UMICOM_TEST_WORKBENCH_FLAKY_TEST_HISTORY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiFlakyTestHistory {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiFlakyTestHistory;
UmiStatus umi_flaky_test_history_init(UmiFlakyTestHistory *model,const char *id,const char *label);
UmiStatus umi_flaky_test_history_set_active(UmiFlakyTestHistory *model,bool active);
UmiStatus umi_flaky_test_history_set_count(UmiFlakyTestHistory *model,uint32_t item_count);
UmiStatus umi_flaky_test_history_set_state(UmiFlakyTestHistory *model,UmiTestWorkbenchState state);
int umi_flaky_test_history_valid(const UmiFlakyTestHistory *model);
#ifdef __cplusplus
}
#endif
#endif
