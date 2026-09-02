/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_suite_model.h
 *
 * PURPOSE:
 *   Model test suite model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_SUITE_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_SUITE_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test suite model data shared with callers of this public contract.
 */
typedef struct UmiTestSuiteModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestSuiteModel;
/**
 * Initialise test suite model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_suite_model_init(UmiTestSuiteModel *model,const char *id,const char *label);
/**
 * Exercise test suite model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_suite_model_set_active(UmiTestSuiteModel *model,bool active);
/**
 * Return the number of records represented by test suite model set without changing their
 * state.
 */
UmiStatus umi_test_suite_model_set_count(UmiTestSuiteModel *model,uint32_t item_count);
/**
 * Exercise test suite model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_suite_model_set_state(UmiTestSuiteModel *model,UmiTestWorkbenchState state);
/**
 * Check that test suite model satisfies its contract before another service relies on it.
 */
int umi_test_suite_model_valid(const UmiTestSuiteModel *model);
#ifdef __cplusplus
}
#endif
#endif
