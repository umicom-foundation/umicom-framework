/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_kv_work.h
 * PURPOSE: Cross-check the ACTUAL stored K/V row count with the existing
 *          conditional stacked-prefix cache planner, without implying speed.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_KV_WORK_H
#define UMICOM_AI_TRAINING_STACKED_KV_WORK_H
#include "umicom/ai/training/stacked_kv_store.h"
#include "umicom/ai/training/stacked_prefix_cache_plan.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedKvWorkAudit {
    uint64_t plannedTokenLayerRows;
    uint64_t observedAcceptedTokenLayerRows;
    uint64_t conditionalRowsAvoided;
    uint64_t observedRebuilds;
    size_t observedRetainedTokens;
    size_t reservedKvBytes;
} UmiAiTrainingStackedKvWorkAudit;

/* Check the store's complete prefill+append+rebuild history against an exact
 * Framework planner case. The final sampled token is pending, not appended;
 * therefore there are generatedTokens-1 state transitions. On ANY mismatch
 * refuse publication. This validates STORAGE WORK, not KV numeric correctness
 * against the canonical decoder, timing, throughput, or quality. */
UmiStatus UmiAiTrainingStackedKvWorkQualify(
    const UmiAiTrainingStackedKvStore *store,
    const UmiAiTrainingStackedPrefixCachePlan *plan,
    UmiAiTrainingStackedKvWorkAudit *outAudit);
#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_KV_WORK_H */
