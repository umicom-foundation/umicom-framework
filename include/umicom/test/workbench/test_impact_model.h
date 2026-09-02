/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_impact_model.h
 *
 * PURPOSE:
 *   Model test impact model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_IMPACT_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_IMPACT_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test impact model data shared with callers of this public contract.
 */
typedef struct UmiTestImpactModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestImpactModel;
/**
 * Initialise test impact model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_test_impact_model_init(UmiTestImpactModel *model,const char *id,const char *label);
/**
 * Exercise test impact model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_impact_model_set_active(UmiTestImpactModel *model,bool active);
/**
 * Return the number of records represented by test impact model set without changing their
 * state.
 */
UmiStatus umi_test_impact_model_set_count(UmiTestImpactModel *model,uint32_t item_count);
/**
 * Exercise test impact model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_test_impact_model_set_state(UmiTestImpactModel *model,UmiTestWorkbenchState state);
/**
 * Check that test impact model satisfies its contract before another service relies on it.
 */
int umi_test_impact_model_valid(const UmiTestImpactModel *model);
#ifdef __cplusplus
}
#endif
#endif
