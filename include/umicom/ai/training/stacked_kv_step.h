/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_kv_step.h
 *
 * PURPOSE:
 *   Evaluate the NEXT genuine stacked-Transformer token using previously
 *   captured, checkpoint-bound key/value projections. Compose the original
 *   Framework tape primitives, without introducing a second attention,
 *   normalisation, matrix or automatic-differentiation implementation.
 *
 * MATHEMATICAL CONTRACT:
 *   The new token occupies the next unchanged learned-absolute position.
 *   Earlier causal Q/K/V remain valid while the prefix only grows. A single
 *   new query uses the existing multi-head causal-attention operator against
 *   copied previous K/V rows and its own new K/V. Previous query rows are
 *   harmless zero placeholders; only the last attended row is selected.
 *   This deliberately performs more attention-query work than an optimised
 *   fused kernel. There is no throughput or whole-model speedup guarantee.
 *
 *   This API refuses a FULL cache; learned-absolute positions and retained
 *   upper-layer histories must all be recomputed after context eviction.
 *   Callers use the canonical StackedDecoderTraceProjections and the existing
 *   store's ReplaceRolled operation at that boundary.
 *
 * OWNERSHIP:
 *   Caller retains decoder/store lifetimes and serialises all model use.
 *   The complete staged K/V rows and logits belong to the caller. The store
 *   is only READ here. Failure does not publish outputs or update weights;
 *   temporary tape nodes are rolled back, clearing prior gradient readiness
 *   as explicitly documented by the established training-tape contract.
 *   No disk, network, GPU driver, or other language runtime is introduced.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_KV_STEP_H
#define UMICOM_AI_TRAINING_STACKED_KV_STEP_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/stacked_kv_store.h"
#include "umicom/ai/training/stacked_model_identity.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedKvStepReport {
    size_t retainedBefore;
    size_t retainedAfter;
    size_t transformerLayers;
    size_t cachedKeyValueRowsRead;
    uint64_t newlyProjectedTokenLayerRows;
    uint64_t attentionQueryLayerRowsExecuted;
    uint64_t canonicalFullContextTokenLayerRows;
} UmiAiTrainingStackedKvStepReport;

/* Produce ONE candidate token's actual per-layer K/V projections and last-
 * position vocabulary logits, all through the established tape mathematics.
 * The borrowed model's SHA-256 checkpoint identity must agree with the store.
 * 1 <= retained < context; tokenId < vocabulary. Capacities count float
 * ELEMENTS. K/V output layout is [layer][channel]; caller must eventually
 * append both through StackedKvStoreAppend if this candidate is accepted.
 * All three outputs must be disjoint. outReport may be NULL. No store
 * mutation takes place here, even after success: this separation makes
 * independent canonical-logit qualification possible BEFORE publication.
 * Caller outputs remain byte-for-byte unchanged after every rejection. */
UmiStatus UmiAiTrainingStackedKvStepPropose(
    UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedKvStore *store,
    size_t nextTokenId,
    float *outKeys, size_t keyCapacity,
    float *outValues, size_t valueCapacity,
    float *outLastLogits, size_t logitCapacity,
    UmiAiTrainingStackedKvStepReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_KV_STEP_H */
