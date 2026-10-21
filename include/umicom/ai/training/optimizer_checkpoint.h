/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/optimizer_checkpoint.h
 *
 * PURPOSE:
 *   Persist a native decoder together with its AdamW moment state so that
 *   interrupted deterministic training can be continued exactly.
 *
 * ARCHITECTURE AND SECURITY:
 *   A new, bounded binary envelope stores the existing unmodified decoder
 *   checkpoint plus all AdamW moments and their parameter shapes. The decoder
 *   checkpoint format remains unchanged and continues to support SGD users.
 *   All public output pointers are failure-atomic; successful load publishes
 *   two independently owned handles (destroy optimiser BEFORE decoder).
 *   The envelope checksum detects accidental corruption, NOT malicious
 *   tampering. Accept only reviewed local checkpoints whose authenticity is
 *   established by the caller. Training plans and source-file identities must
 *   also match the original run to claim exact continuation.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_OPTIMIZER_CHECKPOINT_H
#define UMICOM_AI_TRAINING_OPTIMIZER_CHECKPOINT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/optimizer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES (8U * 1024U * 1024U)

/* Exact encoded length. The optimiser MUST be bound to this decoder, with
 * matching completed steps. Invalid/stale bindings are refused. */
UmiStatus UmiAiTrainingAdamWCheckpointSize(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingAdamW *optimizer, size_t *outSize);

/* Portable IEEE754 binary32/binary64 and little-endian envelope. Caller
 * retains both handles; no model weights or optimiser moments are changed. */
UmiStatus UmiAiTrainingAdamWCheckpointEncode(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingAdamW *optimizer,
    uint8_t *outBytes, size_t capacity, size_t *outWritten);

/* Validate all sizes, tensor dimensions, finite/nonnegative second moments,
 * nested decoder checkpoint and checksums BEFORE publication. On failure
 * neither output pointer is modified, and no partial model escapes. */
UmiStatus UmiAiTrainingAdamWCheckpointDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer);

/* Write only a NEW file using a same-directory staging file and no-clobber
 * publication. Never silently replace earlier training evidence. */
UmiStatus UmiAiTrainingAdamWCheckpointSaveNewFile(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingAdamW *optimizer, const char *path);

/* Load a bounded approved local file and transfer two owned handles on
 * complete success; never execute checkpoint contents. */
UmiStatus UmiAiTrainingAdamWCheckpointLoadFile(
    const char *path, UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_OPTIMIZER_CHECKPOINT_H */
