/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/utf8_stream.h
 * PURPOSE:
 *   Validate UTF-8 incrementally for native AI output and general text UIs.
 *   The Framework byte tokeniser retains raw bytes; this component only
 *   assembles complete, strict RFC 3629 Unicode scalar sequences for display.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BASE_UTF8_STREAM_H
#define UMICOM_BASE_UTF8_STREAM_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Caller-owned, value-semantic streaming state, without global data or heap.
 * Initialise with Reset before use. Fields are exposed for stack allocation;
 * applications should inspect state through Inspect rather than editing them.
 */
typedef struct UmiUtf8Stream {
    uint8_t buffered[4];
    uint8_t bufferedCount;
    uint8_t expectedCount;
    uint64_t completedScalars;
    uint64_t completedBytes;
} UmiUtf8Stream;

typedef struct UmiUtf8StreamInfo {
    size_t bufferedBytes;
    size_t bytesNeeded;
    uint64_t completedScalars;
    uint64_t completedBytes;
} UmiUtf8StreamInfo;

/* Reset only the caller-owned stream. No user-visible output is generated. */
UmiStatus UmiUtf8StreamReset(UmiUtf8Stream *stream);

/* Accept ONE raw byte, stage strict UTF-8 validity and publish one complete
 * scalar only after its final byte. For incomplete prefixes, *outBytes=0 and
 * the output buffer is untouched. Completed UTF-8 scalars are 1..4 bytes;
 * outputCapacity is in bytes and must cover the current complete scalar.
 *
 * This rejects overlong encodings, UTF-16 surrogates, out-of-range code
 * points, invalid starts and malformed continuation sequences. On any error,
 * both the stream state and all caller outputs remain byte-for-byte unchanged.
 * Embedded NUL (U+0000) is valid UTF-8; outputs are NOT C strings. Applications
 * separately decide how control characters and untrusted generated text are
 * displayed. Caller must not alias stream, outputs or each other. */
UmiStatus UmiUtf8StreamAppend(
    UmiUtf8Stream *stream, uint8_t nextByte,
    uint8_t *outUtf8, size_t outputCapacity, size_t *outBytes);

/* Refuse finalisation with an incomplete scalar. Does not modify stream.
 * No replacement character is silently introduced. */
UmiStatus UmiUtf8StreamFinish(const UmiUtf8Stream *stream);

/* Copy counters and pending-byte state; errors preserve caller output. */
UmiStatus UmiUtf8StreamInspect(const UmiUtf8Stream *stream,
                               UmiUtf8StreamInfo *outInfo);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BASE_UTF8_STREAM_H */
