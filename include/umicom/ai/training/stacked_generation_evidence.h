/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_generation_evidence.h
 * PURPOSE: Bind reproducible native generation to model, seed and token IDs.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_GENERATION_EVIDENCE_H
#define UMICOM_AI_TRAINING_STACKED_GENERATION_EVIDENCE_H

#include "umicom/ai/training/stacked_inference.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedGenerationEvidence {
    UmiAiTrainingStackedGenerationStats generation;
    UmiAiTrainingNucleusOptions policy;
    uint64_t seed;
    size_t promptTokenCount;
    size_t outputTokenCount;
    uint8_t samplingPolicySha256[UMI_SHA256_BYTES];
    uint8_t orderedPromptSha256[UMI_SHA256_BYTES];
    uint8_t orderedOutputSha256[UMI_SHA256_BYTES];
} UmiAiTrainingStackedGenerationEvidence;

/* Verify a CLAIMED generated token sequence by recomputing it with the exact
 * frozen decoder, prompt, sampling policy and seed. Then hash the ordered
 * prompt/output ID sequences in a fixed-width endian-neutral representation.
 * Model identity comes from the canonical stacked checkpoint. No original
 * text, file path, generated code or raw prompt is included in the report.
 * This is reproducibility/integrity evidence, NOT a signature, authorisation
 * to release, or proof that generated text is safe/true. Never execute output.
 * The operation is read-only and failure-atomic for outEvidence. */
UmiStatus UmiAiTrainingStackedGenerationEvidenceBuild(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy,
    uint64_t seed, const size_t *claimedTokens, size_t claimedCount,
    UmiAiTrainingStackedGenerationEvidence *outEvidence);

/* Re-run generation with the supplied model and compare all recorded fields.
 * Success means the record matches this execution; it cannot authenticate
 * an untrusted publisher because all digests are unkeyed. */
UmiStatus UmiAiTrainingStackedGenerationEvidenceVerify(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const size_t *claimedTokens, size_t claimedCount,
    const UmiAiTrainingStackedGenerationEvidence *expected);

#ifdef __cplusplus
}
#endif
#endif
