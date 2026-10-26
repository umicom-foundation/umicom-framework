/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/supervised_quality.h
 * PURPOSE:
 *   Qualify native, held-out fine-tuning evidence against explicitly approved
 *   quality thresholds without mistaking a training loss for general utility.
 *
 * This is a READ-ONLY decision helper. It does not approve public release,
 * sign any artifact, evaluate language meaning, inspect private examples or
 * bypass established security and independent model-promotion services.
 * Counters and finite metrics come from SupervisedExperimentEvaluate; the
 * caller must provide both observations over the same held-out corpus.
 * All rejected calls preserve the destination report.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SUPERVISED_QUALITY_H
#define UMICOM_AI_TRAINING_SUPERVISED_QUALITY_H

#include "umicom/ai/training/supervised_experiment.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingSupervisedQualityPolicy {
    double maximumCrossEntropyIncrease; /* Nonnegative; tolerable regression. */
    double minimumTopOneAccuracy;        /* 0..1 held-out exact-token accuracy. */
    double minimumCrossEntropyReduction; /* 0 disables required improvement. */
} UmiAiTrainingSupervisedQualityPolicy;

typedef struct UmiAiTrainingSupervisedQualityReport {
    int accepted;                       /* ALL declared checks satisfied. */
    int lossWithinTolerance;
    int accuracyAtLeastMinimum;
    int improvementAtLeastMinimum;
    size_t evaluatedTokens;
    double lossChange;                  /* after - before; negative is better. */
    double accuracyChange;              /* after - before. */
} UmiAiTrainingSupervisedQualityReport;

/* Valid returns may have accepted=0: a measured model-quality regression is
 * not a C API error. A successful acceptance is NOT deployment authorisation.
 * Positive requested improvement and positive permitted regression cannot
 * both be met unless observed results satisfy both independent checks. */
UmiStatus UmiAiTrainingSupervisedQualityEvaluate(
    const UmiAiTrainingSupervisedEvaluation *before,
    const UmiAiTrainingSupervisedEvaluation *after,
    const UmiAiTrainingSupervisedQualityPolicy *policy,
    UmiAiTrainingSupervisedQualityReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_SUPERVISED_QUALITY_H */
