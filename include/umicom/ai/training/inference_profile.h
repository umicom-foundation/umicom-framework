/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/inference_profile.h
 *
 * PURPOSE:
 *   Measure the cost and numerical parity of the native decoder and its
 *   existing inference-only key/value cache on the SAME token stream.
 *
 * ENGINEERING CONTRACT:
 *   The Framework remains the sole owner of decoder mathematics, training,
 *   model storage, and the qualified cache. This inspection service adds no
 *   model implementation. All output structures are copied only on success.
 *   The caller serialises access to a borrowed decoder for the duration of
 *   the call; neither its trainable parameters nor its step count is changed.
 *
 * MEASUREMENT BOUNDARIES:
 *   Logical parameter bytes are NOT process resident-set size. Cache bytes
 *   describe the sizeof allocation, including reserved capacity, but exclude
 *   allocator metadata. CPU timings use ISO C clock() process CPU time, not
 *   wall-clock time and not a calibrated speedup guarantee. Computed-row
 *   counts are instrumented workload units, not hardware instruction counts.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_INFERENCE_PROFILE_H
#define UMICOM_AI_TRAINING_INFERENCE_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_INFERENCE_PROFILE_MAX_TOKENS 256U

/* All byte fields refer to float32 payloads or the documented cache object,
 * not temporary graph allocations or the operating system's heap overhead. */
typedef struct UmiAiTrainingInferenceFootprint {
    size_t parameterTensors;
    size_t parameterElements;
    size_t logicalParameterBytes;
    size_t keyValuePayloadBytes;
    size_t fullContextLogitsBytes;
    size_t cacheAllocationBytes;
} UmiAiTrainingInferenceFootprint;

typedef struct UmiAiTrainingInferenceProfile {
    UmiAiTrainingInferenceFootprint footprint;
    size_t streamTokens;
    size_t prefillTokens;
    size_t referenceQueries;
    size_t retainedTokens;
    uint64_t referenceComputedRows;
    uint64_t cacheComputedRows;
    uint64_t cacheWindowRebuilds;
    double maximumLogitDeviation;
    double cachedProcessCpuSeconds;
    double referenceProcessCpuSeconds;
    int cpuClockAvailable;
} UmiAiTrainingInferenceProfile;

/* Calculate exact logical parameter sizes using the canonical decoder's
 * internal parameter-shape owner. Return the full fixed cache allocation
 * size for the current build, not an assumed compact allocation. */
UmiStatus UmiAiTrainingInferenceEstimate(
    const UmiAiTrainingDecoderConfig *config,
    UmiAiTrainingInferenceFootprint *outFootprint);

/* Compare cached prefix prefill and all appended tokens against the canonical
 * decoder on a single, bounded stream. Reference queries always use the
 * complete configured context. For short prefixes, unused FUTURE positions
 * are filled with BOS; causal masking ensures those positions cannot change
 * the preceding token's output. Once the window rolls, its learned absolute
 * positions are recomputed by the established cache implementation.
 *
 * prefillTokens must be in [1, min(streamTokens, contextLength)]. Valid
 * stream IDs must be below vocabularySize; streamTokens <= MAX_TOKENS.
 * A numerical mismatch above 5e-5 fails rather than publishing a misleading
 * speed comparison. Rejections leave *outProfile unchanged. */
UmiStatus UmiAiTrainingInferenceProfileStream(
    UmiAiTrainingDecoder *decoder,
    const size_t *streamIds, size_t streamTokens,
    size_t prefillTokens,
    UmiAiTrainingInferenceProfile *outProfile);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_INFERENCE_PROFILE_H */
