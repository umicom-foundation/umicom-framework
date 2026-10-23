/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/model_quality_evidence.h
 *
 * PURPOSE:
 *   Export deterministic, bounded machine-readable evidence of an existing
 *   held-out model-quality comparison, without repeating its neural maths.
 *
 * INTEGRITY AND OWNERSHIP:
 *   Recalculate the policy decision from the supplied canonical audit records
 *   and reject stale/inconsistent reports. Fixed JSON keys require no caller-
 *   controlled string escaping. Exact binary64 bit strings avoid floating-
 *   point locale and rounding ambiguities. Audit FNV fingerprints identify
 *   sampled evidence only; this does NOT verify checkpoint signatures,
 *   dataset rights, signer identity, host integrity or production readiness.
 *   No model, filesystem, process or network access occurs in this service.
 *   All outputs remain byte-for-byte untouched on failure.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_MODEL_QUALITY_EVIDENCE_H
#define UMICOM_AI_TRAINING_MODEL_QUALITY_EVIDENCE_H

#include <stddef.h>
#include "umicom/ai/training/model_quality_gate.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_MODEL_QUALITY_EVIDENCE_JSON_CAPACITY 4096U

/* Render one valid quality decision as a NUL-terminated ASCII JSON object.
 * outWritten excludes the terminator. The caller must supply at least the
 * returned length plus one byte; an undersized buffer causes CAPACITY_EXCEEDED
 * with neither the output bytes nor outWritten changed.
 *
 * Binary64 values are encoded as EXACT 16-digit, lower-case hexadecimal bit
 * strings: this is deterministic across numeric locales and is deliberately
 * not a decimal JSON number. JSON consumers must decode the IEEE754 bits to
 * obtain the original double. Scores are sampled educational measurements;
 * a positive gate is not an independent release authorisation.
 *
 * The supplied report must match the decision recomputed using its embedded
 * audits and the supplied policy. This only guards against inconsistent
 * reports; it cannot establish the origin or authenticity of the audits. */
UmiStatus UmiAiTrainingModelQualityEvidenceJson(
    const UmiAiTrainingModelQualityReport *report,
    const UmiAiTrainingModelQualityPolicy *policy,
    char *outJson, size_t capacity, size_t *outWritten);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_MODEL_QUALITY_EVIDENCE_H */
