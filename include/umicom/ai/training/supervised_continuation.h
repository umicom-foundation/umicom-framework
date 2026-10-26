/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/supervised_continuation.h
 * PURPOSE:
 *   Resume explicitly approved, native C23 adapter-only SGD training at an
 *   exact training-example boundary, including the middle of an epoch.
 *
 * ARCHITECTURE:
 *   SupervisedDataset owns immutable reviewed examples and permutation order.
 *   DecoderProjectionAdapter owns canonical neural operators and trainable
 *   weights. SupervisedExperimentEvaluate owns held-out evaluation. This
 *   service composes those APIs; it adds no optimiser, tokeniser or model.
 *   Only deterministic stateless SGD is supported. AdamW continuation needs
 *   a different moment-state checkpoint and must not use this interface.
 *
 *   State changes are published only after one successful training step or
 *   epoch evaluation. Each training step is independently committed: failures
 *   AFTER a successful update cannot undo the earlier work. The combined
 *   snapshot encodes both the adapter weights and exact next-example cursor
 *   as ONE integrity-checked memory buffer. Applications decide how to save
 *   that buffer using approved Framework storage services.
 *
 *   The record contains source digests and aggregate metrics only, never
 *   training text. SHA-256 is not a publisher signature or consent evidence.
 *   Do not restore untrusted snapshots without authorisation and provenance.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SUPERVISED_CONTINUATION_H
#define UMICOM_AI_TRAINING_SUPERVISED_CONTINUATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/supervised_experiment.h"
#include "umicom/ai/training/decoder_projection_snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_SUPERVISED_CONTINUATION_MAX_BYTES \
    (UMI_AI_PROJECTION_SNAPSHOT_MAX_BYTES + 288U)

typedef struct UmiAiTrainingSupervisedContinuation {
    UmiAiTrainingSupervisedExperimentPlan plan;
    uint8_t orderedCorpusSha256[32];
    char frozenCheckpointSha256[UMI_AI_DECODER_ADAPTER_SHA256_HEX_CAPACITY];
    uint32_t selectedProjections;
    size_t rank;
    size_t trainingExamples;
    size_t evaluationExamples;
    size_t completedEpochs;
    size_t nextExampleOffset;
    uint64_t initialAdapterSteps;
    uint64_t completedAdapterSteps;
    size_t totalSupervisedTargets;
    double totalWeightedPreUpdateLoss;
    int finished;
    int stoppedAtTarget;
    int hasHeldOut;
    UmiAiTrainingSupervisedEvaluation lastHeldOut;
} UmiAiTrainingSupervisedContinuation;

typedef struct UmiAiTrainingSupervisedProgress {
    size_t epoch;                    /* One-based current epoch. */
    size_t positionInEpoch;          /* One-based successful example count. */
    size_t supervisedTokens;
    float crossEntropyBeforeUpdate;
    uint64_t completedAdapterSteps;
} UmiAiTrainingSupervisedProgress;

typedef struct UmiAiTrainingSupervisedEpochResult {
    size_t completedEpochs;
    size_t totalSupervisedTargets;
    double meanPreUpdateCrossEntropy;
    int heldOutEvaluated;
    int finished;
    int stoppedAtTarget;
    UmiAiTrainingSupervisedEvaluation heldOut;
} UmiAiTrainingSupervisedEpochResult;

/* Begin a controlled training run at its current adapter step. The corpus
 * requires both training and evaluation partitions and the same decoder
 * context length. No training step executes during Begin. */
UmiStatus UmiAiTrainingSupervisedContinuationBegin(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedExperimentPlan *plan,
    UmiAiTrainingSupervisedContinuation *outState);

/* Use the established seeded permutation for the current epoch, fetch ONE
 * training example and call the canonical adapter TrainStep once. Refuse if
 * an epoch requires FinishEpoch first or if all epochs are complete.
 * On unsuccessful training, output and cursor are unchanged. If the lower
 * level reports a successful SGD update, the cursor commits that step. */
UmiStatus UmiAiTrainingSupervisedContinuationTrainNext(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingSupervisedContinuation *state,
    UmiAiTrainingSupervisedProgress *outProgress);

/* Only permitted after EVERY training example has been visited. Calls the
 * established held-out evaluator on the declared schedule and always after
 * the final epoch. Early stopping is based on actual held-out cross-entropy.
 * No model weights change; failure retains state and result. */
UmiStatus UmiAiTrainingSupervisedContinuationFinishEpoch(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingSupervisedContinuation *state,
    UmiAiTrainingSupervisedEpochResult *outResult);

/* Inspect the binding and return a complete copy of cursor state. */
UmiStatus UmiAiTrainingSupervisedContinuationInspect(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedContinuation *state,
    UmiAiTrainingSupervisedContinuation *outCopy);

/* Store ONE complete portable record containing the adapter snapshot and
 * cursor. The exact 256-byte header and trailing 32-byte SHA-256 are owned
 * by this service; the nested adapter snapshot uses its original codec.
 * No file IO. Export preserves destination bytes and count on failure. */
UmiStatus UmiAiTrainingSupervisedContinuationSnapshotSize(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedContinuation *state,
    size_t *outBytes);
UmiStatus UmiAiTrainingSupervisedContinuationSnapshotEncode(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedContinuation *state,
    uint8_t *outBytes, size_t capacity, size_t *outWritten);

/* Verify outer/inner snapshots, current approved dataset SHA-256 and frozen
 * model identity. Then restore a NEW independently owned adapter and cursor.
 * Initialise *outAdapter=NULL. Both outputs are unchanged on every error.
 * Destroy the adapter through DecoderProjectionAdapterDestroy on success. */
UmiStatus UmiAiTrainingSupervisedContinuationSnapshotRestore(
    UmiAiTrainingDecoder *frozenDecoder,
    const UmiAiTrainingSupervisedDataset *dataset,
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingDecoderProjectionAdapter **outAdapter,
    UmiAiTrainingSupervisedContinuation *outState);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_SUPERVISED_CONTINUATION_H */
