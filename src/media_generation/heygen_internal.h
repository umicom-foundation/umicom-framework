/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/heygen_internal.h
 * PURPOSE: Share private request and response ownership across HeyGen adapters.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_GENERATION_HEYGEN_INTERNAL_H
#define UMICOM_MEDIA_GENERATION_HEYGEN_INTERNAL_H
#include "umicom/media_generation/heygen.h"
#include "umicom/security/secrets.h"
#include "umicom/language_runtime/json_document.h"
#include "umicom/language_runtime/json_text.h"
struct UmiHeyGenPlan
{
    UmiHeyGenOperation operation;
    bool creates_resource;
    bool consumed;
    char resource_id[UMI_HEYGEN_ID_CAPACITY];
    char request_key[129];
    char url[2049];
    char body[UMI_HEYGEN_BODY_CAPACITY];
};
bool UmiHeyGenIdentifier(const char *value, size_t capacity);
UmiStatus UmiHeyGenEncode(const UmiHeyGenDraft *draft, UmiHeyGenPlan *plan);
UmiStatus UmiHeyGenDecode(const UmiHeyGenPlan *plan, const char *body, size_t length, UmiHeyGenResult *out);
UmiStatus UmiHeyGenHttp(void *context, const char *url, const char *body, const char *request_key,
                        const char *key, const UmiCancellationToken *cancel, char *reply, size_t capacity,
                        size_t *length, unsigned *http);
#endif
