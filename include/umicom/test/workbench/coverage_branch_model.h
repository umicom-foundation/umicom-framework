/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_branch_model.h
 *
 * PURPOSE:
 *   Model coverage branch model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_BRANCH_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_BRANCH_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCoverageBranchModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageBranchModel;
UmiStatus umi_coverage_branch_model_init(UmiCoverageBranchModel *model,const char *id,const char *label);
UmiStatus umi_coverage_branch_model_set_active(UmiCoverageBranchModel *model,bool active);
UmiStatus umi_coverage_branch_model_set_count(UmiCoverageBranchModel *model,uint32_t item_count);
UmiStatus umi_coverage_branch_model_set_state(UmiCoverageBranchModel *model,UmiTestWorkbenchState state);
int umi_coverage_branch_model_valid(const UmiCoverageBranchModel *model);
#ifdef __cplusplus
}
#endif
#endif
