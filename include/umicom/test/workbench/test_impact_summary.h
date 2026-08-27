/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_impact_summary.h
 *
 * PURPOSE:
 *   Model test impact summary state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_IMPACT_SUMMARY_H
#define UMICOM_TEST_WORKBENCH_TEST_IMPACT_SUMMARY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestImpactSummary {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestImpactSummary;
UmiStatus umi_test_impact_summary_init(UmiTestImpactSummary *model,const char *id,const char *label);
UmiStatus umi_test_impact_summary_set_active(UmiTestImpactSummary *model,bool active);
UmiStatus umi_test_impact_summary_set_count(UmiTestImpactSummary *model,uint32_t item_count);
UmiStatus umi_test_impact_summary_set_state(UmiTestImpactSummary *model,UmiTestWorkbenchState state);
int umi_test_impact_summary_valid(const UmiTestImpactSummary *model);
#ifdef __cplusplus
}
#endif
#endif
