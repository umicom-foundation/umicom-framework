/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/response_body.c
 *
 * PURPOSE:
 *   Keep large local response bytes under explicit heap ownership and a fixed admission limit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/response_body.h"
#include <stdlib.h>
#include <string.h>
struct UmiWebResponseBody {
    size_t length;
    unsigned char bytes[];
};
UmiStatus UmiWebResponseBodyCreate(const void *bytes, size_t length, UmiWebResponseBody **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (length != 0U && bytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_WEB_RESPONSE_BODY_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* The fixed one-MiB maximum keeps this addition within size_t on supported
     * hosts and avoids a large automatic array on a worker's small stack. */
    UmiWebResponseBody *body = malloc(sizeof(*body) + length);
    if (body == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    body->length = length;
    if (length != 0U) memcpy(body->bytes, bytes, length);
    *out = body;
    return UMI_STATUS_OK;
}
void UmiWebResponseBodyDestroy(UmiWebResponseBody *body) { free(body); }
const void *UmiWebResponseBodyBytes(const UmiWebResponseBody *body)
{
    return body != NULL ? body->bytes : NULL;
}
size_t UmiWebResponseBodyLength(const UmiWebResponseBody *body)
{
    return body != NULL ? body->length : 0U;
}
