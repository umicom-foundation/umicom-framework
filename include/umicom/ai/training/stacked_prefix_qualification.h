/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_prefix_qualification.h
 * PURPOSE:
 *   Independently qualify causal-prefix invariance of real stacked-model
 *   logits before introducing multi-layer retained-key/value optimisations.
 *
 * The reference evaluates the SAME trained stacked decoder twice using
 * different FUTURE suffix tokens. Every logit belonging to the unchanged
 * prefix must be equal within the declared floating-point tolerance, because
 * causal attention must not read future tokens at any Transformer layer.
 * This tests a crucial necessary invariant, NOT the sufficiency of a cache,
 * numerical equivalence after eviction, model usefulness or CPU speed.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_PREFIX_QUALIFICATION_H
#define UMICOM_AI_TRAINING_STACKED_PREFIX_QUALIFICATION_H

#include "umicom/ai/training/stacked_inference.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedPrefixQualification {
    size_t contextTokens;
    size_t unchangedPrefixTokens;
    size_t comparedLogits;
    size_t transformerLayers;
    float maximumAbsoluteDifference;
    UmiAiTrainingStackedModelIdentity modelIdentity;
} UmiAiTrainingStackedPrefixQualification;

/* prefixTokenCount must be 1..contextLength-1. Compare every logit from
 * every retained prefix row, not merely the winning token. The two reference
 * contexts use identical prefix IDs but distinct future padding values:
 * BOS versus the first ordinary byte-token ID. Both must be legal vocabulary
 * IDs. A non-finite logit or difference > 0.00004 fails INVALID_STATE.
 * Actual model/checkpoint identity is checked before and after both calls.
 * All output memory belongs to the caller and is unchanged on every failure.
 * No encoder change, checkpoint write, training or inferred KV saving occurs. */
UmiStatus UmiAiTrainingStackedPrefixCausalityQualify(
    UmiAiTrainingStackedDecoder *model,
    const size_t *prefixTokenIds, size_t prefixTokenCount,
    UmiAiTrainingStackedPrefixQualification *outEvidence);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_PREFIX_QUALIFICATION_H */
