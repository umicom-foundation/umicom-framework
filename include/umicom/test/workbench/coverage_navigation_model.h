/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/coverage_navigation_model.h
 *
 * PURPOSE:
 *   Model coverage navigation model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_NAVIGATION_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_NAVIGATION_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiCoverageNavigationModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageNavigationModel;
UmiStatus umi_coverage_navigation_model_init(UmiCoverageNavigationModel *model,const char *id,const char *label);
UmiStatus umi_coverage_navigation_model_set_active(UmiCoverageNavigationModel *model,bool active);
UmiStatus umi_coverage_navigation_model_set_count(UmiCoverageNavigationModel *model,uint32_t item_count);
UmiStatus umi_coverage_navigation_model_set_state(UmiCoverageNavigationModel *model,UmiTestWorkbenchState state);
int umi_coverage_navigation_model_valid(const UmiCoverageNavigationModel *model);
#ifdef __cplusplus
}
#endif
#endif
