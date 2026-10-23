/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/model_quality_gate.h
 *
 * PURPOSE:
 *   Compare real, reproducible held-out decoder evaluations against an
 *   explicit candidate-quality policy without duplicating model mathematics.
 *
 * ARCHITECTURE AND LIMITS:
 *   Framework ModelAuditEvaluate remains the only evaluator. The comparison
 *   requires matching dataset and complete experiment-plan fingerprints,
 *   including the evaluation seed/window selection. FNV-1a identities in
 *   ModelAudit are change indicators, NOT signatures or provenance proof.
 *   A positive result assesses a small sampled next-token task only; it is
 *   neither a signed release approval nor evidence of coding competence.
 *   The caller approves the dataset and checkpoints before using them.
 *
 * OWNERSHIP:
 *   No handles are retained. The caller serialises model access and retains
 *   the dataset, plan and policy for the duration of each synchronous call.
 *   All output storage is caller-owned and unchanged on failure.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_MODEL_QUALITY_GATE_H
#define UMICOM_AI_TRAINING_MODEL_QUALITY_GATE_H

#include <stddef.h>
#include "umicom/ai/training/model_audit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingModelQualityPolicy {
    /* A candidate must reduce cross-entropy STRICTLY, even when this is 0.
     * Positive values specify an additional minimum required reduction. */
    double minimumCrossEntropyReduction;
    /* Fractional decrease (e.g. 0.02 = two percentage points) tolerated in
     * measured top-one/top-five next-token accuracy; each is in [0, 1]. */
    double maximumTopOneAccuracyRegression;
    double maximumTopFiveAccuracyRegression;
    /* The count is an educational sample-size floor, not a confidence bound. */
    size_t minimumEvaluatedTokens;
} UmiAiTrainingModelQualityPolicy;

typedef struct UmiAiTrainingModelQualityReport {
    UmiAiTrainingModelAudit baseline;
    UmiAiTrainingModelAudit candidate;
    double crossEntropyReduction;
    double topOneAccuracyChange;
    double topFiveAccuracyChange;
    int accepted;
} UmiAiTrainingModelQualityReport;

/* Conservative educational defaults: any strictly positive improvement in
 * held-out cross-entropy, no top-1/top-5 regression, >=32 evaluated tokens.
 * A production evaluation requires a substantially larger independent suite. */
UmiStatus UmiAiTrainingModelQualityDefaults(
    UmiAiTrainingModelQualityPolicy *outPolicy);

/* Validate nonnegative finite loss threshold, accuracy tolerances [0,1] and
 * a nonzero sample floor. No allocations or output mutations on failure. */
UmiStatus UmiAiTrainingModelQualityValidate(
    const UmiAiTrainingModelQualityPolicy *policy);

/* Compare two existing canonical audit snapshots. The SAME planFingerprint,
 * datasetFingerprint, dataset summary and exact evaluated-token count are
 * mandatory, so separately selected evaluation windows cannot be compared.
 * Comparisons do NOT authenticate externally supplied audit structs. Use
 * Evaluate below to measure both supplied models directly.
 * A failed quality policy returns OK with accepted=0, not an API error.
 * Any malformed or incomparable evidence returns an error and preserves
 * the entire caller-owned report. */
UmiStatus UmiAiTrainingModelQualityCompareReports(
    const UmiAiTrainingModelAudit *baseline,
    const UmiAiTrainingModelAudit *candidate,
    const UmiAiTrainingModelQualityPolicy *policy,
    UmiAiTrainingModelQualityReport *outReport);

/* Evaluate two already-reviewed decoder handles against precisely ONE
 * immutable dataset and ONE approved plan. Canonical ModelAuditEvaluate
 * checks the model checkpoint before and after held-out evaluation.
 * The plan's total steps must cover BOTH input checkpoints; this function
 * never trains, writes checkpoints or modifies caller-owned outputs on error.
 * It does not authenticate the checkpoint files: use independent signed
 * publication verification before loading untrusted files. */
UmiStatus UmiAiTrainingModelQualityEvaluate(
    UmiAiTrainingDecoder *baselineModel,
    UmiAiTrainingDecoder *candidateModel,
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *commonPlan,
    const UmiAiTrainingModelQualityPolicy *policy,
    UmiAiTrainingModelQualityReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_MODEL_QUALITY_GATE_H */
