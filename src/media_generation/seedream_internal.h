/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/seedream_internal.h
 * PURPOSE: Keep provider limits and private request ownership together.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_GENERATION_SEEDREAM_INTERNAL_H
#define UMICOM_MEDIA_GENERATION_SEEDREAM_INTERNAL_H
#include "umicom/media_generation/seedream.h"
#include "umicom/security/secrets.h"
#define UMI_SEEDREAM_ENDPOINT "https://ark.ap-southeast.bytepluses.com/api/v3/images/generations"
#define UMI_SEEDREAM_REPLY_LIMIT (32U * 1024U * 1024U)
#define UMI_SEEDREAM_IMAGE_LIMIT (16U * 1024U * 1024U)
struct UmiSeedreamPlan
{
    char body[50000];
    bool consumed;
};
UmiStatus UmiSeedreamDecode(const char *bytes, size_t length, const UmiCancellationToken *cancel,
                            UmiSeedreamResult *out);
UmiStatus UmiSeedreamHttp(void *context, const char *url, const char *body, const char *key,
                          const UmiCancellationToken *cancel, char *reply, size_t capacity, size_t *length,
                          unsigned *http);
#endif
