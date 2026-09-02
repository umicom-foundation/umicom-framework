/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_configuration_model.h
 *
 * PURPOSE:
 *   Model test configuration model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_CONFIGURATION_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_CONFIGURATION_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test configuration model data shared with callers of this public contract.
 */
typedef struct UmiTestConfigurationModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestConfigurationModel;
/**
 * Initialise test configuration model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_configuration_model_init(UmiTestConfigurationModel *model,const char *id,const char *label);
/**
 * Exercise test configuration model set active and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_test_configuration_model_set_active(UmiTestConfigurationModel *model,bool active);
/**
 * Return the number of records represented by test configuration model set without
 * changing their state.
 */
UmiStatus umi_test_configuration_model_set_count(UmiTestConfigurationModel *model,uint32_t item_count);
/**
 * Exercise test configuration model set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_test_configuration_model_set_state(UmiTestConfigurationModel *model,UmiTestWorkbenchState state);
/**
 * Check that test configuration model satisfies its contract before another service relies
 * on it.
 */
int umi_test_configuration_model_valid(const UmiTestConfigurationModel *model);
#ifdef __cplusplus
}
#endif
#endif
