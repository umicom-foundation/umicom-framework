/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/byte_tokenizer.c
 * PURPOSE: Lossless byte-level tokenisation without global state.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/byte_tokenizer.h"

UmiStatus UmiAiByteEncode(const uint8_t *bytes, size_t length,
                          uint32_t *tokens, size_t capacity,
                          size_t *outCount)
{
    if (outCount == NULL || (length != 0U && (bytes == NULL || tokens == NULL))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < length; ++index) {
        tokens[index] = (uint32_t)bytes[index] + UMI_AI_BYTE_OFFSET;
    }
    *outCount = length;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiByteDecode(const uint32_t *tokens, size_t count,
                          uint8_t *bytes, size_t capacity,
                          size_t *outCount)
{
    if (outCount == NULL || (count != 0U && (tokens == NULL || bytes == NULL))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (count > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Validate all IDs first: bad input cannot mutate caller-owned buffers. */
    for (size_t index = 0U; index < count; ++index) {
        if (tokens[index] < UMI_AI_BYTE_OFFSET ||
            tokens[index] >= UMI_AI_BYTE_VOCABULARY) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    for (size_t index = 0U; index < count; ++index) {
        bytes[index] = (uint8_t)(tokens[index] - UMI_AI_BYTE_OFFSET);
    }
    *outCount = count;
    return UMI_STATUS_OK;
}
