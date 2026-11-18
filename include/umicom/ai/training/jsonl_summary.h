/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/jsonl_summary.h
 *
 * PURPOSE:
 *   Inspect bounded dataset sizes and exact source groups before choosing training partitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_JSONL_SUMMARY_H
#define UMICOM_AI_TRAINING_JSONL_SUMMARY_H
#include "umicom/ai/training/jsonl_pairs.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingJsonlSummary {
    size_t rows;
    size_t sourceGroups;
    size_t largestSourceGroupRows;
    size_t emptyPrompts;
    size_t promptBytes;
    size_t completionBytes;
    size_t longestPromptBytes;
    size_t longestCompletionBytes;
} UmiAiTrainingJsonlSummary;

/* Inspect the same strict records as JsonlVisit without retaining text or
 * publishing partial statistics. Count decoded UTF-8 BYTES, not characters,
 * tokens or a model's context length. Groups use exact decoded source_id bytes:
 * JSON escape variants coincide; case and Unicode normalization do not.
 *
 * At most JSONL_ROWS bounded source IDs are copied, sorted and cleared before
 * release. No prompt/completion corpus is retained. This is not duplicate-text
 * detection, proof of rights, a split assignment, tokenization or training.
 * A dominant group can make a group split differ greatly from a row split.
 *
 * On failure summary remains unchanged; report exposes parser progress and is
 * initialized whenever nonnull. A failure before scanning has INPUT stage.
 * Cancellation is checked before and after bounded sorting, not inside qsort.
 * Inputs, summary and report must be valid separate objects until return.
 */
UmiStatus UmiAiTrainingJsonlSummarize(const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiAiTrainingJsonlSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
