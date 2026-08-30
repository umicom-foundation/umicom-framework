/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_run_session.h
 *
 * PURPOSE:
 *   Model test run session state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RUN_SESSION_H
#define UMICOM_TEST_WORKBENCH_TEST_RUN_SESSION_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRunSession {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRunSession;
UmiStatus umi_test_run_session_init(UmiTestRunSession *model,const char *id,const char *label);
UmiStatus umi_test_run_session_set_active(UmiTestRunSession *model,bool active);
UmiStatus umi_test_run_session_set_count(UmiTestRunSession *model,uint32_t item_count);
UmiStatus umi_test_run_session_set_state(UmiTestRunSession *model,UmiTestWorkbenchState state);
int umi_test_run_session_valid(const UmiTestRunSession *model);
#ifdef __cplusplus
}
#endif
#endif
