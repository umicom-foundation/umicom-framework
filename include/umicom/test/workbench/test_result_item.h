/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_result_item.h
 *
 * PURPOSE:
 *   Model test result item state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_RESULT_ITEM_H
#define UMICOM_TEST_WORKBENCH_TEST_RESULT_ITEM_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestResultItem {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestResultItem;
UmiStatus umi_test_result_item_init(UmiTestResultItem *model,const char *id,const char *label);
UmiStatus umi_test_result_item_set_active(UmiTestResultItem *model,bool active);
UmiStatus umi_test_result_item_set_count(UmiTestResultItem *model,uint32_t item_count);
UmiStatus umi_test_result_item_set_state(UmiTestResultItem *model,UmiTestWorkbenchState state);
int umi_test_result_item_valid(const UmiTestResultItem *model);
#ifdef __cplusplus
}
#endif
#endif
