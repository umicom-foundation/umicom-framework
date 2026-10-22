/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/training_run_lock.h
 *
 * PURPOSE:
 *   Identify the approved dataset snapshot, AdamW training decisions and
 *   exact checkpoint bytes before reproducible clipped training resumes.
 *
 * ARCHITECTURE AND SECURITY:
 *   The combined binary checkpoint and the original .clip-policy format are
 *   unchanged. The optional .training-lock companion adds an independent
 *   deterministic consistency check. No files are opened by Format/Verify;
 *   the dataset must already be loaded through Framework's reviewed loader.
 *   Snapshot identity includes existing FNV-1a document fingerprints, which
 *   are not collision-resistant. Re-hashing those fingerprints with SHA-256
 *   does NOT make the underlying source identity cryptographically secure.
 *   Nor does any unsigned record establish authenticity or trusted execution.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_RUN_LOCK_H
#define UMICOM_AI_TRAINING_RUN_LOCK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/gradient_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_TRAINING_RUN_LOCK_RECORD_CAPACITY 384U

/* All members are borrowed for a single synchronous call. The plan's `steps`
 * is deliberately NOT part of the run identity: a partial run and its later
 * continuation have different end steps but must retain the SAME absolute
 * learning-rate horizon, decoder, seeds, evaluation policy, AdamW parameters,
 * effective batch and clipping threshold. Caller must independently confirm
 * that the checkpoint step is not later than the requested new end step. */
typedef struct UmiAiTrainingRunLockInputs {
    const UmiAiTrainingDataset *dataset;
    const UmiAiTrainingExperimentConfig *experiment;
    const UmiAiTrainingAdamWConfig *adamw;
    const UmiAiTrainingLearningRateSchedule *schedule;
    const UmiAiTrainingGradientPolicy *clipping;
} UmiAiTrainingRunLockInputs;

/* Canonical ASCII record with three SHA-256 digests: raw combined checkpoint
 * bytes, ordered immutable dataset metadata, and complete stable run-policy
 * fields (excluding the permitted final-step stopping point). No ABI padding,
 * pointer values, locale-sensitive floating-point strings, or host endian
 * encodings enter the digest. Rejected calls preserve BOTH outputs. */
UmiStatus UmiAiTrainingRunLockFormat(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingRunLockInputs *inputs,
    char *outRecord, size_t capacity, size_t *outWritten);

/* Byte-for-byte verification; altered order, extra keys, newline changes,
 * changed schedule/seeds/dataset information or checkpoint bytes are refused. */
UmiStatus UmiAiTrainingRunLockVerify(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingRunLockInputs *inputs,
    const char *record, size_t recordLength);

/* Exclusive creation and bounded read of the companion file. Never overwrite
 * another experiment's evidence. A power loss across multiple output files
 * can leave an incomplete pair; callers MUST refuse missing records. */
UmiStatus UmiAiTrainingRunLockSaveNewFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingRunLockInputs *inputs);
UmiStatus UmiAiTrainingRunLockVerifyFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingRunLockInputs *inputs);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_RUN_LOCK_H */
