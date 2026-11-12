/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/input_stream.h
 *
 * PURPOSE:
 *   Read large regular files in bounded chunks without allocating their full size.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_INPUT_STREAM_H
#define UMICOM_PLATFORM_INPUT_STREAM_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Bytes are borrowed only during the synchronous callback. Return a failure to
 * stop reading (including CANCELLED). No callback may close the owned handle.
 * A callback can see bytes before a later I/O/change failure: consumers must
 * publish their result only when the whole visit returns OK. */
typedef UmiStatus (*UmiInputChunkVisitor)(const unsigned char *bytes, size_t size, void *context);
/* Ordinary absolute UTF-8 path, existing trusted parents, regular final leaf,
 * no final symlink/reparse point. Like UmiInputFileRead, this checks metadata
 * before and after reading through the same handle. It is not an immutable
 * snapshot or a security sandbox. maximumBytes is enforced before callbacks.
 * Uses a 64 KiB stack buffer, even for multi-gigabyte files. outSize is cleared
 * on failure. Blocking I/O belongs on an application's worker thread. */
UmiStatus UmiInputFileVisit(const char *path, uint64_t maximumBytes,
    UmiInputChunkVisitor visitor, void *context, uint64_t *outSize);
#ifdef __cplusplus
}
#endif
#endif
