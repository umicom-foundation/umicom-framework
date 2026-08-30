/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_result_group.h
 *
 * PURPOSE:
 *   Model test result group state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RESULT_GROUP_H
#define UMICOM_TEST_WORKBENCH_TEST_RESULT_GROUP_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestResultGroup {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestResultGroup;
UmiStatus umi_test_result_group_init(UmiTestResultGroup *model,const char *id,const char *label);
UmiStatus umi_test_result_group_set_active(UmiTestResultGroup *model,bool active);
UmiStatus umi_test_result_group_set_count(UmiTestResultGroup *model,uint32_t item_count);
UmiStatus umi_test_result_group_set_state(UmiTestResultGroup *model,UmiTestWorkbenchState state);
int umi_test_result_group_valid(const UmiTestResultGroup *model);
#ifdef __cplusplus
}
#endif
#endif
