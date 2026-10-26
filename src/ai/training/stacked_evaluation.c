/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_evaluation.c
 * PURPOSE: Token-weighted held-out assessment via canonical decoder operations.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_evaluation.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus AppendLe64(UmiSha256 *hash, uint64_t value)
{
    uint8_t bytes[8];
    for (size_t i = 0U; i < sizeof(bytes); ++i)
        bytes[i] = (uint8_t)(value >> (8U * i));
    return UmiSha256Update(hash, bytes, sizeof(bytes));
}

static UmiStatus ValidateCases(
    const UmiAiTrainingStackedDecoderConfig *config,
    const UmiAiTrainingStackedEvaluationCase *examples, size_t count)
{
    if (config == NULL || examples == NULL || count == 0U ||
        count > UMI_AI_STACKED_EVALUATION_MAX_CASES ||
        config->vocabularySize < 5U || config->contextLength == 0U ||
        config->contextLength > UMI_AI_STACKED_MAX_CONTEXT)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < count; ++i) {
        const UmiAiTrainingStackedEvaluationCase *item = &examples[i];
        size_t selected = 0U;
        if (item->inputIds == NULL || item->targetIds == NULL ||
            item->lossMask == NULL ||
            item->tokenCount != (size_t)config->contextLength)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t t = 0U; t < item->tokenCount; ++t) {
            if (item->inputIds[t] >= (size_t)config->vocabularySize ||
                item->lossMask[t] > 1U)
                return UMI_STATUS_INVALID_ARGUMENT;
            if (item->lossMask[t] != 0U) {
                if (item->targetIds[t] >= (size_t)config->vocabularySize)
                    return UMI_STATUS_INVALID_ARGUMENT;
                ++selected;
            }
        }
        if (selected == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus HashCases(
    const UmiAiTrainingStackedEvaluationCase *examples, size_t count,
    uint8_t outDigest[UMI_SHA256_BYTES])
{
    static const char domain[] = "umicom.stacked.heldout.cases\n";
    UmiSha256 hash;
    UmiStatus status;
    UmiSha256Init(&hash);
    status = UmiSha256Update(&hash, domain, sizeof(domain) - 1U);
    if (status == UMI_STATUS_OK) status = AppendLe64(&hash, (uint64_t)count);
    for (size_t i = 0U; i < count && status == UMI_STATUS_OK; ++i) {
        const UmiAiTrainingStackedEvaluationCase *item = &examples[i];
        status = UmiSha256Update(&hash, item->originalSourceSha256,
                                 UMI_SHA256_BYTES);
        if (status == UMI_STATUS_OK)
            status = AppendLe64(&hash, (uint64_t)item->tokenCount);
        for (size_t t = 0U; t < item->tokenCount && status == UMI_STATUS_OK; ++t) {
            status = AppendLe64(&hash, (uint64_t)item->inputIds[t]);
            if (status == UMI_STATUS_OK)
                status = AppendLe64(&hash, (uint64_t)item->targetIds[t]);
            if (status == UMI_STATUS_OK)
                status = UmiSha256Update(&hash, &item->lossMask[t], 1U);
        }
    }
    return status == UMI_STATUS_OK
        ? UmiSha256Final(&hash, outDigest) : status;
}

UmiStatus UmiAiTrainingStackedEvaluationRun(
    UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedEvaluationCase *examples,
    size_t exampleCount, UmiAiTrainingStackedEvaluationReport *outReport)
{
    UmiAiTrainingStackedEvaluationReport result = {0};
    UmiAiTrainingStackedDecoderInfo info = {0};
    float *logits = NULL;
    size_t elements = 0U, vocabulary;
    double weightedLoss = 0.0;
    UmiStatus status;
    if (model == NULL || outReport == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedDecoderInspect(model, &info);
    if (status != UMI_STATUS_OK) return status;
    status = ValidateCases(&info.config, examples, exampleCount);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedModelIdentityCapture(
        model, &result.modelIdentity);
    if (status != UMI_STATUS_OK) return status;
    status = HashCases(examples, exampleCount, result.orderedCaseSha256);
    if (status != UMI_STATUS_OK) return status;
    vocabulary = (size_t)info.config.vocabularySize;
    elements = (size_t)info.config.contextLength * vocabulary;
    logits = malloc(elements * sizeof(*logits));
    if (logits == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < exampleCount; ++i) {
        const UmiAiTrainingStackedEvaluationCase *item = &examples[i];
        float caseLoss = 0.0F;
        size_t selected = 0U;
        status = UmiAiTrainingStackedDecoderEvaluateSelected(
            model, item->inputIds, item->targetIds,
            item->lossMask, item->tokenCount, &caseLoss);
        if (status != UMI_STATUS_OK) break;
        if (!isfinite(caseLoss) || caseLoss < 0.0F) {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        status = UmiAiTrainingStackedDecoderLogits(
            model, item->inputIds, item->tokenCount, logits, elements);
        if (status != UMI_STATUS_OK) break;
        for (size_t t = 0U; t < item->tokenCount && status == UMI_STATUS_OK; ++t) {
            if (item->lossMask[t] == 0U) continue;
            const float *row = logits + t * vocabulary;
            size_t best = 0U;
            for (size_t token = 0U; token < vocabulary; ++token) {
                if (!isfinite(row[token])) {
                    status = UMI_STATUS_INVALID_STATE;
                    break;
                }
                if (row[token] > row[best]) best = token;
            }
            if (status != UMI_STATUS_OK) break;
            if (best == item->targetIds[t]) ++result.correctTopOneTargets;
            ++selected;
        }
        if (status != UMI_STATUS_OK) break;
        result.supervisedTargets += selected;
        weightedLoss += (double)caseLoss * (double)selected;
        if (!isfinite(weightedLoss)) {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
    }
    free(logits);
    if (status != UMI_STATUS_OK) return status;
    if (result.supervisedTargets == 0U) return UMI_STATUS_INVALID_STATE;
    result.exampleCount = exampleCount;
    result.meanSelectedCrossEntropy =
        weightedLoss / (double)result.supervisedTargets;
    result.topOneAccuracy =
        (double)result.correctTopOneTargets / (double)result.supervisedTargets;
    result.perplexity = exp(result.meanSelectedCrossEntropy);
    if (isfinite(result.perplexity))
        result.perplexityAvailable = 1;
    else
        result.perplexity = 0.0;
    status = UmiAiTrainingStackedModelIdentityVerify(
        model, &result.modelIdentity);
    if (status != UMI_STATUS_OK) return status;
    *outReport = result;
    return UMI_STATUS_OK;
}
