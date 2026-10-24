/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/position_admission.h
 *
 * PURPOSE:
 *   Enforce the positional-compatibility declaration at actual model-load,
 *   token-generation and benchmark boundaries. Batch 77/78 introduced the
 *   portable identity sidecar; this service makes its checks actionable.
 *
 * OWNERSHIP AND SAFETY:
 *   The caller owns every input, output, decoder and storage buffer. No input
 *   path is opened and no checkpoint or model parameter is modified. Existing
 *   Framework checkpoint, SHA-256, sampling, decoder, KV-cache and benchmark
 *   services remain the sole authorities for their respective work.
 *
 *   The record is NOT a trusted signature. It detects mismatched checkpoint
 *   bytes and generation semantics only. Callers must authenticate external
 *   publishers and approve model files independently before using these APIs.
 *   Models must not be concurrently trained, destroyed or accessed. Output
 *   buffers and result objects must not overlap each other or input buffers.
 *   Rejected operations preserve all caller-owned output values.
 *
 * COMPATIBILITY:
 *   The only qualified positional encoding is LEARNED_ABSOLUTE. Both the
 *   historical BOS-left-padded and opt-in growing-unpadded generation modes
 *   are admitted only with an explicit matching declaration. This adds NO
 *   positional-encoding mathematics, checkpoint upgrade or implicit policy
 *   conversion. Old public APIs remain fully operational and unchanged.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_POSITION_ADMISSION_H
#define UMICOM_AI_TRAINING_POSITION_ADMISSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/position_contract.h"
#include "umicom/ai/training/generation.h"
#include "umicom/ai/training/prefix_generation.h"
#include "umicom/ai/training/prefix_benchmark.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingPositionGenerationEvidence {
    UmiAiTrainingPositionContext admittedContext;
    size_t generatedTokens;
    int cacheWorkAvailable; /* 0 for canonical BOS-left-padded generator */
    UmiAiTrainingGenerationCacheStats cache;
    uint8_t checkpointSha256[UMI_SHA256_BYTES];
} UmiAiTrainingPositionGenerationEvidence;

/* Decode a checkpoint only AFTER checking its complete original byte length,
 * the strict 128-byte position sidecar and SHA-256 against the provided bytes.
 * Also verify the decoded model using the canonical checkpoint encoder, not
 * the sidecar's untrusted shape alone. A rejection leaves *outModel unchanged
 * and destroys any temporary decoder. Caller must first authenticate and
 * approve both external inputs; this API is NOT a signature verifier.
 * The output is a newly owned decoder; destroy it with DecoderDestroy. */
UmiStatus UmiAiTrainingPositionBoundCheckpointDecode(
    const uint8_t *checkpointBytes, size_t checkpointLength,
    const uint8_t *sidecarBytes, size_t sidecarLength,
    UmiAiTrainingPositionContext requestedContext,
    UmiAiTrainingDecoder **outModel);

/* Verify the exact checkpoint identity and requested positional policy
 * BEFORE and AFTER generating the entire caller-requested sequence. Existing
 * Framework generators retain all mathematical and RNG responsibilities.
 * Tokens, count and evidence are staged until both checks succeed.
 * The historical BOS-left-padded path does not expose cache work counters;
 * cacheWorkAvailable=0 and zero counters are published, never invented ones.
 * Outgoing capacity counts size_t token IDs, not bytes. */
UmiStatus UmiAiTrainingPositionBoundGenerate(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingPositionContract *approvedContract,
    UmiAiTrainingPositionContext requestedContext,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount,
    UmiAiTrainingPositionGenerationEvidence *outEvidence);

/* Explicitly admit the unpadded-prefix benchmark only: it is mathematically
 * inappropriate to apply its right-BOS-padded canonical comparison to the
 * historical BOS-left-padded mode. Before and after verification protect the
 * checkpoint identity. The established benchmark owns trial parity, clocks,
 * work estimates and checkpoint immutability. On any error *outResult stays
 * byte-for-byte unchanged. */
UmiStatus UmiAiTrainingPositionBoundBenchmark(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingPositionContract *approvedContract,
    UmiAiTrainingPositionContext requestedContext,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    const UmiAiTrainingPrefixBenchmarkPlan *plan,
    UmiAiTrainingPrefixBenchmarkResult *outResult);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_POSITION_ADMISSION_H */
