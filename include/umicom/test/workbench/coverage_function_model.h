/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_function_model.h
 *
 * PURPOSE:
 *   Model coverage function model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_FUNCTION_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_FUNCTION_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCoverageFunctionModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageFunctionModel;
UmiStatus umi_coverage_function_model_init(UmiCoverageFunctionModel *model,const char *id,const char *label);
UmiStatus umi_coverage_function_model_set_active(UmiCoverageFunctionModel *model,bool active);
UmiStatus umi_coverage_function_model_set_count(UmiCoverageFunctionModel *model,uint32_t item_count);
UmiStatus umi_coverage_function_model_set_state(UmiCoverageFunctionModel *model,UmiTestWorkbenchState state);
int umi_coverage_function_model_valid(const UmiCoverageFunctionModel *model);
#ifdef __cplusplus
}
#endif
#endif
