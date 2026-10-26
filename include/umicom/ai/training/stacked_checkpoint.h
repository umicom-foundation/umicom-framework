/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_checkpoint.h
 * PURPOSE: Portable, content-checked snapshots for the separately identified
 *          multi-layer native decoder family.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_CHECKPOINT_H
#define UMICOM_AI_TRAINING_STACKED_CHECKPOINT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/stacked_decoder.h"
#include "umicom/base/sha256.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_STACKED_CHECKPOINT_FORMAT 1U
#define UMI_AI_STACKED_CHECKPOINT_MAX_BYTES (2U * 1024U * 1024U)

typedef struct UmiAiTrainingStackedCheckpointInfo {
    UmiAiTrainingStackedDecoderConfig config;
    uint64_t completedTrainingSteps;
    size_t parameterTensors;
    size_t parameterElements;
    size_t checkpointBytes;
    uint8_t sha256[UMI_SHA256_BYTES];
} UmiAiTrainingStackedCheckpointInfo;

/* Format (NO native C struct layout is serialized):
 * 0..7  magic "UMISTK01"
 * 8..35 version, vocabulary, context, channels, hidden, heads, layers (u32 LE)
 * 36..51 seeded initialisation and completed training steps (u64 LE)
 * 52..59 parameter tensor count and total values (u32 LE)
 * then ordered tensor shapes (rows and cols, u32 LE), then contiguous
 * little-endian IEEE-754 float32 values, then 32-byte SHA-256 over the
 * entire preceding record. Exactly one record; trailing data is refused.
 * This is NOT a legacy single-block decoder checkpoint or an AdamW snapshot.
 * SHA-256 detects accidental corruption but is NOT publisher authentication.
 * Authorise source/checkpoint separately before decoding untrusted bytes. */
UmiStatus UmiAiTrainingStackedCheckpointSize(
    const UmiAiTrainingStackedDecoder *model, size_t *outBytes);
UmiStatus UmiAiTrainingStackedCheckpointExport(
    const UmiAiTrainingStackedDecoder *model,
    uint8_t *outBytes, size_t capacity, size_t *outWritten);
UmiStatus UmiAiTrainingStackedCheckpointInspect(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingStackedCheckpointInfo *outInfo);
UmiStatus UmiAiTrainingStackedCheckpointDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingStackedDecoder **outModel);
#ifdef __cplusplus
}
#endif
#endif
