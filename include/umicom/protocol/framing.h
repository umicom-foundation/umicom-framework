/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/framing.h
 *
 * PURPOSE:
 *   Encode and decode Content-Length framed messages used by Language Server and Debug Adapter protocols.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_FRAMING_H
#define UMICOM_PROTOCOL_FRAMING_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/protocol/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the protocol frame data shared with callers of this public contract.
 */
typedef struct UmiProtocolFrame {
    size_t content_length;
    size_t header_length;
    size_t total_length;
    const char *content;
} UmiProtocolFrame;

/**
 * Write protocol frame in its stable representation and report capacity or input failures
 * to the caller.
 */
UmiStatus umi_protocol_frame_encode(const char *content,
                                    char *out_frame,
                                    size_t capacity,
                                    size_t *out_length);
/**
 * Read protocol frame into validated module state and return a status when input cannot be
 * used.
 */
UmiStatus umi_protocol_frame_parse(const char *buffer,
                                   size_t buffer_length,
                                   UmiProtocolFrame *out_frame);
/**
 * Provide the protocol frame copy content operation used by this module and its client
 * applications.
 */
UmiStatus umi_protocol_frame_copy_content(const UmiProtocolFrame *frame,
                                          char *out_content,
                                          size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
