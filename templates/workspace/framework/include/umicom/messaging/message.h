/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/messaging/message.h
 *
 * PURPOSE:
 *   Declare the message contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MESSAGING_MESSAGE_H
#define UMICOM_MESSAGING_MESSAGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named message kind values accepted by this public contract.
 */
typedef enum UmiMessageKind {
    UMI_MESSAGE_COMMAND = 1,
    UMI_MESSAGE_EVENT = 2,
    UMI_MESSAGE_QUERY = 3,
    UMI_MESSAGE_REPLY = 4
} UmiMessageKind;

/**
 * Represent the message envelope data shared with callers of this public contract.
 */
typedef struct UmiMessageEnvelope {
    uint32_t structure_size;
    uint32_t schema_version;
    UmiMessageKind kind;
    uint64_t sequence;
    uint64_t correlation_id;
    const char *name;
    const char *payload;
} UmiMessageEnvelope;

#ifdef __cplusplus
}
#endif

#endif
