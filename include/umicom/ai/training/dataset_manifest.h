/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/dataset_manifest.h
 *
 * PURPOSE:
 *   Native, explicitly reviewed multi-file source datasets for learning to
 *   train byte-tokenised decoder models. Whole source files belong to either
 *   training or evaluation; no window crosses a file or split boundary.
 *
 * ARCHITECTURE AND SECURITY:
 *   Framework owns parsing, read-only corpus snapshots, validation, sampling
 *   and reproducible provenance. The manifest is an AUTHORISED LOCAL INPUT,
 *   not a sandbox: symbolic links and filesystem races can escape the selected
 *   directory. Do not open untrusted manifests or include credentials,
 *   proprietary source, personal records or other unauthorised material.
 *   Hashes are FNV-1a64 accidental-change indicators, NOT authenticity proofs.
 *   A dataset is immutable after loading; calls can be serialised externally.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab / Umicom Foundation
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_DATASET_MANIFEST_H
#define UMICOM_AI_TRAINING_DATASET_MANIFEST_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DATASET_MAX_FILES 16U
#define UMI_AI_DATASET_MAX_PATH 160U
#define UMI_AI_DATASET_MAX_MANIFEST_BYTES 8192U
#define UMI_AI_DATASET_MAX_CONTEXT 32U
#define UMI_AI_DATASET_MAX_SEQUENCES 4U

typedef enum UmiAiTrainingPartition {
    UMI_AI_TRAINING_PARTITION_TRAIN = 1,
    UMI_AI_TRAINING_PARTITION_EVALUATE = 2
} UmiAiTrainingPartition;

typedef struct UmiAiTrainingDataset UmiAiTrainingDataset;

typedef struct UmiAiTrainingDocumentInfo {
    UmiAiTrainingPartition partition;
    char relativePath[UMI_AI_DATASET_MAX_PATH];
    size_t byteCount;
    uint64_t fingerprint;
} UmiAiTrainingDocumentInfo;

typedef struct UmiAiTrainingDatasetSummary {
    size_t documentCount;
    size_t trainingDocuments;
    size_t evaluationDocuments;
    size_t trainingBytes;
    size_t evaluationBytes;
} UmiAiTrainingDatasetSummary;

/* Read the *trusted* local manifest and its explicitly named source files.
 * Manifest syntax (UTF-8 ASCII subset, LF or CRLF):
 *
 *   UMICOM-AI-DATASET-1
 *   train train/accounts.c
 *   eval  eval/payments.c
 *
 * Blank lines and whole-line '#' comments are accepted. File paths use '/'
 * relative to the manifest directory; only [A-Za-z0-9_.-/] is accepted,
 * without empty, '.' or '..' components. Paths are not a security boundary.
 * A corpus snapshot is read only once. Exact duplicate paths or byte-for-byte
 * duplicates detected by the noncryptographic fingerprint/length are rejected
 * across either split, to reduce obvious leakage. Near-duplicates need review.
 * Both groups require at least one document. On failure *outDataset remains
 * unchanged and all partially acquired resources are freed. */
UmiStatus UmiAiTrainingDatasetLoadManifest(
    const char *manifestPath, UmiAiTrainingDataset **outDataset);

void UmiAiTrainingDatasetDestroy(UmiAiTrainingDataset *dataset);

/* Copy immutable metadata, never expose internal corpus pointers. Rejected
 * requests do not write output. */
UmiStatus UmiAiTrainingDatasetInspect(
    const UmiAiTrainingDataset *dataset,
    UmiAiTrainingDatasetSummary *outSummary);
UmiStatus UmiAiTrainingDatasetDocument(
    const UmiAiTrainingDataset *dataset, size_t index,
    UmiAiTrainingDocumentInfo *outInfo);

/* Produce sequenceCount complete next-token windows for the selected split.
 * Each sample is chosen uniformly from ALL eligible windows in that split,
 * including windows in different files; no window crosses a file boundary.
 * Sequence selection is reproducible from seed, batch number, partition and
 * the immutable corpus order. Batch arrays contain size_t token IDs accepted
 * by UmiAiTrainingDecoderTrainStep / Evaluate. The caller must supply at least
 * sequenceCount*contextLength ELEMENTS in each NONOVERLAPPING output buffer.
 * Constraints are explicit and small for this educational CPU laboratory:
 * context=1..32, sequences=1..4. The consumer model imposes further shape
 * constraints (e.g. tokenCount*vocabularySize <= tape's element capacity).
 * Invalid and capacity failures leave BOTH output arrays untouched. */
UmiStatus UmiAiTrainingDatasetBatch(
    const UmiAiTrainingDataset *dataset,
    UmiAiTrainingPartition partition,
    size_t contextLength, size_t sequenceCount,
    uint64_t seed, uint64_t batchNumber,
    size_t *outInputs, size_t *outTargets, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_DATASET_MANIFEST_H */
