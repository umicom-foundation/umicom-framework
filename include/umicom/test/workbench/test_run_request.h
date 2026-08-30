/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_run_request.h
 *
 * PURPOSE:
 *   Model test run request state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RUN_REQUEST_H
#define UMICOM_TEST_WORKBENCH_TEST_RUN_REQUEST_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRunRequest {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRunRequest;
UmiStatus umi_test_run_request_init(UmiTestRunRequest *model,const char *id,const char *label);
UmiStatus umi_test_run_request_set_active(UmiTestRunRequest *model,bool active);
UmiStatus umi_test_run_request_set_count(UmiTestRunRequest *model,uint32_t item_count);
UmiStatus umi_test_run_request_set_state(UmiTestRunRequest *model,UmiTestWorkbenchState state);
int umi_test_run_request_valid(const UmiTestRunRequest *model);
#ifdef __cplusplus
}
#endif
#endif
