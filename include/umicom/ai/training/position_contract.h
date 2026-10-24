/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/position_contract.h
 *
 * PURPOSE:
 *   Bind an approved native decoder checkpoint to its existing learned-
 *   absolute positional embedding strategy and to an explicit generation
 *   context policy. Provide a portable, separately stored identity record.
 *
 * ARCHITECTURE AND COMPATIBILITY:
 *   The existing decoder, checkpoint codec and Framework SHA-256 are the only
 *   authorities for model bytes and identity. This service never modifies a
 *   checkpoint, reinterprets position embeddings or introduces another model.
 *   Historical BOS-left-padded generation and the opt-in unpadded prefix
 *   generator are DIFFERENT inference policies, even for identical weights.
 *   A checkpoint using learned positions cannot be silently re-labelled as
 *   rotary / relative-position inference. Such a change needs a separately
 *   trained or explicitly qualified model and checkpoint format.
 *
 * OWNERSHIP AND SECURITY:
 *   All inputs and outputs belong to the caller. The decoder is borrowed and
 *   must not be trained, destroyed or accessed concurrently with these calls.
 *   Outputs remain byte-for-byte unchanged after a rejected call. The record
 *   has a SHA-256 content digest for accidental-damage detection; it is NOT
 *   signed and provides NO authenticity guarantee. Apply the existing Umicom
 *   trusted-publisher policy before trusting a model or external sidecar.
 *   Never load untrusted checkpoint bytes merely to inspect this record.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_POSITION_CONTRACT_H
#define UMICOM_AI_TRAINING_POSITION_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/checkpoint.h"
#include "umicom/base/sha256.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_POSITION_CONTRACT_FORMAT 1U
#define UMI_AI_POSITION_CONTRACT_RECORD_BYTES 128U

/* These values are serialized as unsigned little-endian 32-bit integers.
 * There is deliberately no advertised implementation of rotary, ALiBi or
 * another positional strategy for the CURRENT decoder/checkpoint family. */
typedef enum UmiAiTrainingPositionEncoding {
    UMI_AI_POSITION_ENCODING_LEARNED_ABSOLUTE = 1
} UmiAiTrainingPositionEncoding;

typedef enum UmiAiTrainingPositionContext {
    UMI_AI_POSITION_CONTEXT_BOS_LEFT_PADDED = 1,
    UMI_AI_POSITION_CONTEXT_GROWING_UNPADDED = 2
} UmiAiTrainingPositionContext;

typedef enum UmiAiTrainingPositionRollover {
    UMI_AI_POSITION_ROLLOVER_REBUILD = 1
} UmiAiTrainingPositionRollover;

/* Do not serialize this C struct directly: padding, size_t widths and native
 * endianness are not a stable file format. Encode/Decode own the wire format. */
typedef struct UmiAiTrainingPositionContract {
    uint32_t formatVersion;
    uint32_t positionEncoding;
    uint32_t generationContext;
    uint32_t rolloverBehaviour;
    UmiAiTrainingDecoderConfig decoderConfig;
    uint64_t completedTrainingSteps;
    uint32_t checkpointBytes;
    uint8_t checkpointSha256[UMI_SHA256_BYTES];
} UmiAiTrainingPositionContract;

/* Build an identity record from the Framework's canonical encoded checkpoint.
 * The bytes are hashed with the EXISTING Framework SHA-256 implementation.
 * Requires a supported explicit context policy; both policies currently use
 * the same learned-absolute weight layout, but yield different sample paths.
 * No disk access, model write or checkpoint-format modification occurs. */
UmiStatus UmiAiTrainingPositionContractBuild(
    const UmiAiTrainingDecoder *model,
    UmiAiTrainingPositionContext context,
    UmiAiTrainingPositionContract *outContract);

/* Encode exactly 128 bytes in a canonical, versioned little-endian sidecar.
 * Layout: 8-byte magic and fixed-width fields; see public docs for the
 * fixed offsets. The last 32 bytes hash the first 96 record bytes.
 * Output capacity is measured in BYTES. Output objects must not overlap. */
UmiStatus UmiAiTrainingPositionContractEncode(
    const UmiAiTrainingPositionContract *contract,
    uint8_t *outBytes, size_t capacity, size_t *outWritten);

/* Strictly parse one exact-length, canonical sidecar; refuse unknown formats,
 * encoding strategies, context policies, malformed configuration, tampering
 * and additional trailing bytes. Decode performs no model loading or I/O.
 * A valid unsigned sidecar may still be malicious or misattributed. */
UmiStatus UmiAiTrainingPositionContractDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingPositionContract *outContract);

/* Re-encode the live model, compare SHA-256 and the complete model identity,
 * and refuse an unexpected generation policy. The contract does NOT perform
 * generation or guarantee that future model algorithms are interoperable.
 * INVALID_STATE means the declared contract cannot be applied to this model;
 * INVALID_ARGUMENT identifies a malformed request. */
UmiStatus UmiAiTrainingPositionContractVerify(
    const UmiAiTrainingDecoder *model,
    const UmiAiTrainingPositionContract *expected,
    UmiAiTrainingPositionContext requestedContext);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_POSITION_CONTRACT_H */
