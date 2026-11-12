/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/response_body.h
 *
 * PURPOSE:
 *   Own a bounded immutable HTTP response body without enlarging legacy public structures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_RESPONSE_BODY_H
#define UMICOM_WEB_RESPONSE_BODY_H
#include "umicom/base/status.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WEB_RESPONSE_BODY_LIMIT (1024U * 1024U)
typedef struct UmiWebResponseBody UmiWebResponseBody;
/* Copy exactly length bytes, including embedded NUL. The source is borrowed
 * only during this call. NULL bytes are valid only for length zero. Output is
 * cleared on failure; the caller owns the successful result until Destroy.
 * This bound is independent of request and legacy inline-response limits. */
UmiStatus UmiWebResponseBodyCreate(const void *bytes, size_t length, UmiWebResponseBody **out);
void UmiWebResponseBodyDestroy(UmiWebResponseBody *body);
/* Borrowed immutable bytes remain valid until Destroy; NULL body gives NULL/0. */
const void *UmiWebResponseBodyBytes(const UmiWebResponseBody *body);
size_t UmiWebResponseBodyLength(const UmiWebResponseBody *body);
#ifdef __cplusplus
}
#endif
#endif
