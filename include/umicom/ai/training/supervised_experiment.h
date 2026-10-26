/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/supervised_experiment.h
 * PURPOSE:
 *   Run an explicitly bounded and reproducible supervised fine-tuning
 *   experiment against the existing frozen native decoder projection adapter.
 *
 * ENGINEERING CONTRACT:
 *   Training examples are admitted by SupervisedDataset; the existing decoder
 *   projection adapter owns ALL tensor, attention, loss, SGD and model work.
 *   This service owns the schedule, held-out aggregation, top-1 inspection
 *   and evidence. It does NOT modify frozen model parameters, invoke scripts,
 *   execute model output, publish datasets or authenticate a model publisher.
 *
 *   A RUN IS MUTATING: if a later step/evaluation fails, prior successful
 *   adapter updates remain in memory even though *outReport is untouched.
 *   This is not a transactional multi-epoch update or exact resume. Export an
 *   approved model/adapter checkpoint separately before risky experiments.
 *   The adapter is caller-owned and cannot be shared across concurrent runs.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SUPERVISED_EXPERIMENT_H
#define UMICOM_AI_TRAINING_SUPERVISED_EXPERIMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/decoder_projection_adapter.h"
#include "umicom/ai/training/supervised_dataset.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_SUPERVISED_EXPERIMENT_MAX_EPOCHS 16U

typedef struct UmiAiTrainingSupervisedExperimentPlan {
    size_t epochs;               /* 1..16 */
    size_t evaluateEveryEpochs;  /* 1..epochs */
    uint64_t shuffleSeed;        /* seeded Framework permutation */
    float learningRate;          /* finite and strictly positive */
    double stopAtEvaluationLoss; /* zero disables early stopping; >= 0 otherwise */
} UmiAiTrainingSupervisedExperimentPlan;

typedef struct UmiAiTrainingSupervisedEvaluation {
    size_t exampleCount;
    size_t supervisedTokens;
    size_t correctTopOneTokens;
    double meanCrossEntropy;     /* weighted BY SELECTED TARGET TOKENS */
    double topOneAccuracy;       /* correctTopOneTokens / supervisedTokens */
    /* exp(meanCrossEntropy) is meaningful only when representable. */
    double perplexity;
    int perplexityAvailable;
} UmiAiTrainingSupervisedEvaluation;

typedef struct UmiAiTrainingSupervisedEpoch {
    size_t epoch;                 /* 1-based */
    size_t trainingSteps;
    size_t supervisedTargets;
    double meanPreUpdateLoss;      /* weighted by selected target count */
    int evaluated;
    UmiAiTrainingSupervisedEvaluation heldOut;
} UmiAiTrainingSupervisedEpoch;

typedef struct UmiAiTrainingSupervisedExperimentReport {
    UmiAiTrainingSupervisedDatasetInfo dataset;
    UmiAiTrainingSupervisedExperimentPlan plan;
    char frozenCheckpointSha256[UMI_AI_DECODER_ADAPTER_SHA256_HEX_CAPACITY];
    uint64_t initialAdapterSteps;
    uint64_t finalAdapterSteps;
    size_t completedEpochs;
    size_t completedTrainingSteps;
    int stoppedAtTarget;
    UmiAiTrainingSupervisedEvaluation before;
    UmiAiTrainingSupervisedEvaluation after;
    UmiAiTrainingSupervisedEpoch trace[UMI_AI_SUPERVISED_EXPERIMENT_MAX_EPOCHS];
} UmiAiTrainingSupervisedExperimentReport;

UmiStatus UmiAiTrainingSupervisedExperimentPlanValidate(
    const UmiAiTrainingSupervisedExperimentPlan *plan);

/* Read-only assessment on HELD-OUT examples only. Computes weighted token
 * cross-entropy and actual selected-target top-1 accuracy from the real
 * adapter logits. Non-finite logits or losses are refused. This method is
 * not an external language benchmark or evidence of general reasoning. */
UmiStatus UmiAiTrainingSupervisedExperimentEvaluate(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingSupervisedEvaluation *outEvaluation);

/* Validate adapter/dataset/plan BEFORE training, evaluate held-out examples,
 * visit every train example once per seeded epoch, optionally evaluate every
 * N epochs and once after the final epoch. The report is published only if
 * the COMPLETE requested experiment succeeds (or an early-stop condition
 * was reached). On a mid-run failure, successful SGD updates ARE retained:
 * call AdapterInspect to determine the actual completed step count.
 * Epoch trace stores only aggregate metrics, no examples or private text. */
UmiStatus UmiAiTrainingSupervisedExperimentRun(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedExperimentPlan *plan,
    UmiAiTrainingSupervisedExperimentReport *outReport);

#ifdef __cplusplus
}
#endif
#endif
