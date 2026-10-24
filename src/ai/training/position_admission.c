/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/position_admission.c
 *
 * PURPOSE:
 *   Guard real decoder loading and inference with a separately declared,
 *   checkpoint-bound positional context. No alternative neural runtime.
 *
 * ARCHITECTURE:
 *   This thin, Framework-owned admission layer composes the existing native
 *   checkpoint codec, SHA-256 service, position contract, canonical generator,
 *   growing-prefix KV generator and paired performance service.
 *   Failure-atomic publication prevents invalid provenance from being
 *   represented as completed generation or valid performance evidence.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/position_admission.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* One digest implementation belongs to the existing base service; never
 * duplicate SHA-256, checkpoint layouts or signature handling here. */
static UmiStatus CheckOriginalCheckpointDigest(
    const uint8_t *bytes, size_t length,
    const UmiAiTrainingPositionContract *record)
{
    UmiSha256 state;
    uint8_t observed[UMI_SHA256_BYTES] = {0};
    UmiStatus status;
    if (bytes == NULL || record == NULL || length == 0U ||
        length != (size_t)record->checkpointBytes ||
        length > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_INVALID_STATE;
    UmiSha256Init(&state);
    status = UmiSha256Update(&state, bytes, length);
    if (status != UMI_STATUS_OK) return status;
    status = UmiSha256Final(&state, observed);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(observed, record->checkpointSha256, sizeof(observed)) != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPositionBoundCheckpointDecode(
    const uint8_t *checkpointBytes, size_t checkpointLength,
    const uint8_t *sidecarBytes, size_t sidecarLength,
    UmiAiTrainingPositionContext requestedContext,
    UmiAiTrainingDecoder **outModel)
{
    UmiAiTrainingPositionContract parsed = {0};
    UmiAiTrainingDecoder *stagedModel = NULL;
    UmiStatus status;

    if (checkpointBytes == NULL || sidecarBytes == NULL || outModel == NULL ||
        checkpointLength == 0U ||
        checkpointLength > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES ||
        sidecarLength != UMI_AI_POSITION_CONTRACT_RECORD_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* The strict sidecar parser checks format, checksum, allowed position
     * strategy, decoder shape and approved rollover policy. It allocates no
     * model and performs no file access. */
    status = UmiAiTrainingPositionContractDecode(
        sidecarBytes, sidecarLength, &parsed);
    if (status != UMI_STATUS_OK) return status;
    if (parsed.generationContext != (uint32_t)requestedContext)
        return UMI_STATUS_INVALID_STATE;

    /* Reject wrong bytes BEFORE allocating and parsing a decoder. This is a
     * compatibility check, not trust in an untrusted publisher's digest. */
    status = CheckOriginalCheckpointDigest(checkpointBytes,
                                           checkpointLength, &parsed);
    if (status != UMI_STATUS_OK) return status;

    status = UmiAiTrainingDecoderCheckpointDecode(checkpointBytes,
                                                  checkpointLength,
                                                  &stagedModel);
    if (status != UMI_STATUS_OK) return status;

    /* Round-trip the real decoder through the established checkpoint codec
     * so a shape-consistent sidecar cannot conceal a different loaded model. */
    status = UmiAiTrainingPositionContractVerify(stagedModel, &parsed,
                                                  requestedContext);
    if (status != UMI_STATUS_OK) {
        UmiAiTrainingDecoderDestroy(stagedModel);
        return status;
    }
    *outModel = stagedModel;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPositionBoundGenerate(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingPositionContract *approvedContract,
    UmiAiTrainingPositionContext requestedContext,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount,
    UmiAiTrainingPositionGenerationEvidence *outEvidence)
{
    size_t stagedTokens[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t stagedCount = 0U;
    UmiAiTrainingPositionGenerationEvidence stagedEvidence = {0};
    UmiStatus status;

    if (model == NULL || approvedContract == NULL || policy == NULL ||
        outTokenIds == NULL || outCount == NULL || outEvidence == NULL ||
        (promptTokenCount != 0U && promptTokenIds == NULL) ||
        generatedTokenCount == 0U ||
        generatedTokenCount > UMI_AI_GENERATION_MAX_NEW_TOKENS ||
        (void *)outTokenIds == (void *)outCount ||
        (void *)outTokenIds == (void *)outEvidence ||
        (void *)outCount == (void *)outEvidence)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < generatedTokenCount) return UMI_STATUS_CAPACITY_EXCEEDED;

    status = UmiAiTrainingPositionContractVerify(model, approvedContract,
                                                  requestedContext);
    if (status != UMI_STATUS_OK) return status;

    switch (requestedContext) {
    case UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED:
        /* Canonical generation is the ONLY authority for this historic
         * context policy. It intentionally supplies no KV work counters. */
        status = UmiAiTrainingDecoderGenerateNucleus(model,
            promptTokenIds, promptTokenCount, policy, generatedTokenCount,
            seed, stagedTokens, generatedTokenCount, &stagedCount);
        break;
    case UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED:
        status = UmiAiTrainingDecoderGeneratePrefixNucleus(model,
            promptTokenIds, promptTokenCount, policy, generatedTokenCount,
            seed, stagedTokens, generatedTokenCount, &stagedCount,
            &stagedEvidence.cache);
        if (status == UMI_STATUS_OK) stagedEvidence.cacheWorkAvailable = 1;
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (status != UMI_STATUS_OK) return status;
    if (stagedCount != generatedTokenCount) return UMI_STATUS_INVALID_STATE;

    /* A decoder can become stale between operations if callers violate the
     * serialisation contract. Never publish data after identity drifts. */
    status = UmiAiTrainingPositionContractVerify(model, approvedContract,
                                                  requestedContext);
    if (status != UMI_STATUS_OK) return status;

    stagedEvidence.admittedContext = requestedContext;
    stagedEvidence.generatedTokens = generatedTokenCount;
    memcpy(stagedEvidence.checkpointSha256,
           approvedContract->checkpointSha256,
           sizeof(stagedEvidence.checkpointSha256));
    memcpy(outTokenIds, stagedTokens, generatedTokenCount * sizeof(size_t));
    *outCount = generatedTokenCount;
    *outEvidence = stagedEvidence;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPositionBoundBenchmark(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingPositionContract *approvedContract,
    UmiAiTrainingPositionContext requestedContext,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    const UmiAiTrainingPrefixBenchmarkPlan *plan,
    UmiAiTrainingPrefixBenchmarkResult *outResult)
{
    UmiAiTrainingPrefixBenchmarkResult stagedResult = {0};
    UmiStatus status;
    if (model == NULL || approvedContract == NULL ||
        promptTokenIds == NULL || policy == NULL || plan == NULL ||
        outResult == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* A right-padded prefix reference is not the left-padded historical
     * generator. Admitting that comparison would conflate distinct semantics. */
    if (requestedContext != UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED)
        return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingPositionContractVerify(model, approvedContract,
                                                  requestedContext);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingPrefixBenchmarkRun(model, promptTokenIds,
                promptTokenCount, policy, plan, &stagedResult);
    if (status != UMI_STATUS_OK) return status;
    if (!stagedResult.checkpointUnchanged)
        return UMI_STATUS_INVALID_STATE;
    status = UmiAiTrainingPositionContractVerify(model, approvedContract,
                                                  requestedContext);
    if (status != UMI_STATUS_OK) return status;
    *outResult = stagedResult;
    return UMI_STATUS_OK;
}
