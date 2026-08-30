/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_status_summary.h
 *
 * PURPOSE:
 *   Model test status summary state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_STATUS_SUMMARY_H
#define UMICOM_TEST_WORKBENCH_TEST_STATUS_SUMMARY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestStatusSummary {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestStatusSummary;
UmiStatus umi_test_status_summary_init(UmiTestStatusSummary *model,const char *id,const char *label);
UmiStatus umi_test_status_summary_set_active(UmiTestStatusSummary *model,bool active);
UmiStatus umi_test_status_summary_set_count(UmiTestStatusSummary *model,uint32_t item_count);
UmiStatus umi_test_status_summary_set_state(UmiTestStatusSummary *model,UmiTestWorkbenchState state);
int umi_test_status_summary_valid(const UmiTestStatusSummary *model);
#ifdef __cplusplus
}
#endif
#endif
