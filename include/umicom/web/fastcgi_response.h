/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/fastcgi_response.h
 *
 * PURPOSE:
 *   Translate verified worker output to a bounded Framework HTTP response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_FASTCGI_RESPONSE_H
#define UMICOM_WEB_FASTCGI_RESPONSE_H

#include "umicom/web/fastcgi_exchange.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WEB_FASTCGI_HEADER_LIMIT (32U * 1024U)

/* Admit stdout from a completed successful exchange. Use a consistent LF or
 * CRLF header block, an optional Status field, and Content-Type for ordinary
 * documents. Location requires an explicit supported status. Worker-provided
 * Content-Length, when present, must match the bytes and is consumed here.
 * Framework supplies connection framing when formatting the response.
 *
 * The inputs and outputs must be disjoint. *outBody must start NULL. Success
 * publishes metadata with an empty inline body and copies any content into an
 * independently owned response body. Call UmiWebResponseBodyDestroy after use.
 * On failure the caller's response and body slot retain their previous values.
 */
UmiStatus UmiWebFastCgiResponseDecode(const UmiWebFastCgiResult *result,
    UmiWebResponse *response, UmiWebResponseBody **outBody);

#ifdef __cplusplus
}
#endif
#endif
