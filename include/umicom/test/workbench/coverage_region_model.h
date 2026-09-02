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
/**
 * Represent the coverage region model data shared with callers of this public contract.
 */
typedef struct UmiCoverageRegionModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageRegionModel;
/**
 * Initialise coverage region model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_coverage_region_model_init(UmiCoverageRegionModel *model,const char *id,const char *label);
/**
 * Exercise coverage region model set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_coverage_region_model_set_active(UmiCoverageRegionModel *model,bool active);
/**
 * Return the number of records represented by coverage region model set without changing
 * their state.
 */
UmiStatus umi_coverage_region_model_set_count(UmiCoverageRegionModel *model,uint32_t item_count);
/**
 * Exercise coverage region model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_coverage_region_model_set_state(UmiCoverageRegionModel *model,UmiTestWorkbenchState state);
/**
 * Check that coverage region model satisfies its contract before another service relies on
 * it.
 */
int umi_coverage_region_model_valid(const UmiCoverageRegionModel *model);
#ifdef __cplusplus
}
#endif
#endif
