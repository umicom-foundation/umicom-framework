/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_model_selection.h
 * PURPOSE: Compare two qualified stacked decoders on identical held-out data.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_MODEL_SELECTION_H
#define UMICOM_AI_TRAINING_STACKED_MODEL_SELECTION_H

#include "umicom/ai/training/stacked_evaluation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedSelectionPolicy {
    double maximumAllowedLossIncrease;  /* Absolute nats/token, >=0. */
    double minimumTopOneAccuracy;       /* Inclusive, [0,1]. */
} UmiAiTrainingStackedSelectionPolicy;

typedef struct UmiAiTrainingStackedSelectionReport {
    UmiAiTrainingStackedEvaluationReport reference;
    UmiAiTrainingStackedEvaluationReport candidate;
    double observedLossChange;      /* candidate minus reference. */
    int lossAccepted;
    int accuracyAccepted;
    int candidateAccepted;          /* Both conditions must pass. */
} UmiAiTrainingStackedSelectionReport;

/* Evaluate both model families on the EXACT same reviewed held-out windows.
 * Shapes must agree, although initial seeds and learned weights may differ.
 * A low loss alone is NOT evidence of general model quality or deployment
 * approval. Rejection under the numeric policy is a successful comparison
 * with candidateAccepted==0, NOT an error return.
 *
 * All model operations are read-only. Callers serialise access and retain
 * both models/cases. The two models must not be the same live pointer.
 * Errors, including malformed policies, preserve *outReport entirely. */
UmiStatus UmiAiTrainingStackedModelSelectionCompare(
    UmiAiTrainingStackedDecoder *reference,
    UmiAiTrainingStackedDecoder *candidate,
    const UmiAiTrainingStackedEvaluationCase *heldout,
    size_t exampleCount,
    const UmiAiTrainingStackedSelectionPolicy *policy,
    UmiAiTrainingStackedSelectionReport *outReport);

#ifdef __cplusplus
}
#endif
#endif
