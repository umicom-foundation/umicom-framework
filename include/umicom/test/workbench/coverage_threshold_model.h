/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_threshold_model.h
 *
 * PURPOSE:
 *   Model coverage threshold model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_THRESHOLD_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_THRESHOLD_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCoverageThresholdModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageThresholdModel;
UmiStatus umi_coverage_threshold_model_init(UmiCoverageThresholdModel *model,const char *id,const char *label);
UmiStatus umi_coverage_threshold_model_set_active(UmiCoverageThresholdModel *model,bool active);
UmiStatus umi_coverage_threshold_model_set_count(UmiCoverageThresholdModel *model,uint32_t item_count);
UmiStatus umi_coverage_threshold_model_set_state(UmiCoverageThresholdModel *model,UmiTestWorkbenchState state);
int umi_coverage_threshold_model_valid(const UmiCoverageThresholdModel *model);
#ifdef __cplusplus
}
#endif
#endif
