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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_COVERAGE_THRESHOLD_MODEL_H
#define UMICOM_TEST_WORKBENCH_COVERAGE_THRESHOLD_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the coverage threshold model data shared with callers of this public contract.
 */
typedef struct UmiCoverageThresholdModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageThresholdModel;
/**
 * Initialise coverage threshold model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_coverage_threshold_model_init(UmiCoverageThresholdModel *model,const char *id,const char *label);
/**
 * Exercise coverage threshold model set active and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_coverage_threshold_model_set_active(UmiCoverageThresholdModel *model,bool active);
/**
 * Return the number of records represented by coverage threshold model set without
 * changing their state.
 */
UmiStatus umi_coverage_threshold_model_set_count(UmiCoverageThresholdModel *model,uint32_t item_count);
/**
 * Exercise coverage threshold model set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_coverage_threshold_model_set_state(UmiCoverageThresholdModel *model,UmiTestWorkbenchState state);
/**
 * Check that coverage threshold model satisfies its contract before another service relies
 * on it.
 */
int umi_coverage_threshold_model_valid(const UmiCoverageThresholdModel *model);
#ifdef __cplusplus
}
#endif
#endif
