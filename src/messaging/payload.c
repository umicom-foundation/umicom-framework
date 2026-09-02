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

/*
 * Initialise payload from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_payload_create(const void *data,
                             size_t size,
                             const char *content_type,
                             UmiPayload *out_payload)
{
    unsigned char *copy = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_payload == NULL || (size > 0U && data == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_payload, 0, sizeof(*out_payload));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (size > 0U) {
        copy = (unsigned char *)malloc(size + 1U);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
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

/*
 * Provide the payload create text operation used by this module and its client
 * applications.
 */
UmiStatus umi_payload_create_text(const char *text, UmiPayload *out_payload)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) text = "";
    return umi_payload_create(text,
                              strlen(text),
                              "text/plain; charset=utf-8",
                              out_payload);
}

/* Provide the payload clone operation used by this module and its client applications. */
UmiStatus umi_payload_clone(const UmiPayload *source, UmiPayload *destination)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_payload_create(source->data,
                              source->size,
                              source->content_type,
                              destination);
}

/* Release or reset state held by payload so the same storage can be reused safely. */
void umi_payload_dispose(UmiPayload *payload)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (payload == NULL) return;
    free(payload->data);
    (void)memset(payload, 0, sizeof(*payload));
}

/* Provide the payload text operation used by this module and its client applications. */
const char *umi_payload_text(const UmiPayload *payload)
{
    return payload != NULL && payload->data != NULL
        ? (const char *)payload->data
        : "";
}
