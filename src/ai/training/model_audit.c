/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/model_audit.c
 *
 * PURPOSE:
 *   Calculate reproducible model, dataset and experiment identities and
 *   independently count exact next-token predictions on held-out samples.
 *
 * ENGINEERING:
 *   Reuses the checkpoint codec, dataset manifest and canonical decoder
 *   evaluation. Hashes use explicit byte order, not native struct padding,
 *   endianness or pointer values. The model remains caller-owned.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_audit.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/tape.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define UMI_AI_AUDIT_FNV_OFFSET UINT64_C(14695981039346656037)
#define UMI_AI_AUDIT_FNV_PRIME UINT64_C(1099511628211)

static void MixByte(uint64_t *fingerprint, uint8_t value)
{
    *fingerprint ^= (uint64_t)value;
    *fingerprint *= UMI_AI_AUDIT_FNV_PRIME;
}

/* Explicit little-endian encoding makes these fingerprints independent of
 * host byte order and of padding in native C structures. */
static void Mix32(uint64_t *fingerprint, uint32_t value)
{
    for (unsigned shift = 0U; shift < 32U; shift += 8U)
        MixByte(fingerprint, (uint8_t)(value >> shift));
}

static void Mix64(uint64_t *fingerprint, uint64_t value)
{
    for (unsigned shift = 0U; shift < 64U; shift += 8U)
        MixByte(fingerprint, (uint8_t)(value >> shift));
}

static void MixTag(uint64_t *fingerprint, const char *tag)
{
    while (*tag != '\0') MixByte(fingerprint, (uint8_t)*tag++);
    MixByte(fingerprint, 0U);
}

static UmiStatus FingerprintCheckpoint(
    const UmiAiTrainingDecoder *model, uint64_t *outFingerprint)
{
    uint8_t *bytes;
    size_t length = 0U;
    size_t written = 0U;
    uint64_t hash = UMI_AI_AUDIT_FNV_OFFSET;
    UmiStatus status;

    if (model == NULL || outFingerprint == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderCheckpointSize(model, &length);
    if (status != UMI_STATUS_OK) return status;
    if (length == 0U || length > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    bytes = malloc(length);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingDecoderCheckpointEncode(model, bytes,
                                                 length, &written);
    if (status == UMI_STATUS_OK && written != length)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        MixTag(&hash, "umicom.decoder.checkpoint");
        Mix64(&hash, (uint64_t)length);
        for (size_t index = 0U; index < length; ++index)
            MixByte(&hash, bytes[index]);
        *outFingerprint = hash;
    }
    free(bytes);
    return status;
}

static UmiStatus FingerprintDataset(
    const UmiAiTrainingDataset *dataset,
    UmiAiTrainingDatasetSummary *outSummary,
    uint64_t *outFingerprint)
{
    UmiAiTrainingDatasetSummary summary;
    uint64_t hash = UMI_AI_AUDIT_FNV_OFFSET;
    UmiStatus status;

    if (dataset == NULL || outSummary == NULL || outFingerprint == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDatasetInspect(dataset, &summary);
    if (status != UMI_STATUS_OK) return status;
    if (summary.documentCount > UMI_AI_DATASET_MAX_FILES ||
        summary.trainingDocuments == 0U ||
        summary.evaluationDocuments == 0U)
        return UMI_STATUS_INVALID_STATE;

    MixTag(&hash, "umicom.training.dataset");
    Mix64(&hash, (uint64_t)summary.documentCount);
    Mix64(&hash, (uint64_t)summary.trainingDocuments);
    Mix64(&hash, (uint64_t)summary.evaluationDocuments);
    Mix64(&hash, (uint64_t)summary.trainingBytes);
    Mix64(&hash, (uint64_t)summary.evaluationBytes);

    for (size_t index = 0U; index < summary.documentCount; ++index) {
        UmiAiTrainingDocumentInfo document;
        size_t length = 0U;
        status = UmiAiTrainingDatasetDocument(dataset, index, &document);
        if (status != UMI_STATUS_OK) return status;
        /* Never read an unterminated input beyond the public path capacity. */
        while (length < UMI_AI_DATASET_MAX_PATH &&
               document.relativePath[length] != '\0') ++length;
        if (length == 0U || length == UMI_AI_DATASET_MAX_PATH ||
            (document.partition != UMI_AI_TRAINING_PARTITION_TRAIN &&
             document.partition != UMI_AI_TRAINING_PARTITION_EVALUATE))
            return UMI_STATUS_INVALID_STATE;
        Mix64(&hash, (uint64_t)index);
        Mix32(&hash, (uint32_t)document.partition);
        Mix64(&hash, (uint64_t)length);
        for (size_t character = 0U; character < length; ++character)
            MixByte(&hash, (uint8_t)document.relativePath[character]);
        Mix64(&hash, (uint64_t)document.byteCount);
        Mix64(&hash, document.fingerprint);
    }
    *outSummary = summary;
    *outFingerprint = hash;
    return UMI_STATUS_OK;
}

static uint64_t FingerprintPlan(
    const UmiAiTrainingExperimentConfig *config)
{
    uint64_t hash = UMI_AI_AUDIT_FNV_OFFSET;
    uint32_t rateBits = 0U;
    memcpy(&rateBits, &config->learningRate, sizeof(rateBits));
    MixTag(&hash, "umicom.training.experiment");
    Mix32(&hash, config->decoder.vocabularySize);
    Mix32(&hash, config->decoder.contextLength);
    Mix32(&hash, config->decoder.channels);
    Mix32(&hash, config->decoder.hiddenChannels);
    Mix32(&hash, config->decoder.attentionHeads);
    Mix64(&hash, config->decoder.initialSeed);
    Mix32(&hash, config->steps);
    Mix32(&hash, config->evalInterval);
    Mix32(&hash, config->evalBatches);
    Mix32(&hash, config->batchSequences);
    Mix32(&hash, rateBits);
    Mix64(&hash, config->trainSeed);
    Mix64(&hash, config->evalSeed);
    return hash;
}

static uint64_t FingerprintCombined(uint64_t checkpoint, uint64_t dataset,
                                    uint64_t plan, uint64_t steps)
{
    uint64_t hash = UMI_AI_AUDIT_FNV_OFFSET;
    MixTag(&hash, "umicom.training.audit");
    Mix64(&hash, checkpoint);
    Mix64(&hash, dataset);
    Mix64(&hash, plan);
    Mix64(&hash, steps);
    return hash;
}

static bool SameConfig(const UmiAiTrainingDecoderConfig *left,
                       const UmiAiTrainingDecoderConfig *right)
{
    return left->vocabularySize == right->vocabularySize &&
           left->contextLength == right->contextLength &&
           left->channels == right->channels &&
           left->hiddenChannels == right->hiddenChannels &&
           left->attentionHeads == right->attentionHeads &&
           left->initialSeed == right->initialSeed;
}

static UmiStatus FingerprintIdentities(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    UmiAiTrainingModelAudit *candidate)
{
    UmiAiTrainingDecoderConfig actual;
    uint64_t steps = 0U;
    UmiStatus status = UmiAiTrainingExperimentValidate(config);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDecoderInspect(model, &actual, &steps);
    if (status != UMI_STATUS_OK) return status;
    status = FingerprintDataset(dataset, &candidate->dataset,
                                &candidate->datasetFingerprint);
    if (status != UMI_STATUS_OK) return status;
    status = FingerprintCheckpoint(model, &candidate->modelFingerprint);
    if (status != UMI_STATUS_OK) return status;
    candidate->planFingerprint = FingerprintPlan(config);
    candidate->completedSteps = steps;
    candidate->combinedFingerprint = FingerprintCombined(
        candidate->modelFingerprint, candidate->datasetFingerprint,
        candidate->planFingerprint, steps);
    /* A report may be compared against a different plan by Verify, but the
     * complete evaluator must never silently measure a mismatched model. */
    if (!SameConfig(&actual, &config->decoder) ||
        steps > (uint64_t)config->steps)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingModelAuditEvaluate(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    UmiAiTrainingModelAudit *outAudit)
{
    UmiAiTrainingModelAudit candidate = {0};
    UmiAiTrainingModelAudit after = {0};
    size_t inputs[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    size_t targets[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    float logits[UMI_AI_TRAINING_MAX_ELEMENTS];
    size_t tokens;
    UmiStatus status;

    if (model == NULL || dataset == NULL || config == NULL || outAudit == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = FingerprintIdentities(model, dataset, config, &candidate);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingExperimentEvaluate(model, dataset, config,
             &candidate.heldOutCrossEntropy, &candidate.heldOutPerplexity);
    if (status != UMI_STATUS_OK) return status;
    tokens = (size_t)config->decoder.contextLength *
             (size_t)config->batchSequences;
    if (tokens > UMI_AI_TRAINING_MAX_ELEMENTS /
                 (size_t)config->decoder.vocabularySize)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    for (uint32_t sample = 0U; sample < config->evalBatches; ++sample) {
        status = UmiAiTrainingDatasetBatch(dataset,
            UMI_AI_TRAINING_PARTITION_EVALUATE,
            (size_t)config->decoder.contextLength,
            (size_t)config->batchSequences, config->evalSeed,
            (uint64_t)sample, inputs, targets, tokens);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingDecoderLogits(model, inputs, tokens, logits,
                                    tokens * (size_t)config->decoder.vocabularySize);
        if (status != UMI_STATUS_OK) return status;
        for (size_t row = 0U; row < tokens; ++row) {
            size_t target = targets[row];
            size_t rank = 0U;
            size_t vocabulary = (size_t)config->decoder.vocabularySize;
            if (target >= vocabulary) return UMI_STATUS_INVALID_STATE;
            float targetScore = logits[row * vocabulary + target];
            if (!isfinite(targetScore)) return UMI_STATUS_INVALID_STATE;
            for (size_t column = 0U; column < vocabulary; ++column) {
                float score = logits[row * vocabulary + column];
                if (!isfinite(score)) return UMI_STATUS_INVALID_STATE;
                if (score > targetScore ||
                    (score == targetScore && column < target)) ++rank;
            }
            if (rank == 0U) ++candidate.topOneCorrect;
            if (rank < 5U) ++candidate.topFiveCorrect;
            ++candidate.evaluatedTokens;
        }
    }
    status = FingerprintIdentities(model, dataset, config, &after);
    if (status != UMI_STATUS_OK) return status;
    if (candidate.combinedFingerprint != after.combinedFingerprint)
        return UMI_STATUS_INVALID_STATE;
    *outAudit = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingModelAuditVerify(
    const UmiAiTrainingModelAudit *reference,
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    int *outMatches)
{
    UmiAiTrainingModelAudit current = {0};
    UmiStatus status;
    if (reference == NULL || model == NULL || dataset == NULL ||
        config == NULL || outMatches == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = FingerprintIdentities(model, dataset, config, &current);
    if (status == UMI_STATUS_INVALID_STATE) {
        /* Shape/step mismatch is a valid identity comparison, not a match. */
        *outMatches = 0;
        return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;
    *outMatches =
        reference->modelFingerprint == current.modelFingerprint &&
        reference->datasetFingerprint == current.datasetFingerprint &&
        reference->planFingerprint == current.planFingerprint &&
        reference->completedSteps == current.completedSteps &&
        reference->combinedFingerprint == current.combinedFingerprint;
    return UMI_STATUS_OK;
}
