/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/request_stream_internal.h
 *
 * PURPOSE:
 *   Let the connection owner fill the request stream allocation directly without a second staging buffer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_REQUEST_STREAM_INTERNAL_H
#define UMICOM_WEB_REQUEST_STREAM_INTERNAL_H
#include "umicom/web/request_stream.h"
/* Borrow the unused tail only while the stream is BUSY. The connection owner
 * must bound its read by capacity and commit at most that count. Do not feed,
 * reset or destroy between these calls. Failed I/O need not be committed:
 * destruction still owns and releases the entire allocation. */
UmiStatus UmiWebRequestStreamWritable(UmiWebRequestStream *stream, void **bytes, size_t *capacity);
UmiStatus UmiWebRequestStreamCommit(UmiWebRequestStream *stream, size_t length);
#endif
