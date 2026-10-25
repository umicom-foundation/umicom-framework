/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/reviewed_corpus.h
 * PURPOSE:
 *   Check reviewed supervised JSONL records for byte integrity, exact
 *   duplicates and source-group train/evaluation leakage before dataset use.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_REVIEWED_CORPUS_H
#define UMICOM_AI_TRAINING_REVIEWED_CORPUS_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/reviewed_examples.h"
#include "umicom/base/sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_REVIEWED_CORPUS_MAX_EXAMPLES 256U

typedef struct UmiAiTrainingReviewedCorpusReport {
    size_t exampleCount;
    size_t trainingExamples;
    size_t evaluationExamples;
    size_t distinctOriginalSources;
    size_t jsonBytes;
    uint8_t orderedDigest[UMI_SHA256_BYTES];
} UmiAiTrainingReviewedCorpusReport;

/* Verify a bounded, caller-owned sequence of records that were previously
 * produced by ReviewedExampleJsonl. Every exact JSONL byte span must match
 * its reviewed SHA-256 and recorded length; duplicate records are rejected.
 * All derivatives sharing an original asset digest MUST have the same split.
 * Identical prepared content cannot be assigned to different splits even
 * when original media digests differ. Both train and evaluation are required.
 *
 * This detects exact leaks; it is NOT near-duplicate detection, a content
 * rights investigation, a JSON parser, a file reader or a model trainer.
 * It does not authorise a source on its own. First use ReviewedExampleJsonl
 * on source bytes already approved by SourceVerifyBytes. Outputs are counts
 * and a deterministic ordered digest; no prompts or file paths are emitted.
 * A rejection preserves *outReport byte-for-byte. No network/disk I/O. */
UmiStatus UmiAiTrainingReviewedCorpusAudit(
    const char *const *jsonlRecords, const size_t *jsonlLengths,
    const UmiAiTrainingReviewedExampleEvidence *evidence,
    size_t recordCount, UmiAiTrainingReviewedCorpusReport *outReport);

#ifdef __cplusplus
}
#endif
#endif
