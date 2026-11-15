/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/sse_parser.h
 *
 * PURPOSE:
 *   Receive bounded UTF-8 event-stream records with explicit consumption and owned message copies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_SSE_PARSER_H
#define UMICOM_WEB_SSE_PARSER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebSseParser UmiWebSseParser;
typedef struct UmiWebSseLimits {
    /* Payload bytes, excluding terminators. Each storage limit is 1..1 MiB;
     * event_bytes must be at least 7 to hold the default name "message".
     * Input includes BOM, comments, framing and incomplete UTF-8 bytes. */
    size_t line_bytes, data_bytes, event_bytes, id_bytes;
    uint64_t input_bytes;
} UmiWebSseLimits;
typedef enum UmiWebSseState {
    UMI_WEB_SSE_ACTIVE=0, UMI_WEB_SSE_READY=1,
    UMI_WEB_SSE_FINISHED=2, UMI_WEB_SSE_FAILED=3
} UmiWebSseState;
typedef struct UmiWebSseSnapshot {
    UmiWebSseState state;
    UmiStatus failure;
    uint64_t input_bytes, delivered_events;
    size_t pending_line_bytes, pending_data_bytes, pending_utf8_bytes;
    /* Metadata only: the parser never reconnects, sleeps or opens a socket. */
    uint64_t retry_milliseconds;
    int has_retry;
} UmiWebSseSnapshot;
typedef struct UmiWebSseMessage {
    /* Immutable NUL-terminated views owned by this single message allocation.
     * Free the whole object with MessageFree; never free these views alone. */
    const char *event, *data, *id;
    size_t event_bytes, data_bytes, id_bytes;
    uint64_t retry_milliseconds;
    int has_retry;
} UmiWebSseMessage;

/* One externally serialized owner per response. No callbacks or transport.
 * Create preserves *out on failure; Destroy accepts NULL. Do not concurrently
 * feed, take, inspect or destroy. No implicit parser reuse across requests. */
UmiStatus UmiWebSseParserCreate(const UmiWebSseLimits *limits,UmiWebSseParser **out);
void UmiWebSseParserDestroy(UmiWebSseParser *parser);
/* Accept input up to the first complete event. OK means READY, BUSY means more
 * input is needed. Feed while READY returns OK and consumes zero bytes.
 * Resume at bytes+consumed after Take; never discard a coalesced successor.
 * Valid calls always set consumed, including errors. Invalid arguments leave
 * it unchanged. A malformed/over-budget input makes failure sticky; bytes
 * accepted earlier in that call are not rolled back. The offending byte is
 * consumed for grammar/UTF-8 errors, but not when the total input budget ends.
 * Strict text profile: invalid UTF-8 and NUL are refused, including comments.
 * This is not a browser EventSource replacement (no replacement decoding).
 * Input and consumed must not overlap each other or parser-owned storage. */
UmiStatus UmiWebSseParserFeed(UmiWebSseParser *parser,const void *bytes,
    size_t length,size_t *consumed);
/* Copy the ready event into one allocation, then return parser to ACTIVE.
 * BUSY means no complete event. Allocation failure preserves READY and *out
 * for retry. The message remains valid after the parser is destroyed.
 * Empty data fields produce events; blocks without data fields do not.
 * ID and valid retry values persist across events; names reset each block.
 * Empty, nondecimal or UINT64_MAX-overflowing retry fields are ignored.
 * After FINISHED, Feed/Take return INVALID_STATE; a failed parser returns its
 * original error. No operation silently reopens a closed response. */
UmiStatus UmiWebSseParserTake(UmiWebSseParser *parser,UmiWebSseMessage **out);
void UmiWebSseMessageFree(UmiWebSseMessage *message);
/* Mark EOF. A READY event must be taken first (BUSY, state unchanged).
 * Incomplete UTF-8 fails. Otherwise discard pending line/data/name and finish,
 * even if a data line ended without the final blank line. Idempotent after
 * FINISHED. No implicit final event, reconnect or new request is produced. */
UmiStatus UmiWebSseParserFinish(UmiWebSseParser *parser);
/* Copy diagnostics, preserving output on invalid arguments. An inspection of
 * FAILED succeeds and reports its original error in snapshot.failure. */
UmiStatus UmiWebSseParserInspect(const UmiWebSseParser *parser,
    UmiWebSseSnapshot *out);
#ifdef __cplusplus
}
#endif
#endif
