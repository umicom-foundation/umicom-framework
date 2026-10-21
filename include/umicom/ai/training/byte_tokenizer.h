/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/byte_tokenizer.h
 *
 * PURPOSE:
 *   Define a reversible byte-level tokenizer for native LLM education.
 *   All 256 byte values round-trip without external vocabulary or locale.
 *
 * ARCHITECTURE:
 *   The Framework owns token semantics; applications consume this C23 ABI.
 *   Token IDs 0..3 are reserved; bytes map to ID = byte + 4.
 *   Caller owns all inputs and outputs. There are no hidden allocations.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_BYTE_TOKENIZER_H
#define UMICOM_AI_TRAINING_BYTE_TOKENIZER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_BYTE_PAD 0U
#define UMI_AI_BYTE_BOS 1U
#define UMI_AI_BYTE_EOS 2U
#define UMI_AI_BYTE_UNK 3U
#define UMI_AI_BYTE_OFFSET 4U
#define UMI_AI_BYTE_VOCABULARY 260U

/* Lengths are element counts, not terminator capacities. Encoding works on
 * arbitrary binary data, including embedded NUL. A failed call never writes
 * partial output. On success, *outCount is the number of written token IDs. */
UmiStatus UmiAiByteEncode(const uint8_t *bytes, size_t length,
                          uint32_t *tokens, size_t capacity,
                          size_t *outCount);

/* Decoding rejects reserved IDs: special-token handling belongs to an
 * explicit caller policy rather than silently changing text contents. */
UmiStatus UmiAiByteDecode(const uint32_t *tokens, size_t count,
                          uint8_t *bytes, size_t capacity,
                          size_t *outCount);
#ifdef __cplusplus
}
#endif
#endif
