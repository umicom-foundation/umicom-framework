/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_output_session.h
 *
 * PURPOSE:
 *   Model test output session state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_OUTPUT_SESSION_H
#define UMICOM_TEST_WORKBENCH_TEST_OUTPUT_SESSION_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestOutputSession {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestOutputSession;
UmiStatus umi_test_output_session_init(UmiTestOutputSession *model,const char *id,const char *label);
UmiStatus umi_test_output_session_set_active(UmiTestOutputSession *model,bool active);
UmiStatus umi_test_output_session_set_count(UmiTestOutputSession *model,uint32_t item_count);
UmiStatus umi_test_output_session_set_state(UmiTestOutputSession *model,UmiTestWorkbenchState state);
int umi_test_output_session_valid(const UmiTestOutputSession *model);
#ifdef __cplusplus
}
#endif
#endif
