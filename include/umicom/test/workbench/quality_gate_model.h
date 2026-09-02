/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/quality_gate_model.h
 *
 * PURPOSE:
 *   Model quality gate model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_QUALITY_GATE_MODEL_H
#define UMICOM_TEST_WORKBENCH_QUALITY_GATE_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the quality gate model data shared with callers of this public contract.
 */
typedef struct UmiQualityGateModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiQualityGateModel;
/**
 * Initialise quality gate model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_quality_gate_model_init(UmiQualityGateModel *model,const char *id,const char *label);
/**
 * Exercise quality gate model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_quality_gate_model_set_active(UmiQualityGateModel *model,bool active);
/**
 * Return the number of records represented by quality gate model set without changing
 * their state.
 */
UmiStatus umi_quality_gate_model_set_count(UmiQualityGateModel *model,uint32_t item_count);
/**
 * Exercise quality gate model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_quality_gate_model_set_state(UmiQualityGateModel *model,UmiTestWorkbenchState state);
/**
 * Check that quality gate model satisfies its contract before another service relies on
 * it.
 */
int umi_quality_gate_model_valid(const UmiQualityGateModel *model);
#ifdef __cplusplus
}
#endif
#endif
