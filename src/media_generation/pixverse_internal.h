/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/pixverse_internal.h
 * PURPOSE: Share private request ownership between the PixVerse adapters.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_GENERATION_PIXVERSE_INTERNAL_H
#define UMICOM_MEDIA_GENERATION_PIXVERSE_INTERNAL_H
#include "umicom/media_generation/pixverse.h"
#include "umicom/security/secrets.h"
#define UMI_PIXVERSE_ORIGIN "https://app-api.pixverse.ai/openapi/v2/"
struct UmiPixVersePlan
{
    UmiPixVerseOperation operation;
    bool creates_resource, consumed;
    int64_t video_id;
    char request_key[37], url[160], body[UMI_PIXVERSE_BODY_CAPACITY];
};
UmiStatus UmiPixVerseEncode(const UmiPixVerseDraft *draft, UmiPixVersePlan *plan);
UmiStatus UmiPixVerseDecode(const UmiPixVersePlan *plan, const char *body, size_t length,
                            UmiPixVerseResult *out);
UmiStatus UmiPixVerseHttp(void *context, const char *url, const char *body, const char *trace_id,
                          const char *key, const UmiCancellationToken *cancel, char *reply, size_t capacity,
                          size_t *length, unsigned *http);
#endif
