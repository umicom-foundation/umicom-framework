/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/utf8_stream.c
 * PURPOSE:
 *   Safely assemble length-delimited UTF-8 scalars without converting binary
 *   model output to a guessed text encoding or allocating unbounded buffers.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/utf8_stream.h"

#include <stdint.h>
#include <string.h>

static int Utf8StateValid(const UmiUtf8Stream *stream)
{
    if (stream == NULL || stream->bufferedCount > 3U ||
        stream->expectedCount > 4U) return 0;
    if (stream->bufferedCount == 0U)
        return stream->expectedCount == 0U;
    return stream->expectedCount >= 2U &&
           stream->bufferedCount < stream->expectedCount;
}

UmiStatus UmiUtf8StreamReset(UmiUtf8Stream *stream)
{
    if (stream == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *stream = (UmiUtf8Stream){0};
    return UMI_STATUS_OK;
}

UmiStatus UmiUtf8StreamInspect(const UmiUtf8Stream *stream,
                               UmiUtf8StreamInfo *outInfo)
{
    if (!Utf8StateValid(stream) || outInfo == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiUtf8StreamInfo candidate = {
        .bufferedBytes = (size_t)stream->bufferedCount,
        .bytesNeeded = (size_t)(stream->expectedCount - stream->bufferedCount),
        .completedScalars = stream->completedScalars,
        .completedBytes = stream->completedBytes
    };
    *outInfo = candidate;
    return UMI_STATUS_OK;
}

/* Enforce canonical UTF-8 scalar ranges at the SECOND byte. Subsequent
 * continuation bytes must be 80..BF, and all lengths are already bounded. */
static int Utf8ContinuationValid(uint8_t lead, uint8_t byte, size_t index)
{
    if (byte < 0x80U || byte > 0xbfU) return 0;
    if (index != 1U) return 1;
    if (lead == 0xe0U && byte < 0xa0U) return 0;
    if (lead == 0xedU && byte > 0x9fU) return 0;
    if (lead == 0xf0U && byte < 0x90U) return 0;
    if (lead == 0xf4U && byte > 0x8fU) return 0;
    return 1;
}

UmiStatus UmiUtf8StreamAppend(
    UmiUtf8Stream *stream, uint8_t nextByte,
    uint8_t *outUtf8, size_t outputCapacity, size_t *outBytes)
{
    UmiUtf8Stream candidate;
    size_t completed = 0U;
    if (!Utf8StateValid(stream) || outUtf8 == NULL || outBytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    candidate = *stream;
    if (candidate.bufferedCount == 0U) {
        if (nextByte < 0x80U) {
            candidate.expectedCount = 1U;
        } else if (nextByte >= 0xc2U && nextByte <= 0xdfU) {
            candidate.expectedCount = 2U;
        } else if (nextByte >= 0xe0U && nextByte <= 0xefU) {
            candidate.expectedCount = 3U;
        } else if (nextByte >= 0xf0U && nextByte <= 0xf4U) {
            candidate.expectedCount = 4U;
        } else {
            return UMI_STATUS_PARSE_ERROR;
        }
    } else if (!Utf8ContinuationValid(candidate.buffered[0],
                                     nextByte, candidate.bufferedCount)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    candidate.buffered[candidate.bufferedCount++] = nextByte;
    if (candidate.bufferedCount == candidate.expectedCount) {
        completed = candidate.expectedCount;
        if (completed > outputCapacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (candidate.completedScalars == UINT64_MAX ||
            candidate.completedBytes > UINT64_MAX - (uint64_t)completed)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        ++candidate.completedScalars;
        candidate.completedBytes += (uint64_t)completed;
    }
    /* Commit an accepted byte only after all validity and capacity checks.
     * For incomplete scalars, the output storage is unchanged. */
    if (completed != 0U) {
        memcpy(outUtf8, candidate.buffered, completed);
        memset(candidate.buffered, 0, sizeof(candidate.buffered));
        candidate.bufferedCount = 0U;
        candidate.expectedCount = 0U;
    }
    *stream = candidate;
    *outBytes = completed;
    return UMI_STATUS_OK;
}

UmiStatus UmiUtf8StreamFinish(const UmiUtf8Stream *stream)
{
    if (!Utf8StateValid(stream)) return UMI_STATUS_INVALID_ARGUMENT;
    return stream->bufferedCount == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
