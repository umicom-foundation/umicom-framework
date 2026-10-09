/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/compiler_stream.h
 * PURPOSE: Recognise compiler records from arbitrary process-output chunks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DIAGNOSTICS_COMPILER_STREAM_H
#define UMICOM_DIAGNOSTICS_COMPILER_STREAM_H
#include "umicom/diagnostics/compiler_parser.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiCompilerDiagnosticStream UmiCompilerDiagnosticStream;
    /* A callback borrows fields only for its duration. OK supplies one complete
     * diagnostic. A lost physical line or logical record supplies NULL and
     * CAPACITY_EXCEEDED or INVALID_ARGUMENT (embedded NUL). Ordinary output has
     * no callback. The callback must not destroy, feed or finish this stream. */
    typedef void (*UmiCompilerDiagnosticObserver)(UmiStatus status,
                                                  const UmiCompilerDiagnosticFields *fields,
                                                  void *context);
    /**
     * Create a thread-confined parser. It retains at most one physical line
     * (8191 bytes) and one CMake record (32767 bytes), never the whole transcript.
     * The caller owns the returned handle and keeps observer/context valid until
     * destruction. On failure, a valid out pointer receives NULL.
     */
    UmiStatus UmiCompilerDiagnosticStreamCreate(UmiCompilerDiagnosticObserver observer,
                                                void *context, UmiCompilerDiagnosticStream **out);
    /**
     * Feed exactly length bytes in producer order, including split UTF-8 and ANSI
     * sequences. NULL is accepted only for zero length. Completed compiler lines
     * are emitted immediately; CMake records wait for their final continuation.
     * Malformed or oversized records are reported through observer and skipped
     * whole, so their suffix cannot be mistaken for a separate diagnostic.
     * OK means the bytes were accepted, not that every byte formed a diagnostic.
     */
    UmiStatus UmiCompilerDiagnosticStreamFeed(UmiCompilerDiagnosticStream *stream,
                                              const char *bytes, size_t length);
    /**
     * Finish after process completion, cancellation or failure. An unterminated
     * final physical line is accepted as the producer's final record. Finishing
     * twice is harmless; later Feed calls return INVALID_STATE. It does not
     * manufacture a successful process outcome from partial captured output.
     */
    UmiStatus UmiCompilerDiagnosticStreamFinish(UmiCompilerDiagnosticStream *stream);
    /**
     * Release the parser without emitting unfinished records. Finish explicitly
     * when the caller wants the final record delivered. NULL is accepted.
     */
    void UmiCompilerDiagnosticStreamDestroy(UmiCompilerDiagnosticStream *stream);
#ifdef __cplusplus
}
#endif
#endif
