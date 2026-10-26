/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/supervised_dataset.h
 * PURPOSE:
 *   Convert a verified collection of approved prompt/completion JSONL records
 *   into owned, bounded next-token windows and deterministic split orderings.
 *
 * ARCHITECTURE AND SAFETY:
 *   The existing ReviewedCorpusAudit verifies source evidence, exact JSONL
 *   bytes, duplicates and train/evaluation source grouping. The existing
 *   SupervisedByteWindowBuild owns byte-token semantics and response masking.
 *   This module only composes those Framework services. It never reads files,
 *   accepts unknown JSON properties, trains a model or approves new sources.
 *   Only the canonical JSONL representation emitted by ReviewedExampleJsonl
 *   is supported. Input byte spans are copied into token windows and are not
 *   retained as text or logged. Destroy clears owned token storage before free.
 *
 *   Every output is failure-atomic. The returned dataset is immutable after
 *   creation; callers serialise its destruction with all readers. C23 native.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SUPERVISED_DATASET_H
#define UMICOM_AI_TRAINING_SUPERVISED_DATASET_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/reviewed_corpus.h"
#include "umicom/ai/training/supervised_window.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_SUPERVISED_DATASET_MAX_EXAMPLES UMI_AI_REVIEWED_CORPUS_MAX_EXAMPLES
#define UMI_AI_SUPERVISED_DATASET_MAX_CONTEXT UMI_AI_DECODER_MAX_CONTEXT

typedef struct UmiAiTrainingSupervisedDataset UmiAiTrainingSupervisedDataset;

typedef struct UmiAiTrainingSupervisedExample {
    size_t inputIds[UMI_AI_SUPERVISED_DATASET_MAX_CONTEXT];
    size_t targetIds[UMI_AI_SUPERVISED_DATASET_MAX_CONTEXT];
    uint8_t lossMask[UMI_AI_SUPERVISED_DATASET_MAX_CONTEXT];
    UmiAiTrainingPartition partition;
    size_t supervisedTokens;
    size_t promptBytes;
    size_t completionBytes;
} UmiAiTrainingSupervisedExample;

typedef struct UmiAiTrainingSupervisedDatasetInfo {
    size_t contextLength;
    size_t exampleCount;
    size_t trainingExamples;
    size_t evaluationExamples;
    size_t trainingTargets;
    size_t evaluationTargets;
    size_t distinctSources;
    /* The canonical ReviewedCorpusAudit digest binds the exact ordered
     * approved JSONL, source groups and split. It is not a publisher signature. */
    uint8_t orderedCorpusSha256[UMI_SHA256_BYTES];
} UmiAiTrainingSupervisedDatasetInfo;

/* Admit the COMPLETE approved collection before retaining any example.
 * Every record is validated against the existing review evidence. The parser
 * accepts ONLY the canonical producer's exact key order and JSON escapes.
 * No record may be silently truncated: BOS, prompt, completion and EOS must
 * fit one context. Unsupported JSON, damaged UTF-8 source evidence or a split
 * with zero examples fails. The caller continues to own JSONL/evidence bytes.
 * Initialise *outDataset=NULL; on failure it remains unchanged. */
UmiStatus UmiAiTrainingSupervisedDatasetCreate(
    const char *const *jsonlRecords, const size_t *jsonlLengths,
    const UmiAiTrainingReviewedExampleEvidence *evidence,
    size_t exampleCount, size_t contextLength,
    UmiAiTrainingSupervisedDataset **outDataset);

void UmiAiTrainingSupervisedDatasetDestroy(
    UmiAiTrainingSupervisedDataset *dataset);

UmiStatus UmiAiTrainingSupervisedDatasetInspect(
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingSupervisedDatasetInfo *outInfo);

/* Copies one owned fixed-length window; no pointer to private memory escapes.
 * Both training and held-out examples use canonical decoder token IDs. */
UmiStatus UmiAiTrainingSupervisedDatasetGet(
    const UmiAiTrainingSupervisedDataset *dataset, size_t index,
    UmiAiTrainingSupervisedExample *outExample);

/* Return a deterministic permutation of GLOBAL example indices for ONE split,
 * drawing each eligible example exactly once with Framework's seeded RNG.
 * epoch is caller-supplied; the same seed/epoch/dataset/split reproduces the
 * same result. Capacity counts size_t indices. Caller output and count remain
 * byte-for-byte unchanged on all errors, including insufficient capacity. */
UmiStatus UmiAiTrainingSupervisedDatasetOrder(
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingPartition partition, uint64_t seed, uint64_t epoch,
    size_t *outIndices, size_t capacity, size_t *outCount);

#ifdef __cplusplus
}
#endif
#endif
