/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/gradient_policy.h
 *
 * PURPOSE:
 *   Bind an explicitly selected global-gradient clipping policy to exact
 *   portable AdamW checkpoint bytes without changing checkpoint format.
 *
 * SECURITY:
 *   The companion record is an integrity/provenance consistency check, NOT
 *   an authentication signature. A malicious writer could replace both
 *   checkpoint and record; a reviewer must still approve their provenance.
 *   These functions never run user-supplied contents. Callers must approve
 *   model files before loading or resuming them.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_GRADIENT_POLICY_H
#define UMICOM_AI_TRAINING_GRADIENT_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/tape.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The stored IEEE754 binary32 bits eliminate ambiguity caused by decimal
 * formatting or locale. Exactly 0.0F is the explicitly disabled policy.
 * The full-batch historical path uses microbatchSequences=0. */
typedef struct UmiAiTrainingGradientPolicy {
    float maximumGradientNorm;
    uint32_t batchSequences;
    uint32_t microbatchSequences;
} UmiAiTrainingGradientPolicy;

#define UMI_AI_GRADIENT_POLICY_RECORD_CAPACITY 256U

/* Produce one canonical ASCII companion record from checkpoint bytes and an
 * approved policy. Outputs are all-or-nothing. Identifiers are SHA-256 hashes
 * of the supplied exact bytes and do not authenticate origin. */
UmiStatus UmiAiTrainingGradientPolicyFormat(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingGradientPolicy *policy,
    char *outRecord, size_t capacity, size_t *outWritten);

/* Verify an exact canonical record: no trailing or unknown keys, no duplicate
 * fields, no silent numeric conversion. Policy and data are borrowed. */
UmiStatus UmiAiTrainingGradientPolicyVerify(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingGradientPolicy *policy,
    const char *record, size_t recordLength);

/* Exclusive no-clobber publication, with an fsync/commit before close.
 * A partial newly created file is removed on error. Never replace an existing
 * policy record, even when it contains identical bytes. */
UmiStatus UmiAiTrainingGradientPolicySaveNewFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingGradientPolicy *policy);

UmiStatus UmiAiTrainingGradientPolicyVerifyFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingGradientPolicy *policy);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_GRADIENT_POLICY_H */
