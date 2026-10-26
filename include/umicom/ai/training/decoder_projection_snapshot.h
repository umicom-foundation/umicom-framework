/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/decoder_projection_snapshot.h
 * PURPOSE:
 *   Preserve and restore the *real* frozen-decoder Transformer projection
 *   adapters through the existing portable per-projection checkpoint codec.
 *
 * OWNERSHIP AND TRUST:
 *   The existing decoder is borrowed and remains the immutable model owner.
 *   A restored adapter owns its independent tape; destroy it before decoder.
 *   This unsigned SHA-256-protected snapshot checks accidental corruption and
 *   exact frozen-checkpoint identity; it does not authenticate a publisher.
 *   Apply existing Framework publisher/authorisation policy before admission.
 *   This format stores adapter SGD weights and step count, not AdamW moments.
 *   No decoder checkpoint, original numerical operator or public ABI changes.
 *   Output values remain unchanged after rejected operations.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_DECODER_PROJECTION_SNAPSHOT_H
#define UMICOM_AI_TRAINING_DECODER_PROJECTION_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/decoder_projection_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_PROJECTION_SNAPSHOT_MAX_BYTES 262144U

typedef struct UmiAiTrainingDecoderProjectionSnapshotInfo {
    uint32_t selectedProjections;
    size_t rank;
    size_t adapterCheckpointCount;
    uint64_t frozenDecoderSteps;
    uint64_t completedAdapterSteps;
    size_t byteCount;
    char frozenCheckpointSha256[UMI_AI_DECODER_ADAPTER_SHA256_HEX_CAPACITY];
} UmiAiTrainingDecoderProjectionSnapshotInfo;

/* Returns the exact capacity needed for the complete portable snapshot.
 * The snapshot contains one established per-projection checkpoint for every
 * selected Transformer role, and a whole-record SHA-256 trailer. */
UmiStatus UmiAiTrainingDecoderProjectionSnapshotSize(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    size_t *outBytes);

/* Stage, verify and export all six possible projection roles in a fixed
 * order. No partial bytes or outWritten count are published on failure.
 * This operation does not write a file or mutate the adapter/decoder. */
UmiStatus UmiAiTrainingDecoderProjectionSnapshotEncode(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    uint8_t *outBytes, size_t capacity, size_t *outWritten);

/* Inspect an exact-length, canonical snapshot. Full outer SHA-256, inner
 * projection digests, format, bounds and nonselected-role layout are checked;
 * source authenticity and compatibility with a *live* model are NOT proved. */
UmiStatus UmiAiTrainingDecoderProjectionSnapshotInspect(
    const uint8_t *bytes, size_t count,
    UmiAiTrainingDecoderProjectionSnapshotInfo *outInfo);

/* Validate the frozen decoder's actual canonical checkpoint identity before
 * allocating a new tape and import each existing Framework adapter checkpoint
 * into that tape. No existing adapter is modified and no weights are guessed.
 * Initialise *outAdapter=NULL. A failure leaves it unchanged and releases all
 * temporary allocations. Completed SGD steps are restored exactly. */
UmiStatus UmiAiTrainingDecoderProjectionSnapshotRestore(
    UmiAiTrainingDecoder *frozenDecoder,
    const uint8_t *bytes, size_t count,
    UmiAiTrainingDecoderProjectionAdapter **outAdapter);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_DECODER_PROJECTION_SNAPSHOT_H */
