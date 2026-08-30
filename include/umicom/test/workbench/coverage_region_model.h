/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_region_model.h
 *
 * PURPOSE:
 *   Model coverage region model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_REGION_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_REGION_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCoverageRegionModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageRegionModel;
UmiStatus umi_coverage_region_model_init(UmiCoverageRegionModel *model,const char *id,const char *label);
UmiStatus umi_coverage_region_model_set_active(UmiCoverageRegionModel *model,bool active);
UmiStatus umi_coverage_region_model_set_count(UmiCoverageRegionModel *model,uint32_t item_count);
UmiStatus umi_coverage_region_model_set_state(UmiCoverageRegionModel *model,UmiTestWorkbenchState state);
int umi_coverage_region_model_valid(const UmiCoverageRegionModel *model);
#ifdef __cplusplus
}
#endif
#endif
