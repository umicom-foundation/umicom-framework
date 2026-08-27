/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_result_history.h
 *
 * PURPOSE:
 *   Model test result history state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_RESULT_HISTORY_H
#define UMICOM_TEST_WORKBENCH_TEST_RESULT_HISTORY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestResultHistory {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestResultHistory;
UmiStatus umi_test_result_history_init(UmiTestResultHistory *model,const char *id,const char *label);
UmiStatus umi_test_result_history_set_active(UmiTestResultHistory *model,bool active);
UmiStatus umi_test_result_history_set_count(UmiTestResultHistory *model,uint32_t item_count);
UmiStatus umi_test_result_history_set_state(UmiTestResultHistory *model,UmiTestWorkbenchState state);
int umi_test_result_history_valid(const UmiTestResultHistory *model);
#ifdef __cplusplus
}
#endif
#endif
