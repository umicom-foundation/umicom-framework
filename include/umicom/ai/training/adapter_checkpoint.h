/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/adapter_checkpoint.h
 * PURPOSE:
 *   Preserve small trained low-rank adapter weights in a portable, integrity-
 *   checked snapshot, bound to the exact immutable base projection.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_ADAPTER_CHECKPOINT_H
#define UMICOM_AI_TRAINING_ADAPTER_CHECKPOINT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/low_rank.h"

#ifdef __cplusplus
extern "C" {
#endif

/* An adapter checkpoint is NOT a complete decoder checkpoint or an AdamW
 * continuation checkpoint. It contains only the two low-rank trainable
 * matrices and an SHA-256 identity for the frozen base projection.
 *
 * Bytes are portable: eight-byte magic, little-endian format revision and
 * dimensions, 32-byte base digest, IEEE-754 binary32 adapter values in row-
 * major little-endian order, and a final SHA-256 of all preceding bytes.
 * The format is bounded by the existing Framework training-tape limits.
 * SHA-256 detects damage and mismatches, not a trusted publisher signature. */
typedef struct UmiAiTrainingAdapterCheckpointInfo {
    size_t inputChannels;
    size_t outputChannels;
    size_t rank;
    size_t byteCount;
    unsigned char baseDigest[32];
} UmiAiTrainingAdapterCheckpointInfo;

/* Query the complete encoded snapshot size for this projection. The caller
 * owns the projection and retains its tape while the query executes. */
UmiStatus UmiAiTrainingAdapterCheckpointSize(
    const UmiAiTrainingLowRankProjection *projection, size_t *outBytes);

/* Export to caller-provided bytes. Output is committed only after all tensor
 * values and the complete checksum are validated. A failure leaves the caller
 * buffer and *outBytesWritten unchanged. This call performs no file I/O. */
UmiStatus UmiAiTrainingAdapterCheckpointExport(
    const UmiAiTrainingLowRankProjection *projection,
    void *outBytes, size_t capacity, size_t *outBytesWritten);

/* Read and validate the complete snapshot including SHA-256 and finite FP32
 * values, without creating a tape or exposing trainable pointers. */
UmiStatus UmiAiTrainingAdapterCheckpointInspect(
    const void *bytes, size_t byteCount,
    UmiAiTrainingAdapterCheckpointInfo *outInfo);

/* Create THREE new tape-owned leaves: caller-supplied frozen base, restored
 * trainable down/up matrices. The checkpoint's SHA-256 base identity must
 * match the exact supplied FP32 base bytes in the declared dimensions.
 * The caller must not rewind past these leaves. The import never mutates an
 * existing projection and is failure-atomic with respect to the tape suffix
 * and *outProjection. It does not resume optimiser moments. */
UmiStatus UmiAiTrainingAdapterCheckpointImport(
    UmiAiTrainingTape *tape,
    const float *baseWeights, size_t baseValueCount,
    const void *bytes, size_t byteCount,
    UmiAiTrainingLowRankProjection *outProjection);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_ADAPTER_CHECKPOINT_H */
