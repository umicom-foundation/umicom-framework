/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/source_reader.h
 *
 * PURPOSE:
 *   Adapt bounded native file input to CodeGuard byte and text analysis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CODEGUARD_SOURCE_READER_H
#define UMICOM_CODEGUARD_SOURCE_READER_H
#include "umicom/platform/input_stream.h"
#include "umicom/platform/cancellation.h"

/* Private adapter: callers keep candidate results unpublished until success.
 * Original path spelling remains with the caller for diagnostic identity.
 * Missing files map to CodeGuard's existing IO_ERROR convention. */
UmiStatus umi_codeguard_source_visit_bytes(const char *path, uint64_t maximum,
    UmiInputChunkVisitor visitor, void *context,
    const UmiCancellationToken *cancellation);

/* Preserve the historical fgets segmentation: at most 8191 input bytes,
 * newline included, then one terminator not counted in size. A physical line
 * longer than that produces several numbered segments. This is a heuristic,
 * not a parser. Embedded NUL is INVALID_ARGUMENT for textual analysis.
 * Borrowed text is valid only during the callback. Earlier callbacks may
 * have run when a later read, metadata or close failure is reported. */
typedef UmiStatus (*UmiCodeGuardLineVisitor)(const char *text, size_t size,
    size_t number, void *context);
UmiStatus umi_codeguard_source_visit_lines(const char *path, uint64_t maximum,
    UmiCodeGuardLineVisitor visitor, void *context,
    const UmiCancellationToken *cancellation);
#endif
