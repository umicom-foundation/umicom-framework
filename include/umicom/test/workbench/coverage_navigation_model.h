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
/**
 * Represent the coverage navigation model data shared with callers of this public
 * contract.
 */
typedef struct UmiCoverageNavigationModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageNavigationModel;
/**
 * Initialise coverage navigation model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_coverage_navigation_model_init(UmiCoverageNavigationModel *model,const char *id,const char *label);
/**
 * Exercise coverage navigation model set active and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_coverage_navigation_model_set_active(UmiCoverageNavigationModel *model,bool active);
/**
 * Return the number of records represented by coverage navigation model set without
 * changing their state.
 */
UmiStatus umi_coverage_navigation_model_set_count(UmiCoverageNavigationModel *model,uint32_t item_count);
/**
 * Exercise coverage navigation model set state and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_coverage_navigation_model_set_state(UmiCoverageNavigationModel *model,UmiTestWorkbenchState state);
/**
 * Check that coverage navigation model satisfies its contract before another service
 * relies on it.
 */
int umi_coverage_navigation_model_valid(const UmiCoverageNavigationModel *model);
#ifdef __cplusplus
}
#endif
#endif
