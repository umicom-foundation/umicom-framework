/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_retry_policy.h
 *
 * PURPOSE:
 *   Model test retry policy state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_RETRY_POLICY_H
#define UMICOM_TEST_WORKBENCH_TEST_RETRY_POLICY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRetryPolicy {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestRetryPolicy;
UmiStatus umi_test_retry_policy_init(UmiTestRetryPolicy *model,const char *id,const char *label);
UmiStatus umi_test_retry_policy_set_active(UmiTestRetryPolicy *model,bool active);
UmiStatus umi_test_retry_policy_set_count(UmiTestRetryPolicy *model,uint32_t item_count);
UmiStatus umi_test_retry_policy_set_state(UmiTestRetryPolicy *model,UmiTestWorkbenchState state);
int umi_test_retry_policy_valid(const UmiTestRetryPolicy *model);
#ifdef __cplusplus
}
#endif
#endif
