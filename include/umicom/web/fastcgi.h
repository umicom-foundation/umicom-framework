/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/fastcgi.h
 *
 * PURPOSE:
 *   Encode one FastCGI responder request and admit bounded records from its
 *   reply before an HTTP or process adapter interprets the worker output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_FASTCGI_H
#define UMICOM_WEB_FASTCGI_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WEB_FASTCGI_CONTENT_LIMIT UINT16_MAX

typedef enum UmiWebFastCgiType {
    UMI_WEB_FASTCGI_BEGIN_REQUEST = 1,
    UMI_WEB_FASTCGI_END_REQUEST = 3,
    UMI_WEB_FASTCGI_PARAMS = 4,
    UMI_WEB_FASTCGI_STDIN = 5,
    UMI_WEB_FASTCGI_STDOUT = 6,
    UMI_WEB_FASTCGI_STDERR = 7
} UmiWebFastCgiType;

typedef enum UmiWebFastCgiProtocolStatus {
    UMI_WEB_FASTCGI_REQUEST_COMPLETE = 0,
    UMI_WEB_FASTCGI_CANNOT_MULTIPLEX = 1,
    UMI_WEB_FASTCGI_OVERLOADED = 2,
    UMI_WEB_FASTCGI_UNKNOWN_ROLE = 3
} UmiWebFastCgiProtocolStatus;

typedef struct UmiWebFastCgiRecord {
    uint8_t type;
    uint16_t requestId;
    const uint8_t *body;
    size_t bodyBytes;
    size_t consumed;
} UmiWebFastCgiRecord;

typedef struct UmiWebFastCgiReply {
    uint16_t requestId;
    size_t bodyLimit;
    size_t stdoutBytes;
    size_t stderrBytes;
    uint32_t applicationStatus;
    UmiWebFastCgiProtocolStatus protocolStatus;
    int stdoutClosed;
    int stderrClosed;
    int complete;
} UmiWebFastCgiReply;

/** Start one responder request with a known nonzero ID. No connection reuse
 * flag is set. All encoding routines require independent input and output
 * storage. Failure preserves output bytes and the caller's written count.
 */
UmiStatus UmiWebFastCgiBegin(uint16_t requestId, uint8_t *buffer, size_t capacity,
    size_t *written);

/** Encode a PARAMS or STDIN record. An empty record ends that input stream.
 * Use exact binary contents and a length of at most 65,535 bytes.
 */
UmiStatus UmiWebFastCgiInput(UmiWebFastCgiType type, uint16_t requestId,
    const void *body, size_t bodyBytes, uint8_t *buffer, size_t capacity, size_t *written);

/** Encode a complete CGI name/value pair for a PARAMS record. The name uses
 * uppercase ASCII letters, digits and underscores, starting with a letter
 * or underscore. The value may be empty and cannot contain a NUL byte.
 * The transport owner chooses and validates which CGI variables to forward.
 */
UmiStatus UmiWebFastCgiParameter(const char *name, size_t nameBytes, const void *value,
    size_t valueBytes, uint8_t *buffer, size_t capacity, size_t *written);

/** Read one record, leaving any following bytes for the caller. An incomplete
 * header, body or padding returns UNAVAILABLE. A successful record borrows
 * its body from wire and reports the exact number of bytes consumed.
 */
UmiStatus UmiWebFastCgiParseRecord(const void *wire, size_t available, UmiWebFastCgiRecord *record);

/** Start tracking one reply. bodyLimit bounds the sum of STDOUT and STDERR.
 * Storage is caller-owned, and callers serialize access to a reply state.
 */
UmiStatus UmiWebFastCgiReplyInit(uint16_t requestId, size_t bodyLimit, UmiWebFastCgiReply *reply);

/** Accept only reply records for this exact request. Honor each stream's
 * empty terminator and close both streams at END_REQUEST. A transport EOF
 * before END_REQUEST leaves complete false. Inspect applicationStatus and
 * protocolStatus separately before consuming a successful full response.
 *
 * On failure, both outputs remain unchanged. This function performs no
 * I/O and does not forward stderr or parse the worker's HTTP headers.
 */
UmiStatus UmiWebFastCgiReadReply(UmiWebFastCgiReply *reply, const void *wire,
    size_t available, UmiWebFastCgiRecord *record);

#ifdef __cplusplus
}
#endif
#endif
