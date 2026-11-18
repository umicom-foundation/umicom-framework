/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/jsonl_partition.h
 *
 * PURPOSE:
 *   Keep exact JSONL source identities together through the shared deterministic dataset splitter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_JSONL_PARTITION_H
#define UMICOM_AI_TRAINING_JSONL_PARTITION_H
#include "umicom/ai/training/jsonl_pairs.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingJsonlPartitionSummary {
    size_t rows;
    size_t sourceGroups;
    size_t trainingRows;
    size_t evaluationRows;
} UmiAiTrainingJsonlPartitionSummary;

/* Validate all records, then assign one membership byte per input row: zero
 * means training, one means evaluation. Exact decoded source_id strings are
 * sorted into integer ranks and passed to UmiAiTrainingSplitGroups. No hash
 * collision can merge two different source IDs. JSON escape variants match;
 * case and Unicode normalization variants remain distinct caller identities.
 *
 * Select at least one source group for each side. Membership is reproducible
 * for the same SET of source IDs, seed and evaluationGroupCount, regardless
 * of row order or repeated-row counts. Adding/removing a source can change
 * membership: save the reviewed assignment with the dataset version. A group
 * fraction does not promise the same row fraction. This does not identify
 * near-duplicates, prove data rights or produce chronological/stratified splits.
 *
 * Output and summary remain byte-for-byte unchanged on every failure. Report
 * is initialized whenever nonnull; it describes parsing, so COMPLETE can
 * accompany a later grouping/capacity/cancellation error. Only summary.rows
 * membership bytes are written on success. All buffers must be valid separate
 * objects until return. At most JSONL_ROWS rows and JSONL_BYTES input bytes
 * are accepted. Temporary source names are cleared before release. No files,
 * models, network requests or caller buffers are retained. Cancellation is
 * checked around bounded sorting/splitting; those calls are not interruptible.
 */
UmiStatus UmiAiTrainingJsonlPartition(const void *bytes, size_t length,
    size_t evaluationGroupCount, uint64_t seed,
    const UmiCancellationToken *cancel, unsigned char *outEvaluation,
    size_t capacity, UmiAiTrainingJsonlPartitionSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
