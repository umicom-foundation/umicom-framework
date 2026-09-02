/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_environment_model.h
 *
 * PURPOSE:
 *   Model test environment model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_ENVIRONMENT_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_ENVIRONMENT_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test environment model data shared with callers of this public contract.
 */
typedef struct UmiTestEnvironmentModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestEnvironmentModel;
/**
 * Initialise test environment model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_environment_model_init(UmiTestEnvironmentModel *model,const char *id,const char *label);
/**
 * Exercise test environment model set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_environment_model_set_active(UmiTestEnvironmentModel *model,bool active);
/**
 * Return the number of records represented by test environment model set without changing
 * their state.
 */
UmiStatus umi_test_environment_model_set_count(UmiTestEnvironmentModel *model,uint32_t item_count);
/**
 * Exercise test environment model set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_environment_model_set_state(UmiTestEnvironmentModel *model,UmiTestWorkbenchState state);
/**
 * Check that test environment model satisfies its contract before another service relies
 * on it.
 */
int umi_test_environment_model_valid(const UmiTestEnvironmentModel *model);
#ifdef __cplusplus
}
#endif
#endif
