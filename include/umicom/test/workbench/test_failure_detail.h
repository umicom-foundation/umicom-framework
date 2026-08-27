/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_failure_detail.h
 *
 * PURPOSE:
 *   Model test failure detail state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_FAILURE_DETAIL_H
#define UMICOM_TEST_WORKBENCH_TEST_FAILURE_DETAIL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestFailureDetail {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestFailureDetail;
UmiStatus umi_test_failure_detail_init(UmiTestFailureDetail *model,const char *id,const char *label);
UmiStatus umi_test_failure_detail_set_active(UmiTestFailureDetail *model,bool active);
UmiStatus umi_test_failure_detail_set_count(UmiTestFailureDetail *model,uint32_t item_count);
UmiStatus umi_test_failure_detail_set_state(UmiTestFailureDetail *model,UmiTestWorkbenchState state);
int umi_test_failure_detail_valid(const UmiTestFailureDetail *model);
#ifdef __cplusplus
}
#endif
#endif
