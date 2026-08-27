/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_result_snapshot.h
 *
 * PURPOSE:
 *   Model test result snapshot state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_RESULT_SNAPSHOT_H
#define UMICOM_TEST_WORKBENCH_TEST_RESULT_SNAPSHOT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestResultSnapshot {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestResultSnapshot;
UmiStatus umi_test_result_snapshot_init(UmiTestResultSnapshot *model,const char *id,const char *label);
UmiStatus umi_test_result_snapshot_set_active(UmiTestResultSnapshot *model,bool active);
UmiStatus umi_test_result_snapshot_set_count(UmiTestResultSnapshot *model,uint32_t item_count);
UmiStatus umi_test_result_snapshot_set_state(UmiTestResultSnapshot *model,UmiTestWorkbenchState state);
int umi_test_result_snapshot_valid(const UmiTestResultSnapshot *model);
#ifdef __cplusplus
}
#endif
#endif
