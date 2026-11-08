/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/request_stream.h
 *
 * PURPOSE:
 *   Own and incrementally frame one bounded HTTP request while reusing the shared strict parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_REQUEST_STREAM_H
#define UMICOM_WEB_REQUEST_STREAM_H
#include "umicom/web/connection.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebRequestStream UmiWebRequestStream;
typedef struct UmiWebRequestStreamSnapshot {
    size_t received_bytes;
    size_t maximum_bytes;
    /* The expected frame is known only after its complete header is admitted.
     * It describes framing, not successful dispatch or client authorization. */
    UmiWebRequestFrame frame;
    UmiStatus status;
} UmiWebRequestStreamSnapshot;

/* Create one owner-thread accumulator with 1..REQUEST_LIMIT bytes of storage.
 * No sockets, clocks, handlers or implicit endpoint. *out is NULL on failure.
 * Destroy accepts NULL. Neither function establishes a transport deadline. */
UmiStatus UmiWebRequestStreamCreate(size_t maximum_bytes, UmiWebRequestStream **out);
void UmiWebRequestStreamDestroy(UmiWebRequestStream *stream);

/* Copy a complete chunk into owned storage. BUSY means more input is needed;
 * OK means exactly one complete request. Empty input reports current status.
 * A NULL nonempty input is invalid and preserves the stream. Capacity/framing
 * errors are sticky until Reset; a refused oversized chunk is not copied.
 * Additional bytes after completion turn the stream into PARSE_ERROR. This
 * API never silently consumes a second pipelined request. No partial-consume
 * result is provided: the caller must stop using this request on an error.
 * Header semantics are checked once the complete header arrives. CR/LF
 * errors can be rejected sooner. The complete body may contain any bytes.
 * Transport waits and synchronous application work need separate bounds. */
UmiStatus UmiWebRequestStreamFeed(UmiWebRequestStream *stream, const void *bytes, size_t length);

/* Copy diagnostics without exposing a borrowed buffer. Return OK for a valid
 * object even when snapshot.status records a framing error. Clear the output
 * on invalid input. Snapshotting does not advance or reset the stream. */
UmiStatus UmiWebRequestStreamInspect(const UmiWebRequestStream *stream,
                                  UmiWebRequestStreamSnapshot *out);
/* Parse a complete request into caller-owned storage through ParseStrict.
 * BUSY/errors leave the destination unchanged. No handler is dispatched. */
UmiStatus UmiWebRequestStreamParse(const UmiWebRequestStream *stream, UmiWebRequest *out);
/* Explicitly begin another independent request with the same capacity.
 * Discard the prior bytes and framing state; no transport is kept alive. */
UmiStatus UmiWebRequestStreamReset(UmiWebRequestStream *stream);
#ifdef __cplusplus
}
#endif
#endif
