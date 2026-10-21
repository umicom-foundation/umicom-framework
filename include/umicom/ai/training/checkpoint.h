/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/checkpoint.h
 *
 * PURPOSE:
 *   Portable, bounded binary checkpoints for the experimental native decoder.
 *   Encoding/decoding belongs to Framework. Applications decide which approved
 *   source or destination may be used for a checkpoint file.
 *
 * FORMAT:
 *   Versioned little-endian integers and IEEE754 binary32 weight bits.
 *   A trailing FNV-1a checksum detects accidental corruption, NOT tampering.
 *   Do not load untrusted checkpoints or use checksum as authenticity proof.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_CHECKPOINT_H
#define UMICOM_AI_TRAINING_CHECKPOINT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/decoder.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Max bytes includes metadata, per-tensor dimensions and checksum. */
#define UMI_AI_DECODER_MAX_CHECKPOINT_BYTES 131072U

/* Compute exact required output size without modifying outSize on error. */
UmiStatus UmiAiTrainingDecoderCheckpointSize(
    const UmiAiTrainingDecoder *model, size_t *outSize);

/* Write the complete portable checkpoint into caller-owned memory. Errors
 * leave outBytes, outWritten and model untouched. Never expose raw tensor
 * storage or native struct layout directly. */
UmiStatus UmiAiTrainingDecoderCheckpointEncode(
    const UmiAiTrainingDecoder *model, uint8_t *outBytes,
    size_t capacity, size_t *outWritten);

/* Validate magic, format, lengths, shape, finiteness and full-file checksum
 * before publishing an owned, independent decoder. *outModel is unchanged
 * if any input is damaged, truncated, unsupported or oversized. */
UmiStatus UmiAiTrainingDecoderCheckpointDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingDecoder **outModel);

/* Save to a NEW file only. A same-directory staging file is fully written
 * before publication; an existing target is never intentionally replaced.
 * Only approved local paths should be passed by application policy.
 * The portable codec contains no keys or signatures, so file authenticity
 * must be enforced separately by the application/Framework security service. */
UmiStatus UmiAiTrainingDecoderCheckpointSaveNewFile(
    const UmiAiTrainingDecoder *model, const char *path);

/* Read one bounded regular checkpoint file, rejecting excess length before
 * decoding. On failure *outModel remains untouched. */
UmiStatus UmiAiTrainingDecoderCheckpointLoadFile(
    const char *path, UmiAiTrainingDecoder **outModel);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_CHECKPOINT_H */
