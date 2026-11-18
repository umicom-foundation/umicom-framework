/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/jsonl_file.h
 *
 * PURPOSE:
 *   Inspect a selected bounded local dataset through shared file and JSON ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_JSONL_FILE_H
#define UMICOM_AI_TRAINING_JSONL_FILE_H
#include "umicom/ai/training/jsonl_summary.h"
#include "umicom/ai/training/jsonl_context.h"
#include "umicom/ai/training/jsonl_review.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Read one explicitly selected ordinary absolute local path through the
 * platform input-file service, up to JSONL_BYTES, then summarize strict
 * source_id/prompt/completion records. Final links/devices are refused;
 * parent aliases can be followed. This is not directory confinement.
 *
 * Summary remains unchanged on failure. A nonnull report is initialized;
 * file admission failures have INPUT stage and zero parser progress.
 * Bytes successfully returned by the file reader are cleared before release.
 * This does not erase OS caches, parser allocations or the source file.
 *
 * Cancellation is checked before I/O and during parsing; the bounded
 * synchronous file read itself cannot be interrupted. Use a worker for an
 * interactive application and keep path/token/output storage valid until
 * return. No network, model load, training, file write or retained corpus.
 */
UmiStatus UmiAiTrainingJsonlInspectFile(const char *absolutePath,
    const UmiCancellationToken *cancel, UmiAiTrainingJsonlSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport);
/* Use the same regular-file admission and cleared input ownership for native
 * context statistics. Context bounds, atomic summary and decoded-byte semantics
 * are those of JsonlInspectContext. Invalid context is refused before file I/O.
 * The absolute-path and synchronous cancellation limits above still apply. */
UmiStatus UmiAiTrainingJsonlInspectContextFile(const char *absolutePath,
    size_t contextLength, const UmiCancellationToken *cancel,
    UmiAiTrainingJsonlContextSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport);

/* Read the selected file ONCE through the same admission/clearing owner, then
 * review the acquired bytes. Outputs follow JsonlReview's atomic contract.
 * This retains up to 16 MiB plus a terminator for the input snapshot; the overlap
 * staging budget is NOT a whole-process or total-operation memory budget.
 * Regular-file before/after checks detect some concurrent changes, not a
 * filesystem snapshot transaction. Returned membership belongs to the acquired
 * row order; this API returns no file digest or retained bytes. Do not apply it
 * to a later reopened/edited file without independently establishing identity.
 * Use the in-memory review/selection APIs on one owned snapshot for export.
 * No split files or training run are created. */
UmiStatus UmiAiTrainingJsonlReviewFile(const char *absolutePath,
    const UmiAiTrainingJsonlReviewOptions *options,const UmiCancellationToken *cancel,
    unsigned char *outEvaluation,size_t capacity,UmiAiTrainingJsonlReviewSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
