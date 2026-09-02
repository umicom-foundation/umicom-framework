/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/quality_score_model.h
 *
 * PURPOSE:
 *   Model quality score model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_QUALITY_SCORE_MODEL_H
#define UMICOM_TEST_WORKBENCH_QUALITY_SCORE_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the quality score model data shared with callers of this public contract.
 */
typedef struct UmiQualityScoreModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiQualityScoreModel;
/**
 * Initialise quality score model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_quality_score_model_init(UmiQualityScoreModel *model,const char *id,const char *label);
/**
 * Exercise quality score model set active and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_quality_score_model_set_active(UmiQualityScoreModel *model,bool active);
/**
 * Return the number of records represented by quality score model set without changing
 * their state.
 */
UmiStatus umi_quality_score_model_set_count(UmiQualityScoreModel *model,uint32_t item_count);
/**
 * Exercise quality score model set state and return a clear result when the behaviour no
 * longer matches its contract.
 */
UmiStatus umi_quality_score_model_set_state(UmiQualityScoreModel *model,UmiTestWorkbenchState state);
/**
 * Check that quality score model satisfies its contract before another service relies on
 * it.
 */
int umi_quality_score_model_valid(const UmiQualityScoreModel *model);
#ifdef __cplusplus
}
#endif
#endif
