/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/supervised_experiment.c
 * PURPOSE:
 *   Deterministic multi-example SGD training with explicit held-out evidence.
 *   Reuse reviewed windows and actual frozen-decoder projection adapter APIs.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_experiment.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

UmiStatus UmiAiTrainingSupervisedExperimentPlanValidate(
    const UmiAiTrainingSupervisedExperimentPlan *plan)
{
    if (plan == NULL || plan->epochs == 0U ||
        plan->epochs > UMI_AI_SUPERVISED_EXPERIMENT_MAX_EPOCHS ||
        plan->evaluateEveryEpochs == 0U ||
        plan->evaluateEveryEpochs > plan->epochs ||
        !isfinite(plan->learningRate) || plan->learningRate <= 0.0F ||
        !isfinite(plan->stopAtEvaluationLoss) ||
        plan->stopAtEvaluationLoss < 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static UmiStatus ExperimentBinding(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingDecoderProjectionAdapterInfo *outAdapter,
    UmiAiTrainingSupervisedDatasetInfo *outDataset)
{
    UmiStatus status;
    if (adapter == NULL || dataset == NULL ||
        outAdapter == NULL || outDataset == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderProjectionAdapterInspect(adapter, outAdapter);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingSupervisedDatasetInspect(dataset, outDataset);
    if (status != UMI_STATUS_OK) return status;
    if (outDataset->contextLength == 0U ||
        outDataset->contextLength != (size_t)outAdapter->decoder.contextLength ||
        outDataset->trainingExamples == 0U || outDataset->evaluationExamples == 0U ||
        outDataset->evaluationTargets == 0U || outDataset->trainingTargets == 0U ||
        outAdapter->decoder.vocabularySize == 0U ||
        outAdapter->decoder.vocabularySize > UMI_AI_DECODER_MAX_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* Always use a held-out record for validation. The classification statistic
 * is EXACT selected-target top-1 agreement with the expected token ID; it is
 * not natural-language comprehension or a safety measure. Ties choose the
 * lowest vocabulary ID, matching the canonical argmax policy. */
UmiStatus UmiAiTrainingSupervisedExperimentEvaluate(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingSupervisedEvaluation *outEvaluation)
{
    UmiAiTrainingDecoderProjectionAdapterInfo adapterInfo = {0};
    UmiAiTrainingSupervisedDatasetInfo datasetInfo = {0};
    UmiAiTrainingSupervisedEvaluation candidate = {0};
    UmiStatus status;
    double weightedLoss = 0.0;
    float *logits = NULL;
    size_t context, vocabulary, rowValues;

    if (outEvaluation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ExperimentBinding(adapter, dataset, &adapterInfo, &datasetInfo);
    if (status != UMI_STATUS_OK) return status;
    context = datasetInfo.contextLength;
    vocabulary = (size_t)adapterInfo.decoder.vocabularySize;
    if (context > SIZE_MAX / vocabulary ||
        context * vocabulary > UMI_AI_TRAINING_MAX_ELEMENTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    rowValues = context * vocabulary;
    logits = malloc(rowValues * sizeof(*logits));
    if (logits == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    for (size_t i = 0U; i < datasetInfo.exampleCount; ++i) {
        UmiAiTrainingSupervisedExample example = {0};
        float sampleLoss = 0.0F;
        status = UmiAiTrainingSupervisedDatasetGet(dataset, i, &example);
        if (status != UMI_STATUS_OK) goto cleanup;
        if (example.partition != UMI_AI_TRAINING_PARTITION_EVALUATE) continue;
        if (example.supervisedTokens == 0U ||
            example.supervisedTokens > context) {
            status = UMI_STATUS_INVALID_STATE;
            goto cleanup;
        }
        status = UmiAiTrainingDecoderProjectionAdapterEvaluate(
            adapter, example.inputIds, example.targetIds,
            example.lossMask, context, &sampleLoss);
        if (status != UMI_STATUS_OK) goto cleanup;
        if (!isfinite(sampleLoss) || sampleLoss < 0.0F) {
            status = UMI_STATUS_INVALID_STATE; goto cleanup;
        }
        status = UmiAiTrainingDecoderProjectionAdapterLogits(
            adapter, example.inputIds, context, logits, rowValues);
        if (status != UMI_STATUS_OK) goto cleanup;
        for (size_t row = 0U; row < context; ++row) {
            if (example.lossMask[row] > 1U) {
                status = UMI_STATUS_INVALID_STATE; goto cleanup;
            }
            size_t topToken = 0U;
            float topValue = -INFINITY;
            for (size_t token = 0U; token < vocabulary; ++token) {
                const float value = logits[row * vocabulary + token];
                if (!isfinite(value)) {
                    status = UMI_STATUS_INVALID_STATE; goto cleanup;
                }
                if (value > topValue) {
                    topValue = value;
                    topToken = token;
                }
            }
            if (example.lossMask[row] != 0U) {
                if (example.targetIds[row] >= vocabulary) {
                    status = UMI_STATUS_INVALID_STATE; goto cleanup;
                }
                ++candidate.supervisedTokens;
                if (topToken == example.targetIds[row])
                    ++candidate.correctTopOneTokens;
            }
        }
        weightedLoss += (double)sampleLoss * (double)example.supervisedTokens;
        ++candidate.exampleCount;
    }
    if (candidate.exampleCount != datasetInfo.evaluationExamples ||
        candidate.supervisedTokens != datasetInfo.evaluationTargets ||
        candidate.supervisedTokens == 0U || !isfinite(weightedLoss)) {
        status = UMI_STATUS_INVALID_STATE; goto cleanup;
    }
    candidate.meanCrossEntropy = weightedLoss / (double)candidate.supervisedTokens;
    candidate.topOneAccuracy = (double)candidate.correctTopOneTokens /
                               (double)candidate.supervisedTokens;
    if (!isfinite(candidate.meanCrossEntropy) ||
        candidate.meanCrossEntropy < 0.0 ||
        !isfinite(candidate.topOneAccuracy)) {
        status = UMI_STATUS_INVALID_STATE; goto cleanup;
    }
    candidate.perplexityAvailable = candidate.meanCrossEntropy < log(DBL_MAX);
    if (candidate.perplexityAvailable) {
        candidate.perplexity = exp(candidate.meanCrossEntropy);
        if (!isfinite(candidate.perplexity)) {
            status = UMI_STATUS_INVALID_STATE; goto cleanup;
        }
    }
    /* No partially constructed evidence is published. */
    *outEvaluation = candidate;
    status = UMI_STATUS_OK;
cleanup:
    free(logits);
    return status;
}

UmiStatus UmiAiTrainingSupervisedExperimentRun(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedExperimentPlan *plan,
    UmiAiTrainingSupervisedExperimentReport *outReport)
{
    UmiAiTrainingSupervisedExperimentReport candidate = {0};
    UmiAiTrainingDecoderProjectionAdapterInfo original = {0}, current = {0};
    size_t indices[UMI_AI_SUPERVISED_DATASET_MAX_EXAMPLES] = {0};
    UmiStatus status;

    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingSupervisedExperimentPlanValidate(plan);
    if (status != UMI_STATUS_OK) return status;
    status = ExperimentBinding(adapter, dataset, &original, &candidate.dataset);
    if (status != UMI_STATUS_OK) return status;
    if (candidate.dataset.trainingExamples > UINT64_MAX / plan->epochs ||
        original.completedAdapterSteps > UINT64_MAX -
            (uint64_t)candidate.dataset.trainingExamples * (uint64_t)plan->epochs)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate.plan = *plan;
    candidate.initialAdapterSteps = original.completedAdapterSteps;
    memcpy(candidate.frozenCheckpointSha256, original.frozenCheckpointSha256,
           sizeof(candidate.frozenCheckpointSha256));

    status = UmiAiTrainingSupervisedExperimentEvaluate(
        adapter, dataset, &candidate.before);
    if (status != UMI_STATUS_OK) return status;
    candidate.after = candidate.before;

    for (size_t epoch = 0U; epoch < plan->epochs; ++epoch) {
        UmiAiTrainingSupervisedEpoch *observed = &candidate.trace[epoch];
        double totalWeightedLoss = 0.0;
        size_t count = 0U;
        status = UmiAiTrainingSupervisedDatasetOrder(dataset,
                    UMI_AI_TRAINING_PARTITION_TRAIN,
                    plan->shuffleSeed, (uint64_t)epoch,
                    indices, UMI_AI_SUPERVISED_DATASET_MAX_EXAMPLES, &count);
        if (status != UMI_STATUS_OK) return status;
        if (count != candidate.dataset.trainingExamples) return UMI_STATUS_INVALID_STATE;
        observed->epoch = epoch + 1U;
        for (size_t ordinal = 0U; ordinal < count; ++ordinal) {
            UmiAiTrainingSupervisedExample example = {0};
            UmiAiTrainingDecoderProjectionStep step = {0};
            status = UmiAiTrainingSupervisedDatasetGet(
                         dataset, indices[ordinal], &example);
            if (status != UMI_STATUS_OK) return status;
            if (example.partition != UMI_AI_TRAINING_PARTITION_TRAIN ||
                example.supervisedTokens == 0U)
                return UMI_STATUS_INVALID_STATE;
            status = UmiAiTrainingDecoderProjectionAdapterTrainStep(
                adapter, example.inputIds, example.targetIds,
                example.lossMask, candidate.dataset.contextLength,
                plan->learningRate, &step);
            if (status != UMI_STATUS_OK) return status;
            /* There is no rollback of ALREADY SUCCESSFUL SGD updates if a
             * later step fails. Report publication remains all-or-nothing. */
            if (!isfinite(step.selectedCrossEntropyBeforeUpdate) ||
                step.selectedCrossEntropyBeforeUpdate < 0.0F ||
                step.supervisedTokenCount != example.supervisedTokens ||
                step.completedAdapterSteps !=
                    candidate.initialAdapterSteps +
                    (uint64_t)candidate.completedTrainingSteps + 1U)
                return UMI_STATUS_INVALID_STATE;
            totalWeightedLoss += (double)step.selectedCrossEntropyBeforeUpdate *
                                 (double)example.supervisedTokens;
            ++candidate.completedTrainingSteps;
            ++observed->trainingSteps;
            observed->supervisedTargets += example.supervisedTokens;
        }
        if (observed->trainingSteps != candidate.dataset.trainingExamples ||
            observed->supervisedTargets != candidate.dataset.trainingTargets ||
            !isfinite(totalWeightedLoss)) return UMI_STATUS_INVALID_STATE;
        observed->meanPreUpdateLoss = totalWeightedLoss /
                                      (double)observed->supervisedTargets;
        if (!isfinite(observed->meanPreUpdateLoss)) return UMI_STATUS_INVALID_STATE;
        ++candidate.completedEpochs;
        if ((epoch + 1U) % plan->evaluateEveryEpochs == 0U ||
            epoch + 1U == plan->epochs) {
            status = UmiAiTrainingSupervisedExperimentEvaluate(
                adapter, dataset, &observed->heldOut);
            if (status != UMI_STATUS_OK) return status;
            observed->evaluated = 1;
            candidate.after = observed->heldOut;
            if (plan->stopAtEvaluationLoss > 0.0 &&
                observed->heldOut.meanCrossEntropy <= plan->stopAtEvaluationLoss) {
                candidate.stoppedAtTarget = 1;
                break;
            }
        }
    }
    status = UmiAiTrainingDecoderProjectionAdapterInspect(adapter, &current);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(current.frozenCheckpointSha256,
               original.frozenCheckpointSha256,
               sizeof(original.frozenCheckpointSha256)) != 0 ||
        current.selectedProjections != original.selectedProjections ||
        current.rank != original.rank ||
        current.completedAdapterSteps !=
            original.completedAdapterSteps + (uint64_t)candidate.completedTrainingSteps)
        return UMI_STATUS_INVALID_STATE;
    candidate.finalAdapterSteps = current.completedAdapterSteps;
    *outReport = candidate;
    return UMI_STATUS_OK;
}
