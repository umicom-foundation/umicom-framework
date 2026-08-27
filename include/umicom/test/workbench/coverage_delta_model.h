/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_delta_model.h
 *
 * PURPOSE:
 *   Model coverage delta model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_DELTA_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_DELTA_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCoverageDeltaModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageDeltaModel;
UmiStatus umi_coverage_delta_model_init(UmiCoverageDeltaModel *model,const char *id,const char *label);
UmiStatus umi_coverage_delta_model_set_active(UmiCoverageDeltaModel *model,bool active);
UmiStatus umi_coverage_delta_model_set_count(UmiCoverageDeltaModel *model,uint32_t item_count);
UmiStatus umi_coverage_delta_model_set_state(UmiCoverageDeltaModel *model,UmiTestWorkbenchState state);
int umi_coverage_delta_model_valid(const UmiCoverageDeltaModel *model);
#ifdef __cplusplus
}
#endif
#endif
