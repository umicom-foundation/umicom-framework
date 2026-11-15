/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/utf8_stream.h
 *
 * PURPOSE:
 *   Reconstruct strict UTF-8 scalars across byte chunks using the shared Base decoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BASE_UTF8_STREAM_H
#define UMICOM_BASE_UTF8_STREAM_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/* A zero-initialized stream is ready. Copy this value to stage a transaction;
 * serialize access, and do not edit fields except through Reset/Append. */
typedef struct UmiUtf8Stream {
    uint8_t pending[4];
    size_t pendingCount;
    size_t expectedBytes;
    uint64_t completedScalars;
    uint64_t completedBytes;
} UmiUtf8Stream;
typedef struct UmiUtf8StreamInfo {
    uint64_t completedScalars;
    uint64_t completedBytes;
    size_t bytesNeeded;
} UmiUtf8StreamInfo;

UmiStatus UmiUtf8StreamReset(UmiUtf8Stream *stream);
/* Accept one byte. A partial scalar publishes produced=0 and no output bytes.
 * A complete scalar publishes its original 1..4 UTF-8 bytes. Embedded NUL is a
 * valid scalar; this API is not a NUL-terminated string interface.
 * Failure preserves the stream, output bytes and produced. Output storage and
 * produced must not overlap each other or stream. Output may be NULL only with
 * capacity=0; a completed scalar then returns CAPACITY_EXCEEDED for retry.
 * Invalid Unicode is PARSE_ERROR; a malformed public state is INVALID_STATE. */
UmiStatus UmiUtf8StreamAppend(UmiUtf8Stream *stream,uint8_t byte,
    uint8_t *output,size_t capacity,size_t *produced);
/* Check completeness without retiring the stream. A partial scalar returns
 * INVALID_STATE and remains available for a later Append. */
UmiStatus UmiUtf8StreamFinish(const UmiUtf8Stream *stream);
/* Copy counters and outstanding byte count. Failure preserves out_info. */
UmiStatus UmiUtf8StreamInspect(const UmiUtf8Stream *stream,UmiUtf8StreamInfo *out_info);
#ifdef __cplusplus
}
#endif
#endif
