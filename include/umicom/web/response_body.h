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
/* Transfer an existing immutable allocation without copying payload bytes.
 * On success, *bytes becomes NULL and Destroy calls release(context, original)
 * exactly once, including an empty allocation. On failure, *bytes stays owned
 * by the caller, *out is NULL and release is not called. The callback and its
 * context must remain valid until Destroy; they must not destroy this body.
 * A NULL buffer is valid only when length is zero. release is always required.
 * All pointer slots must be disjoint. After success, do not mutate or free the
 * transferred storage through aliases. The original Create copy API remains. */
typedef void (*UmiWebResponseBodyRelease)(void *context, void *bytes);
UmiStatus UmiWebResponseBodyTake(unsigned char **bytes, size_t length,
    UmiWebResponseBodyRelease release, void *context, UmiWebResponseBody **out);
#ifdef __cplusplus
}
#endif
#endif
