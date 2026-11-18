/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/jsonl_context.h
 *
 * PURPOSE:
 *   Inspect native teaching-decoder context fit without truncating or retaining reviewed JSONL examples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_TRAINING_JSONL_CONTEXT_H
#define UMICOM_AI_TRAINING_JSONL_CONTEXT_H
#include "umicom/ai/training/jsonl_pairs.h"
#include "umicom/ai/training/byte_pair_sequence.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiTrainingJsonlContextSummary {
    size_t contextLength;
    size_t records;
    size_t fittingRecords;
    size_t overlongRecords;
    size_t firstOverlongLine; /* One-based; zero when all fit. */
    size_t shortestRequiredRows;
    size_t longestRequiredRows;
    size_t requiredRows; /* Across ALL records, including overlong ones. */
    size_t fittingValidRows;
    size_t fittingSelectedRows;
    size_t fittingPaddingRows;
} UmiAiTrainingJsonlContextSummary;

/* Reuse the strict JSONL visitor and native byte-pair measurement. Require
 * contextLength in 1..UMI_AI_DECODER_MAX_CONTEXT. Each example needs prompt
 * UTF-8 bytes + completion UTF-8 bytes + one EOS prediction row. This formula
 * applies only to the native byte teaching format, not a pretrained tokenizer.
 *
 * Overlong valid records are counted, not truncated or treated as parse errors.
 * Their rows do not contribute to fitting counters. No text, token arrays or
 * corpus are retained. The shared parser still owns bounded temporary storage.
 * On any malformed row, overflow or cancellation, summary remains unchanged;
 * report describes the scan progress and is initialized whenever nonnull.
 * NULL report is refused. Keep input, summary, report and cancellation lifetime
 * valid, separate and stable for the synchronous call. No I/O or training.
 * A successful scan is not evidence of rights, quality, model compatibility,
 * memory affordability or an acceptable proportion of fitting examples. */
UmiStatus UmiAiTrainingJsonlInspectContext(const void *bytes, size_t length,
    size_t contextLength, const UmiCancellationToken *cancel,
    UmiAiTrainingJsonlContextSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
