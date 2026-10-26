/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_experiment.c
 * PURPOSE: Deterministic multi-epoch training orchestration using the
 *          Framework stacked decoder, source partitions and local RNG.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_experiment.h"
#include "umicom/ai/training/initialisation.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

static int NonzeroDigest(const uint8_t digest[UMI_SHA256_BYTES])
{
    uint8_t any = 0U;
    for (size_t i = 0U; i < UMI_SHA256_BYTES; ++i) any |= digest[i];
    return any != 0U;
}

static size_t SelectedCount(const UmiAiTrainingStackedExample *example)
{
    size_t selected = 0U;
    for (size_t i = 0U; i < example->tokenCount; ++i)
        selected += (size_t)(example->lossMask[i] != 0U);
    return selected;
}

static int SameExample(const UmiAiTrainingStackedExample *left,
                       const UmiAiTrainingStackedExample *right)
{
    if (left->tokenCount != right->tokenCount) return 0;
    for (size_t i = 0U; i < left->tokenCount; ++i) {
        if (left->inputIds[i] != right->inputIds[i] ||
            left->lossMask[i] != right->lossMask[i] ||
            (left->lossMask[i] == 1U &&
             left->targetIds[i] != right->targetIds[i])) return 0;
    }
    return 1;
}

UmiStatus UmiAiTrainingStackedExperimentValidate(
    const UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedExample *examples, size_t count,
    const UmiAiTrainingStackedExperimentPlan *plan)
{
    UmiAiTrainingStackedDecoderInfo info = {0};
    size_t train = 0U, eval = 0U;
    if (model == NULL || examples == NULL || plan == NULL ||
        count < 2U || count > UMI_AI_STACKED_MAX_EXAMPLES ||
        plan->epochs == 0U || plan->epochs > UMI_AI_STACKED_MAX_EPOCHS ||
        !isfinite(plan->learningRate) || plan->learningRate <= 0.0F ||
        UmiAiTrainingStackedDecoderInspect(model, &info) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < count; ++i) {
        const UmiAiTrainingStackedExample *item = &examples[i];
        size_t selected = 0U;
        if (item->inputIds == NULL || item->targetIds == NULL ||
            item->lossMask == NULL ||
            item->tokenCount != (size_t)info.config.contextLength ||
            !NonzeroDigest(item->originalSourceSha256) ||
            (item->partition != UMI_AI_TRAINING_PARTITION_TRAIN &&
             item->partition != UMI_AI_TRAINING_PARTITION_EVALUATE))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < item->tokenCount; ++j) {
            if (item->inputIds[j] >= (size_t)info.config.vocabularySize ||
                item->lossMask[j] > 1U ||
                (item->lossMask[j] == 1U &&
                 item->targetIds[j] >= (size_t)info.config.vocabularySize))
                return UMI_STATUS_INVALID_ARGUMENT;
            selected += (size_t)item->lossMask[j];
        }
        if (selected == 0U) return UMI_STATUS_INVALID_ARGUMENT;
        if (item->partition == UMI_AI_TRAINING_PARTITION_TRAIN) ++train;
        else ++eval;
        for (size_t j = 0U; j < i; ++j) {
            if (memcmp(item->originalSourceSha256,
                       examples[j].originalSourceSha256,
                       UMI_SHA256_BYTES) == 0 &&
                item->partition != examples[j].partition)
                return UMI_STATUS_INVALID_STATE;
            if (SameExample(item, &examples[j]))
                return UMI_STATUS_INVALID_STATE;
        }
    }
    if (train == 0U || eval == 0U ||
        train > (UINT64_MAX - info.completedTrainingSteps) / plan->epochs)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

static UmiStatus Append64(UmiSha256 *state, uint64_t value)
{
    uint8_t bytes[8];
    for (size_t i = 0U; i < 8U; ++i)
        bytes[i] = (uint8_t)(value >> (8U * i));
    return UmiSha256Update(state, bytes, sizeof(bytes));
}

/* Hash the ACTUAL supplied token-window identities and declared partitions.
 * This is not an original-document signature or a substitute for source review. */
static UmiStatus HashWindows(
    const UmiAiTrainingStackedExample *items, size_t count,
    uint8_t result[UMI_SHA256_BYTES])
{
    static const char domain[] = "umicom-stacked-windows\n";
    UmiSha256 state;
    UmiStatus status;
    UmiSha256Init(&state);
    status = UmiSha256Update(&state, domain, sizeof(domain) - 1U);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i) {
        const UmiAiTrainingStackedExample *entry = &items[i];
        status = UmiSha256Update(&state, entry->originalSourceSha256,
                                  UMI_SHA256_BYTES);
        if (status == UMI_STATUS_OK)
            status = Append64(&state, (uint64_t)entry->partition);
        if (status == UMI_STATUS_OK)
            status = Append64(&state, (uint64_t)entry->tokenCount);
        for (size_t j = 0U; status == UMI_STATUS_OK && j < entry->tokenCount;
             ++j) {
            status = Append64(&state, (uint64_t)entry->inputIds[j]);
            if (status == UMI_STATUS_OK)
                status = Append64(&state, entry->lossMask[j] != 0U
                    ? (uint64_t)entry->targetIds[j] : UINT64_C(0));
            if (status == UMI_STATUS_OK)
                status = UmiSha256Update(&state, entry->lossMask + j, 1U);
        }
    }
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&state, result);
    return status;
}

static UmiStatus EvaluatePartition(
    UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedExample *examples, size_t count,
    UmiAiTrainingPartition partition, double *outLoss)
{
    double weighted = 0.0;
    size_t totalTargets = 0U;
    UmiStatus status;
    for (size_t i = 0U; i < count; ++i) {
        if (examples[i].partition != partition) continue;
        float loss = 0.0F;
        const size_t selected = SelectedCount(&examples[i]);
        status = UmiAiTrainingStackedDecoderEvaluateSelected(
            model, examples[i].inputIds, examples[i].targetIds,
            examples[i].lossMask, examples[i].tokenCount, &loss);
        if (status != UMI_STATUS_OK) return status;
        if (!isfinite(loss) || loss < 0.0F)
            return UMI_STATUS_INVALID_STATE;
        weighted += (double)loss * (double)selected;
        totalTargets += selected;
    }
    if (totalTargets == 0U || !isfinite(weighted))
        return UMI_STATUS_INVALID_STATE;
    *outLoss = weighted / (double)totalTargets;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedExperimentRun(
    UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedExample *examples, size_t count,
    const UmiAiTrainingStackedExperimentPlan *plan,
    UmiAiTrainingStackedExperimentReport *outReport)
{
    UmiAiTrainingStackedExperimentReport candidate = {0};
    size_t indices[UMI_AI_STACKED_MAX_EXAMPLES] = {0};
    size_t training = 0U;
    UmiStatus status;
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedExperimentValidate(model, examples,
                                                     count, plan);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < count; ++i) {
        if (examples[i].partition == UMI_AI_TRAINING_PARTITION_TRAIN)
            indices[training++] = i;
        else
            ++candidate.evaluationExamples;
    }
    candidate.trainingExamples = training;
    status = HashWindows(examples, count,
                          candidate.orderedTrainingSourceSha256);
    if (status != UMI_STATUS_OK) return status;
    status = EvaluatePartition(model, examples, count,
                  UMI_AI_TRAINING_PARTITION_EVALUATE,
                  &candidate.heldoutLossBefore);
    if (status != UMI_STATUS_OK) return status;
    for (size_t epoch = 0U; epoch < plan->epochs; ++epoch) {
        UmiAiTrainingRandomState random = {0};
        size_t order[UMI_AI_STACKED_MAX_EXAMPLES] = {0};
        double weighted = 0.0;
        size_t selectedTotal = 0U;
        memcpy(order, indices, training * sizeof(order[0]));
        status = UmiAiTrainingRandomSeed(&random,
            plan->shuffleSeed ^
            (UINT64_C(0x9e3779b97f4a7c15) * (uint64_t)(epoch + 1U)));
        if (status != UMI_STATUS_OK) return status;
        for (size_t remaining = training; remaining > 1U; --remaining) {
            uint64_t sampled = 0U;
            status = UmiAiTrainingRandomIndex(
                &random, (uint64_t)remaining, &sampled);
            if (status != UMI_STATUS_OK) return status;
            size_t swap = order[remaining - 1U];
            order[remaining - 1U] = order[(size_t)sampled];
            order[(size_t)sampled] = swap;
        }
        for (size_t i = 0U; i < training; ++i) {
            const UmiAiTrainingStackedExample *item = &examples[order[i]];
            UmiAiTrainingStackedStep step = {0};
            status = UmiAiTrainingStackedDecoderTrainSelected(
                model, item->inputIds, item->targetIds, item->lossMask,
                item->tokenCount, plan->learningRate, &step);
            if (status != UMI_STATUS_OK) return status;
            weighted += (double)step.selectedCrossEntropyBeforeUpdate *
                        (double)step.supervisedTokens;
            selectedTotal += step.supervisedTokens;
            candidate.supervisedTrainingTokens += (uint64_t)step.supervisedTokens;
            candidate.completedTrainingSteps = step.completedTrainingSteps;
        }
        if (selectedTotal == 0U || !isfinite(weighted))
            return UMI_STATUS_INVALID_STATE;
        candidate.epochTrainingLoss[epoch] = weighted / (double)selectedTotal;
        status = EvaluatePartition(model, examples, count,
                     UMI_AI_TRAINING_PARTITION_EVALUATE,
                     &candidate.epochHeldoutLoss[epoch]);
        if (status != UMI_STATUS_OK) return status;
        candidate.epochsCompleted = epoch + 1U;
    }
    candidate.heldoutLossAfter =
        candidate.epochHeldoutLoss[plan->epochs - 1U];
    *outReport = candidate;
    return UMI_STATUS_OK;
}
