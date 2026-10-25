/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/reviewed_examples.h
 * PURPOSE:
 *   Prepare explicitly reviewed text spans as a bounded supervised-learning
 *   JSONL record with cryptographic provenance kept separately from text.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_REVIEWED_EXAMPLES_H
#define UMICOM_AI_TRAINING_REVIEWED_EXAMPLES_H

#include <stddef.h>
#include "umicom/ai/training/source_evidence.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES 4096U

typedef struct UmiAiTrainingTextSpan {
    size_t offset;
    size_t byteCount;
} UmiAiTrainingTextSpan;

typedef struct UmiAiTrainingReviewedExampleEvidence {
    UmiAiTrainingAdmittedSource admittedSource;
    UmiAiTrainingTextSpan prompt;
    UmiAiTrainingTextSpan completion;
    size_t jsonBytes;
    char jsonSha256[65];
} UmiAiTrainingReviewedExampleEvidence;

/* Format ONE prompt/completion example drawn from a source that passes the
 * existing training-use review AND the actual SHA-256 source-byte checks.
 * Prompt and completion are nonempty, ordered, nonoverlapping byte spans of
 * reviewed UTF-8 text. Both must be <= MAX_SPAN_BYTES. A full UTF-8 scan
 * rejects invalid sequences and NUL bytes; JSON controls are escaped.
 *
 * The returned single-line JSONL object is exactly:
 *   {"prompt":"...","completion":"..."}\n
 * Provenance (including the original asset digest, reviewed derivative digest
 * and train/evaluation partition) is returned in outEvidence, NOT included in
 * the model-facing JSONL. This avoids inadvertently copying file names or
 * provenance identifiers into training examples. The caller must keep the
 * evidence associated with the exact record before admitting a corpus.
 * Records derived from the same original asset receive the same partition.
 *
 * outputCapacity includes one trailing NUL for convenient inspection;
 * outLength counts JSONL bytes including LF, excluding the terminator.
 * ALL caller outputs remain unchanged on failure, including a short buffer.
 * The function never writes a file, reads a device, invokes an AI provider,
 * redacts personal data, discovers usage rights or trains a model. Human
 * reviews remain explicit and mandatory; this is not an automated approval.
 * Caller-owned input and output regions must remain valid during the call.
 * The two scalar output objects must not overlap other output storage. */
UmiStatus UmiAiTrainingReviewedExampleJsonl(
    const UmiAiTrainingSourceReview *review,
    const void *originalAsset, size_t originalLength,
    const void *preparedUtf8, size_t preparedLength,
    UmiAiTrainingTextSpan prompt,
    UmiAiTrainingTextSpan completion,
    unsigned evaluationPercentage,
    char *output, size_t outputCapacity,
    size_t *outLength,
    UmiAiTrainingReviewedExampleEvidence *outEvidence);

#ifdef __cplusplus
}
#endif
#endif
