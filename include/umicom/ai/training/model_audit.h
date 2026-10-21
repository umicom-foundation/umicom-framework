/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/model_audit.h
 *
 * PURPOSE:
 *   Bind a held-out decoder evaluation to an exact model checkpoint,
 *   immutable source dataset and reviewed native training configuration.
 *
 * ARCHITECTURE AND TRUST:
 *   All training, model mathematics, dataset sampling and checkpoint encoding
 *   remain in their existing Framework owners. This service collects evidence
 *   without introducing another model or learning algorithm.
 *   The fingerprints are FNV-1a64 accidental-change identifiers, NOT
 *   cryptographic hashes, signatures or evidence of file authenticity.
 *   Model weights and optimiser steps must not change during inspection.
 *   A sampled held-out score is not a measure of general coding competence.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_MODEL_AUDIT_H
#define UMICOM_AI_TRAINING_MODEL_AUDIT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/experiment.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This is a copied, transient report. It owns no model, corpus or resources.
 * The dataset fingerprint covers ordered document names, splits, sizes and
 * immutable source-byte fingerprints. The model fingerprint covers the exact
 * canonical checkpoint bytes, including training-step and parameter state.
 * The plan fingerprint includes model dimensions, training and evaluation
 * settings, floating-point learning-rate bits and all declared seeds.
 * The combined fingerprint binds those three identities and completed steps;
 * it does not include implementation-specific floating-point metric rounding.
 * Neither these identifiers nor the existing checkpoint checksum establish
 * authenticity. Consumers need an independent trust and authorisation policy. */
typedef struct UmiAiTrainingModelAudit {
    uint64_t modelFingerprint;
    uint64_t datasetFingerprint;
    uint64_t planFingerprint;
    uint64_t combinedFingerprint;
    uint64_t completedSteps;
    UmiAiTrainingDatasetSummary dataset;
    size_t evaluatedTokens;
    size_t topOneCorrect;
    size_t topFiveCorrect;
    double heldOutCrossEntropy;
    double heldOutPerplexity;
} UmiAiTrainingModelAudit;

/* Sample the same deterministic evaluation windows as ExperimentEvaluate.
 * Cross-entropy/perplexity are delegated to that canonical service; Top-1
 * and Top-5 accuracy are counted independently from the decoder's logits.
 * Equal logit scores rank the lower token ID first. Only held-out documents
 * are sampled. The caller serialises use of model and retains the dataset.
 * On ANY failure, *outAudit is unchanged; model parameters are not updated.
 * A parameter/state fingerprint is verified before and after evaluation. */
UmiStatus UmiAiTrainingModelAuditEvaluate(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    UmiAiTrainingModelAudit *outAudit);

/* Compare an earlier reviewed report to the CURRENT checkpoint, immutable
 * dataset and complete plan. The expensive held-out evaluation is NOT rerun.
 * Returns OK with *outMatches=0 for different identities, including a
 * changed training plan or a newly trained checkpoint. Failure leaves the
 * caller's match output unchanged. No files are opened by this function.
 * Re-loading the dataset is necessary to detect subsequently changed files. */
UmiStatus UmiAiTrainingModelAuditVerify(
    const UmiAiTrainingModelAudit *reference,
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    int *outMatches);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_MODEL_AUDIT_H */
