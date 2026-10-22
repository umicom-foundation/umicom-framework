/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/training_source_lock.h
 *
 * PURPOSE:
 *   Bind a native AdamW checkpoint to cryptographic identity of the exact
 *   immutable source bytes already imported by the canonical dataset loader.
 *
 * COMPATIBILITY AND SECURITY:
 *   This optional .source-lock companion is ADDITIVE. The original combined
 *   checkpoint, .clip-policy and .training-lock formats stay untouched. The
 *   latter is still responsible for complete training-setting identity.
 *   Callers must verify ALL records before attempting strict continuation.
 *   SHA-256 detects changes to the reviewed snapshot, but unsigned records
 *   cannot authenticate the publisher or establish host trust. Paths must be
 *   independently approved; none of these functions is a filesystem sandbox.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SOURCE_LOCK_H
#define UMICOM_AI_TRAINING_SOURCE_LOCK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/dataset_manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_TRAINING_SOURCE_LOCK_RECORD_CAPACITY 256U

/* Format/Verify accept canonical combined checkpoint bytes supplied by the
 * caller. They do not decode the checkpoint or re-read its path. Use the
 * existing checkpoint codec and the existing strict run lock separately.
 * The record covers the SHA-256 of the checkpoint bytes and the SHA-256
 * source-snapshot identity (including document paths, ordering and splits).
 * Failure preserves outRecord and outWritten. */
UmiStatus UmiAiTrainingSourceLockFormat(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingDataset *dataset,
    char *outRecord, size_t capacity, size_t *outWritten);

UmiStatus UmiAiTrainingSourceLockVerify(
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingDataset *dataset,
    const char *record, size_t recordLength);

/* Companion file publication refuses an occupied destination. Corrupt,
 * missing, oversized, truncated, or noncanonical records fail closed. File
 * publication and the checkpoint/other sidecars are NOT one atomic action. */
UmiStatus UmiAiTrainingSourceLockSaveNewFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingDataset *dataset);

UmiStatus UmiAiTrainingSourceLockVerifyFile(
    const char *recordPath,
    const uint8_t *checkpointBytes, size_t checkpointByteCount,
    const UmiAiTrainingDataset *dataset);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_SOURCE_LOCK_H */
