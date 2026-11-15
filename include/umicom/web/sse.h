/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/sse.h
 *
 * PURPOSE:
 *   Format Server-Sent Events for browser streaming.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SSE_H
#define UMICOM_WEB_SSE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the web sse event data shared with callers of this public contract.
 */
typedef struct UmiWebSseEvent { uint64_t id; char event[64]; char data[2048]; } UmiWebSseEvent;
/**
 * Provide the web sse format operation used by this module and its client applications.
 */
/* Format a complete NUL-terminated record. Both fixed fields must contain a
 * NUL; event names must not contain CR/LF. Malformed UTF-8 returns PARSE_ERROR.
 * Data CR, LF and CRLF boundaries become separate data fields and are decoded
 * as LF by an SSE receiver. Empty and trailing lines are retained. Failure
 * preserves output bytes and optional out_length. Capacity includes the NUL;
 * out_length excludes it. Output may overlap event, but out_length must not
 * overlap either object. This formats data only; it does not send HTTP. */
UmiStatus umi_web_sse_format(const UmiWebSseEvent *event,char *out_text,size_t capacity,size_t *out_length);
#ifdef __cplusplus
}
#endif
#endif
