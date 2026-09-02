/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/payload.h
 *
 * PURPOSE:
 *   Define owned binary and UTF-8 payload values with explicit size and content type for message transport and persistence boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_PAYLOAD_H
#define UMICOM_MESSAGING_PAYLOAD_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the payload data shared with callers of this public contract.
 */
typedef struct UmiPayload {
    unsigned char *data;
    size_t size;
    char content_type[64];
} UmiPayload;

/**
 * Initialise payload from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_payload_create(const void *data,
                             size_t size,
                             const char *content_type,
                             UmiPayload *out_payload);
/**
 * Provide the payload create text operation used by this module and its client
 * applications.
 */
UmiStatus umi_payload_create_text(const char *text, UmiPayload *out_payload);
/**
 * Provide the payload clone operation used by this module and its client applications.
 */
UmiStatus umi_payload_clone(const UmiPayload *source, UmiPayload *destination);
/**
 * Release or reset state held by payload so the same storage can be reused safely.
 */
void umi_payload_dispose(UmiPayload *payload);
/**
 * Provide the payload text operation used by this module and its client applications.
 */
const char *umi_payload_text(const UmiPayload *payload);

#ifdef __cplusplus
}
#endif

#endif
