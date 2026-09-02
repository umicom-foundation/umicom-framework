/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/quality_gate_result.h
 *
 * PURPOSE:
 *   Model quality gate result state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_QUALITY_GATE_RESULT_H
#define UMICOM_TEST_WORKBENCH_QUALITY_GATE_RESULT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the quality gate result data shared with callers of this public contract.
 */
typedef struct UmiQualityGateResult {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiQualityGateResult;
/**
 * Initialise quality gate result from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_quality_gate_result_init(UmiQualityGateResult *model,const char *id,const char *label);
/**
 * Exercise quality gate result set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_quality_gate_result_set_active(UmiQualityGateResult *model,bool active);
/**
 * Return the number of records represented by quality gate result set without changing
 * their state.
 */
UmiStatus umi_quality_gate_result_set_count(UmiQualityGateResult *model,uint32_t item_count);
/**
 * Exercise quality gate result set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_quality_gate_result_set_state(UmiQualityGateResult *model,UmiTestWorkbenchState state);
/**
 * Check that quality gate result satisfies its contract before another service relies on
 * it.
 */
int umi_quality_gate_result_valid(const UmiQualityGateResult *model);
#ifdef __cplusplus
}
#endif
#endif
