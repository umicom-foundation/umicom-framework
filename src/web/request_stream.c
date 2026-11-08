/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/request_stream.c
 *
 * PURPOSE:
 *   Accumulate request fragments with a forward-only header scan and shared strict framing rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "request_stream_internal.h"
#include "connection_internal.h"
#include <stdlib.h>
#include <string.h>

struct UmiWebRequestStream {
    unsigned char *bytes;
    size_t maximum, received, scanned;
    UmiWebRequestFrame frame;
    UmiStatus status;
};

UmiStatus UmiWebRequestStreamCreate(size_t maximum, UmiWebRequestStream **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (maximum == 0U || maximum > UMI_WEB_CONNECTION_REQUEST_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiWebRequestStream *stream = calloc(1U, sizeof(*stream));
    if (stream == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    stream->bytes = malloc(maximum);
    if (stream->bytes == NULL) { free(stream); return UMI_STATUS_OUT_OF_MEMORY; }
    stream->maximum = maximum;
    stream->status = UMI_STATUS_BUSY;
    *out = stream;
    return UMI_STATUS_OK;
}
void UmiWebRequestStreamDestroy(UmiWebRequestStream *stream)
{
    if (stream == NULL) return;
    free(stream->bytes);
    free(stream);
}
UmiStatus UmiWebRequestStreamReset(UmiWebRequestStream *stream)
{
    if (stream == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Clear the previously received data for ordinary reuse. This is not a
     * cryptographic secret-erasure contract and does not close a connection. */
    memset(stream->bytes, 0, stream->received);
    stream->received = 0U;
    stream->scanned = 0U;
    memset(&stream->frame, 0, sizeof(stream->frame));
    stream->status = UMI_STATUS_BUSY;
    return UMI_STATUS_OK;
}
UmiStatus UmiWebRequestStreamInspect(const UmiWebRequestStream *stream,
                                  UmiWebRequestStreamSnapshot *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (stream == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out->received_bytes = stream->received;
    out->maximum_bytes = stream->maximum;
    out->frame = stream->frame;
    out->status = stream->status;
    return UMI_STATUS_OK;
}
UmiStatus UmiWebRequestStreamWritable(UmiWebRequestStream *stream, void **bytes, size_t *capacity)
{
    if (bytes != NULL) *bytes = NULL;
    if (capacity != NULL) *capacity = 0U;
    if (stream == NULL || bytes == NULL || capacity == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (stream->status != UMI_STATUS_BUSY) return UMI_STATUS_INVALID_STATE;
    *bytes = stream->bytes + stream->received;
    *capacity = stream->maximum - stream->received;
    return UMI_STATUS_OK;
}
UmiStatus UmiWebRequestStreamCommit(UmiWebRequestStream *stream, size_t length)
{
    if (stream == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length == 0U) return stream->status;
    if (stream->status != UMI_STATUS_BUSY) {
        if (stream->status == UMI_STATUS_OK) stream->status = UMI_STATUS_PARSE_ERROR;
        return stream->status;
    }
    if (length > stream->maximum - stream->received) {
        stream->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return stream->status;
    }
    stream->received += length;
    /* Only new header bytes are scanned. Once the header is admitted, body
     * completion is a size comparison: CR/LF and NUL are valid body bytes. */
    while (stream->frame.header_bytes == 0U && stream->scanned < stream->received) {
        const size_t i = stream->scanned++;
        const unsigned char value = stream->bytes[i];
        if ((value == '\n' && (i == 0U || stream->bytes[i - 1U] != '\r')) ||
            (i != 0U && stream->bytes[i - 1U] == '\r' && value != '\n')) {
            stream->status = UMI_STATUS_PARSE_ERROR;
            return stream->status;
        }
        if (i >= 3U && memcmp(stream->bytes + i - 3U, "\r\n\r\n", 4U) == 0) {
            stream->status = UmiWebRequestHeaderFrameRead(stream->bytes, i + 1U,
                                                        stream->maximum, &stream->frame);
            if (stream->status != UMI_STATUS_OK) return stream->status;
        }
    }
    if (stream->frame.header_bytes != 0U) {
        stream->status = stream->received > stream->frame.message_bytes ? UMI_STATUS_PARSE_ERROR :
                         stream->received == stream->frame.message_bytes ? UMI_STATUS_OK : UMI_STATUS_BUSY;
    } else {
        stream->status = stream->received == stream->maximum ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_BUSY;
    }
    return stream->status;
}
UmiStatus UmiWebRequestStreamFeed(UmiWebRequestStream *stream, const void *bytes, size_t length)
{
    if (stream == NULL || (bytes == NULL && length != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (length == 0U || stream->status != UMI_STATUS_BUSY ||
        length > stream->maximum - stream->received)
        return UmiWebRequestStreamCommit(stream, length);
    memmove(stream->bytes + stream->received, bytes, length);
    return UmiWebRequestStreamCommit(stream, length);
}
UmiStatus UmiWebRequestStreamParse(const UmiWebRequestStream *stream, UmiWebRequest *out)
{
    if (stream == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (stream->status != UMI_STATUS_OK) return stream->status;
    /* Keep final owned parsing authoritative. The incremental state provides
     * admission and storage, not a competing interpretation of HTTP fields. */
    return UmiWebRequestParseStrict(stream->bytes, stream->received, stream->maximum, out);
}
