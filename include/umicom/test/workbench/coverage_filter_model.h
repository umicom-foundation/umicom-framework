/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_filter_model.h
 *
 * PURPOSE:
 *   Model coverage filter model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_FILTER_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_FILTER_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCoverageFilterModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageFilterModel;
UmiStatus umi_coverage_filter_model_init(UmiCoverageFilterModel *model,const char *id,const char *label);
UmiStatus umi_coverage_filter_model_set_active(UmiCoverageFilterModel *model,bool active);
UmiStatus umi_coverage_filter_model_set_count(UmiCoverageFilterModel *model,uint32_t item_count);
UmiStatus umi_coverage_filter_model_set_state(UmiCoverageFilterModel *model,UmiTestWorkbenchState state);
int umi_coverage_filter_model_valid(const UmiCoverageFilterModel *model);
#ifdef __cplusplus
}
#endif
#endif
