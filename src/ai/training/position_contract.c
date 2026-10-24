/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/position_contract.c
 *
 * PURPOSE:
 *   Provide failure-atomic, byte-exact positional-encoding compatibility
 *   admission for native decoder checkpoints and generation policies.
 *
 * DESIGN:
 *   This module does not implement alternative positional mathematics. It
 *   binds the CURRENT learned-absolute decoder to a canonical checkpoint
 *   digest and one explicitly named inference context policy. The existing
 *   checkpoint codec and Framework SHA-256 remain the only data authorities.
 *   The separate 128-byte sidecar can be inspected without loading a model.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/position_contract.h"
#include "umicom/ai/training/kv_cache.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* This portable sidecar is additive; it is NOT a modification of the decoder
 * checkpoint. No C structure is written raw and all reserved bytes are zero.
 *  0..7  magic  "UMIPOS01"
 *  8..11  format version (uint32 little-endian)
 * 12..15  positional strategy
 * 16..19  generation-context policy
 * 20..23  rollover behaviour
 * 24..43  vocabulary, context, channels, hidden, heads (five uint32)
 * 44..47  checkpoint byte count (uint32)
 * 48..55  initial seed (uint64)
 * 56..63  completed optimiser steps (uint64)
 * 64..95  SHA-256 of original checkpoint bytes
 * 96..127 SHA-256 of the first 96 sidecar bytes (integrity, NOT signing)
 */
static const uint8_t POSITION_MAGIC[8] = {
    (uint8_t)'U', (uint8_t)'M', (uint8_t)'I', (uint8_t)'P',
    (uint8_t)'O', (uint8_t)'S', (uint8_t)'0', (uint8_t)'1'
};

static void Write32(uint8_t *out, uint32_t value)
{
    for (unsigned byte = 0U; byte < 4U; ++byte)
        out[byte] = (uint8_t)(value >> (byte * 8U));
}

static void Write64(uint8_t *out, uint64_t value)
{
    for (unsigned byte = 0U; byte < 8U; ++byte)
        out[byte] = (uint8_t)(value >> (byte * 8U));
}

static uint32_t Read32(const uint8_t *bytes)
{
    uint32_t value = UINT32_C(0);
    for (unsigned byte = 0U; byte < 4U; ++byte)
        value |= (uint32_t)bytes[byte] << (byte * 8U);
    return value;
}

static uint64_t Read64(const uint8_t *bytes)
{
    uint64_t value = UINT64_C(0);
    for (unsigned byte = 0U; byte < 8U; ++byte)
        value |= (uint64_t)bytes[byte] << (byte * 8U);
    return value;
}

/* Hashing is delegated to the existing Framework native SHA-256 service.
 * A digest is a useful identity comparison but not proof of who created it. */
static UmiStatus HashBytes(const void *bytes, size_t count,
                           uint8_t outHash[UMI_SHA256_BYTES])
{
    UmiSha256 sha;
    UmiStatus status;
    UmiSha256Init(&sha);
    status = UmiSha256Update(&sha, bytes, count);
    if (status != UMI_STATUS_OK) return status;
    return UmiSha256Final(&sha, outHash);
}

static int SameConfig(const UmiAiTrainingDecoderConfig *left,
                      const UmiAiTrainingDecoderConfig *right)
{
    return left->vocabularySize == right->vocabularySize &&
           left->contextLength == right->contextLength &&
           left->channels == right->channels &&
           left->hiddenChannels == right->hiddenChannels &&
           left->attentionHeads == right->attentionHeads &&
           left->initialSeed == right->initialSeed;
}

static int ContextSupported(UmiAiTrainingPositionContext context)
{
    return context == UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED ||
           context == UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED;
}

/* The existing Framework KV planner checks real parameter shapes and channel
 * divisibility. This prevents a second, divergent decoder-config validator.
 * Unknown encodings are rejected rather than converted implicitly. */
static UmiStatus ValidateContract(
    const UmiAiTrainingPositionContract *contract)
{
    size_t cacheBytes = 0U;
    if (contract == NULL ||
        contract->formatVersion != UMI_AI_POSITION_CONTRACT_FORMAT ||
        contract->positionEncoding !=
            (uint32_t)UMI_AI_POSITION_ENCODING_LEARNED_ABSOLUTE ||
        !ContextSupported((UmiAiTrainingPositionContext)
            contract->generationContext) ||
        contract->rolloverBehaviour !=
            (uint32_t)UMI_AI_POSITION_ROLLOVER_REBUILD ||
        contract->checkpointBytes == 0U ||
        contract->checkpointBytes > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;

    if (UmiAiTrainingKvCacheAllocationForConfig(
            &contract->decoderConfig, &cacheBytes) != UMI_STATUS_OK ||
        cacheBytes == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPositionContractBuild(
    const UmiAiTrainingDecoder *model,
    UmiAiTrainingPositionContext context,
    UmiAiTrainingPositionContract *outContract)
{
    UmiAiTrainingPositionContract proposed = {0};
    UmiAiTrainingDecoderConfig before = {0}, after = {0};
    uint64_t stepsBefore = 0U, stepsAfter = 0U;
    size_t checkpointSize = 0U, written = 0U;
    uint8_t *checkpoint = NULL;
    UmiStatus status;

    if (model == NULL || outContract == NULL || !ContextSupported(context))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(model, &before, &stepsBefore);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize);
    if (status != UMI_STATUS_OK) return status;
    if (checkpointSize == 0U ||
        checkpointSize > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES ||
        checkpointSize > UINT32_MAX)
        return UMI_STATUS_INVALID_STATE;

    proposed.formatVersion = UMI_AI_POSITION_CONTRACT_FORMAT;
    proposed.positionEncoding =
        (uint32_t)UMI_AI_POSITION_ENCODING_LEARNED_ABSOLUTE;
    proposed.generationContext = (uint32_t)context;
    proposed.rolloverBehaviour =
        (uint32_t)UMI_AI_POSITION_ROLLOVER_REBUILD;
    proposed.decoderConfig = before;
    proposed.completedTrainingSteps = stepsBefore;
    proposed.checkpointBytes = (uint32_t)checkpointSize;
    status = ValidateContract(&proposed);
    if (status != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;

    checkpoint = malloc(checkpointSize);
    if (checkpoint == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingDecoderCheckpointEncode(model, checkpoint,
                                                  checkpointSize, &written);
    if (status == UMI_STATUS_OK && written != checkpointSize)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = HashBytes(checkpoint, checkpointSize,
                           proposed.checkpointSha256);
    free(checkpoint);
    if (status != UMI_STATUS_OK) return status;

    /* Catch accidental model replacement or training interleaved with a
     * supposedly read-only operation. Concurrent use is still forbidden. */
    status = UmiAiTrainingDecoderInspect(model, &after, &stepsAfter);
    if (status != UMI_STATUS_OK) return status;
    if (!SameConfig(&before, &after) || stepsAfter != stepsBefore)
        return UMI_STATUS_INVALID_STATE;

    *outContract = proposed;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPositionContractEncode(
    const UmiAiTrainingPositionContract *contract,
    uint8_t *outBytes, size_t capacity, size_t *outWritten)
{
    uint8_t encoded[UMI_AI_POSITION_CONTRACT_RECORD_BYTES] = {0};
    UmiStatus status;
    if (contract == NULL || outBytes == NULL || outWritten == NULL ||
        (const void *)outBytes == (const void *)outWritten)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < UMI_AI_POSITION_CONTRACT_RECORD_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = ValidateContract(contract);
    if (status != UMI_STATUS_OK) return status;

    memcpy(encoded, POSITION_MAGIC, sizeof(POSITION_MAGIC));
    Write32(encoded + 8U, contract->formatVersion);
    Write32(encoded + 12U, contract->positionEncoding);
    Write32(encoded + 16U, contract->generationContext);
    Write32(encoded + 20U, contract->rolloverBehaviour);
    Write32(encoded + 24U, contract->decoderConfig.vocabularySize);
    Write32(encoded + 28U, contract->decoderConfig.contextLength);
    Write32(encoded + 32U, contract->decoderConfig.channels);
    Write32(encoded + 36U, contract->decoderConfig.hiddenChannels);
    Write32(encoded + 40U, contract->decoderConfig.attentionHeads);
    Write32(encoded + 44U, contract->checkpointBytes);
    Write64(encoded + 48U, contract->decoderConfig.initialSeed);
    Write64(encoded + 56U, contract->completedTrainingSteps);
    memcpy(encoded + 64U, contract->checkpointSha256, UMI_SHA256_BYTES);
    status = HashBytes(encoded, 96U, encoded + 96U);
    if (status != UMI_STATUS_OK) return status;

    /* The fully staged array is published exactly once, followed by count.
     * outBytes and outWritten must address nonoverlapping objects. */
    memcpy(outBytes, encoded, sizeof(encoded));
    *outWritten = sizeof(encoded);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPositionContractDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingPositionContract *outContract)
{
    UmiAiTrainingPositionContract proposed = {0};
    uint8_t digest[UMI_SHA256_BYTES] = {0};
    UmiStatus status;
    if (bytes == NULL || outContract == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (byteCount != UMI_AI_POSITION_CONTRACT_RECORD_BYTES ||
        memcmp(bytes, POSITION_MAGIC, sizeof(POSITION_MAGIC)) != 0)
        return UMI_STATUS_PARSE_ERROR;

    status = HashBytes(bytes, 96U, digest);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(digest, bytes + 96U, UMI_SHA256_BYTES) != 0)
        return UMI_STATUS_PARSE_ERROR;

    proposed.formatVersion = Read32(bytes + 8U);
    proposed.positionEncoding = Read32(bytes + 12U);
    proposed.generationContext = Read32(bytes + 16U);
    proposed.rolloverBehaviour = Read32(bytes + 20U);
    proposed.decoderConfig.vocabularySize = Read32(bytes + 24U);
    proposed.decoderConfig.contextLength = Read32(bytes + 28U);
    proposed.decoderConfig.channels = Read32(bytes + 32U);
    proposed.decoderConfig.hiddenChannels = Read32(bytes + 36U);
    proposed.decoderConfig.attentionHeads = Read32(bytes + 40U);
    proposed.checkpointBytes = Read32(bytes + 44U);
    proposed.decoderConfig.initialSeed = Read64(bytes + 48U);
    proposed.completedTrainingSteps = Read64(bytes + 56U);
    memcpy(proposed.checkpointSha256, bytes + 64U, UMI_SHA256_BYTES);
    if (ValidateContract(&proposed) != UMI_STATUS_OK)
        return UMI_STATUS_PARSE_ERROR;

    *outContract = proposed;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPositionContractVerify(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingPositionContract *expected,
    UmiAiTrainingPositionContext requestedContext)
{
    UmiAiTrainingPositionContract observed = {0};
    UmiStatus status;
    if (model == NULL || expected == NULL ||
        !ContextSupported(requestedContext))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (ValidateContract(expected) != UMI_STATUS_OK ||
        expected->generationContext != (uint32_t)requestedContext)
        return UMI_STATUS_INVALID_STATE;

    status = UmiAiTrainingPositionContractBuild(model, requestedContext,
                                                &observed);
    if (status != UMI_STATUS_OK) return status;
    if (expected->formatVersion != observed.formatVersion ||
        expected->positionEncoding != observed.positionEncoding ||
        expected->generationContext != observed.generationContext ||
        expected->rolloverBehaviour != observed.rolloverBehaviour ||
        !SameConfig(&expected->decoderConfig, &observed.decoderConfig) ||
        expected->completedTrainingSteps != observed.completedTrainingSteps ||
        expected->checkpointBytes != observed.checkpointBytes ||
        memcmp(expected->checkpointSha256, observed.checkpointSha256,
               UMI_SHA256_BYTES) != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
