/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/payload.c
 *
 * PURPOSE:
 *   Implement owned payload creation, cloning, UTF-8 convenience access and deterministic cleanup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/payload.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus umi_payload_create(const void *data,
                             size_t size,
                             const char *content_type,
                             UmiPayload *out_payload)
{
    unsigned char *copy = NULL;
    if (out_payload == NULL || (size > 0U && data == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_payload, 0, sizeof(*out_payload));
    if (size > 0U) {
        copy = (unsigned char *)malloc(size + 1U);
        if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        (void)memcpy(copy, data, size);
        copy[size] = 0U;
    }
    out_payload->data = copy;
    out_payload->size = size;
    (void)snprintf(out_payload->content_type,
                   sizeof(out_payload->content_type),
                   "%s",
                   content_type != NULL ? content_type :
                       "application/octet-stream");
    return UMI_STATUS_OK;
}

UmiStatus umi_payload_create_text(const char *text, UmiPayload *out_payload)
{
    if (text == NULL) text = "";
    return umi_payload_create(text,
                              strlen(text),
                              "text/plain; charset=utf-8",
                              out_payload);
}

UmiStatus umi_payload_clone(const UmiPayload *source, UmiPayload *destination)
{
    if (source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_payload_create(source->data,
                              source->size,
                              source->content_type,
                              destination);
}

void umi_payload_dispose(UmiPayload *payload)
{
    if (payload == NULL) return;
    free(payload->data);
    (void)memset(payload, 0, sizeof(*payload));
}

const char *umi_payload_text(const UmiPayload *payload)
{
    return payload != NULL && payload->data != NULL
        ? (const char *)payload->data
        : "";
}
