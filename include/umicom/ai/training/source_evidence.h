/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/source_evidence.h
 * PURPOSE:
 *   Bind explicit training-data admission to the actual source and reviewed
 *   derivative bytes using the Framework's canonical SHA-256 implementation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SOURCE_EVIDENCE_H
#define UMICOM_AI_TRAINING_SOURCE_EVIDENCE_H

#include <stddef.h>
#include "umicom/ai/training/source_admission.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Verify both caller-declared SHA-256 hex digests against the exact original
 * asset bytes and prepared text bytes, THEN release an admitted source record.
 * The existing SourceAdmit service remains the policy/partition authority.
 * This check does not infer rights, validate textual accuracy, remove personal
 * data, detect near-duplicates or publish the dataset. The caller must ensure
 * the supplied bytes are immutable during the call and were reviewed by an
 * authorised person. All buffers remain caller-owned; no bytes are retained.
 * Empty inputs, missing buffers and invalid requests are rejected. Digest
 * disagreement returns INVALID_STATE. Rejection preserves *outSource. */
UmiStatus UmiAiTrainingSourceVerifyBytes(
    const UmiAiTrainingSourceReview *review,
    const void *originalBytes, size_t originalLength,
    const void *preparedTextBytes, size_t preparedTextLength,
    unsigned evaluationPercentage,
    UmiAiTrainingAdmittedSource *outSource);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_SOURCE_EVIDENCE_H */
