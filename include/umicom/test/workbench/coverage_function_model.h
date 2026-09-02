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
/**
 * Represent the coverage function model data shared with callers of this public contract.
 */
typedef struct UmiCoverageFunctionModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiCoverageFunctionModel;
/**
 * Initialise coverage function model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_coverage_function_model_init(UmiCoverageFunctionModel *model,const char *id,const char *label);
/**
 * Exercise coverage function model set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_coverage_function_model_set_active(UmiCoverageFunctionModel *model,bool active);
/**
 * Return the number of records represented by coverage function model set without changing
 * their state.
 */
UmiStatus umi_coverage_function_model_set_count(UmiCoverageFunctionModel *model,uint32_t item_count);
/**
 * Exercise coverage function model set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_coverage_function_model_set_state(UmiCoverageFunctionModel *model,UmiTestWorkbenchState state);
/**
 * Check that coverage function model satisfies its contract before another service relies
 * on it.
 */
int umi_coverage_function_model_valid(const UmiCoverageFunctionModel *model);
#ifdef __cplusplus
}
#endif
#endif
