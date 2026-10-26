/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/supervised_receipt.h
 * PURPOSE:
 *   Encode a deterministic, privacy-minimising experiment evidence receipt
 *   bound to the reviewed dataset and frozen decoder checkpoint identifiers.
 *
 * The receipt is NOT a model checkpoint, adapter weight file, optimiser
 * state, proof of licensing, or a trusted digital signature. No training text,
 * path, person or environment secret appears in the portable record. The
 * existing Framework SHA-256 is reused for accidental-damage detection.
 * Applications must apply independent approval and signing where required.
 * Every output remains unchanged after an error; no disk or network I/O.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SUPERVISED_RECEIPT_H
#define UMICOM_AI_TRAINING_SUPERVISED_RECEIPT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/supervised_experiment.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_SUPERVISED_RECEIPT_BYTES 208U

typedef struct UmiAiTrainingSupervisedReceiptInfo {
    uint32_t version;
    int stoppedAtTarget;
    size_t completedEpochs;
    size_t trainingExamples;
    size_t evaluationExamples;
    size_t contextLength;
    uint64_t initialAdapterSteps;
    uint64_t finalAdapterSteps;
    size_t trainingTargets;
    size_t evaluationTargets;
    double initialHeldOutLoss;
    double finalHeldOutLoss;
    double initialHeldOutAccuracy;
    double finalHeldOutAccuracy;
    uint64_t shuffleSeed;
    float learningRate;
    uint8_t orderedCorpusSha256[32];
    uint8_t frozenCheckpointSha256[32];
} UmiAiTrainingSupervisedReceiptInfo;

/* A strictly defined 208-byte little-endian record, with IEEE-754 binary32
 * and binary64 metrics, reserved-zero fields and trailing SHA-256. Rejects
 * invalid or non-finite metrics before writing any bytes. Count includes
 * all 208 bytes; outWritten is unchanged on failure. */
UmiStatus UmiAiTrainingSupervisedReceiptEncode(
    const UmiAiTrainingSupervisedExperimentReport *report,
    uint8_t *outBytes, size_t capacity, size_t *outWritten);

/* Strictly verify the complete byte stream without reading a model. Unknown
 * versions, extra bytes, altered checksums, invalid metrics and reserved data
 * are refused. outInfo is not modified on any rejection. */
UmiStatus UmiAiTrainingSupervisedReceiptDecode(
    const uint8_t *bytes, size_t length,
    UmiAiTrainingSupervisedReceiptInfo *outInfo);

/* Confirm that the receipt identifies THIS immutable approved corpus and
 * THIS active adapter's frozen model plus completed step count. Does not
 * verify the adapter's trainable weights: those need a separate checkpoint. */
UmiStatus UmiAiTrainingSupervisedReceiptVerifyLive(
    const UmiAiTrainingSupervisedReceiptInfo *receipt,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingDecoderProjectionAdapter *adapter);

#ifdef __cplusplus
}
#endif
#endif
