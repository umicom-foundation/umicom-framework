/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_summary_model.h
 *
 * PURPOSE:
 *   Model coverage summary model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_SUMMARY_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_SUMMARY_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCoverageSummaryModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageSummaryModel;
UmiStatus umi_coverage_summary_model_init(UmiCoverageSummaryModel *model,const char *id,const char *label);
UmiStatus umi_coverage_summary_model_set_active(UmiCoverageSummaryModel *model,bool active);
UmiStatus umi_coverage_summary_model_set_count(UmiCoverageSummaryModel *model,uint32_t item_count);
UmiStatus umi_coverage_summary_model_set_state(UmiCoverageSummaryModel *model,UmiTestWorkbenchState state);
int umi_coverage_summary_model_valid(const UmiCoverageSummaryModel *model);
#ifdef __cplusplus
}
#endif
#endif
