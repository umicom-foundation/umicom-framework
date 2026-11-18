/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/jsonl_pairs.h
 *
 * PURPOSE:
 *   Read bounded prompt/completion teaching records without training, uploading or retaining caller buffers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_JSONL_PAIRS_H
#define UMICOM_AI_TRAINING_JSONL_PAIRS_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_TRAINING_PAIR_TEXT_BYTES 4096U
#define UMI_AI_TRAINING_PAIR_SOURCE_BYTES 128U
#define UMI_AI_TRAINING_PAIR_LINE_BYTES 32768U
#define UMI_AI_TRAINING_JSONL_BYTES (16U * 1024U * 1024U)
#define UMI_AI_TRAINING_JSONL_ROWS 4096U

typedef struct UmiAiTrainingPair {
    char sourceId[UMI_AI_TRAINING_PAIR_SOURCE_BYTES + 1U];
    char prompt[UMI_AI_TRAINING_PAIR_TEXT_BYTES + 1U];
    char completion[UMI_AI_TRAINING_PAIR_TEXT_BYTES + 1U];
} UmiAiTrainingPair;

/* One object with exactly source_id, prompt and completion string fields.
 * Source ID and completion must be nonempty; an empty prompt is allowed.
 * Whitespace-only content is retained for caller quality review. JSON escapes
 * are decoded by Framework's shared JSON owner, including strict UTF-8 and
 * surrogate checks. Duplicate decoded keys, unknown fields, nested values,
 * embedded/decoded NUL and trailing non-whitespace data are rejected.
 * This small teaching schema is NOT a provider's chat or tokenizer format.
 *
 * Read exactly length bytes; no terminator is required. Output is a caller-
 * owned copy, unchanged on failure. No I/O, model loading or normalization.
 * Input and output must be valid separate objects until return. A selected
 * source ID is provenance supplied by the author, not proof of data rights.
 */
UmiStatus UmiAiTrainingPairRead(const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiAiTrainingPair *outPair);


/* Write the exact teaching schema in source_id/prompt/completion order,
 * followed by LF and a terminating NUL. outBytes excludes NUL and includes LF.
 * All three fixed arrays must contain a terminator within their declared
 * bounds. Empty source IDs/completions and malformed UTF-8 are refused.
 * Escaping uses the existing Framework JSON writer; no values are evaluated.
 *
 * The serialized JSON (without LF) must fit PAIR_LINE_BYTES. Escape expansion
 * can exceed that bound even when decoded fields fit their individual limits.
 * Output and outBytes remain unchanged on EVERY failure. Capacity includes
 * the terminator; temporary owned output is cleared before release.
 * Inputs, output and outBytes must be valid separate caller-owned objects.
 * This writes memory only; file publication remains the caller's operation.
 */
UmiStatus UmiAiTrainingPairWrite(const UmiAiTrainingPair *pair,
    char *output, size_t capacity, size_t *outBytes);

typedef enum UmiAiTrainingJsonlStage {
    UMI_AI_TRAINING_JSONL_INPUT = 0,
    UMI_AI_TRAINING_JSONL_RECORD = 1,
    UMI_AI_TRAINING_JSONL_VISITOR = 2,
    UMI_AI_TRAINING_JSONL_COMPLETE = 3
} UmiAiTrainingJsonlStage;

typedef struct UmiAiTrainingJsonlReport {
    size_t acceptedRows;
    size_t consumedBytes;
    size_t failedLine; /* one-based on failure, zero on complete input */
    UmiAiTrainingJsonlStage stage;
} UmiAiTrainingJsonlReport;

/* The pair is borrowed ONLY until this synchronous callback returns. Copy any
 * values you need afterward. Return OK to accept a row, or an error to stop.
 * The callback must not modify the input bytes, report, pair or token lifetime.
 */
typedef UmiStatus (*UmiAiTrainingPairVisitor)(void *context,
    const UmiAiTrainingPair *pair, size_t line);

/* Scan LF or CRLF records, including a final record without LF. A single
 * final CR is also stripped. Delimiter bytes do not consume the per-record
 * JSON byte cap, but do consume the whole-input byte cap. Blank lines,
 * a UTF-8 BOM and multiline JSON are refused; an escaped "\n" remains text.
 * Bound the entire input, each line and the accepted row count. Cancellation
 * is checked between records and by the shared JSON parser. A callback itself
 * is not interruptible. Do not retain the input or callback after return.
 *
 * A scan is NOT transactional across callbacks: earlier accepted callbacks
 * remain completed if a later row fails. consumedBytes ends after the last
 * accepted line (including its delimiter); it excludes the failing line.
 * Every call with a nonnull report initializes it, even invalid input calls.
 * Keep report storage separate from input and callback-owned state.
 * This is a validator/visitor, not a file loader or model trainer.
 */
UmiStatus UmiAiTrainingJsonlVisit(const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiAiTrainingPairVisitor visitor,
    void *context, UmiAiTrainingJsonlReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
