/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/reply_binding.h
 *
 * PURPOSE:
 *   Bind one pending reply to its verified connection and current suite process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_REPLY_BINDING_H
#define UMICOM_INTEGRATION_REPLY_BINDING_H

#include "umicom/integration/envelope.h"
#include "umicom/integration/readiness_gate.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Construct this value from trusted transport state, after OS peer
 * verification. The connection identity is locally issued, nonzero, and
 * never reused by an owner while stale replies could still arrive. */
typedef struct UmiIntegrationChannelPeer {
    uint64_t connectionIdentity;
    uint64_t processToken;
    uint64_t processGeneration;
} UmiIntegrationChannelPeer;

typedef enum UmiIntegrationReplyState {
    UMI_INTEGRATION_REPLY_EMPTY = 0,
    UMI_INTEGRATION_REPLY_PENDING,
    UMI_INTEGRATION_REPLY_COMPLETED,
    UMI_INTEGRATION_REPLY_CANCELLED
} UmiIntegrationReplyState;

/* The serial channel owner allocates one binding per request. Initialize it
 * before use and treat these fields as read-only. The request payload and
 * gate are never stored. Finish or cancel before reusing this storage. */
typedef struct UmiIntegrationReplyBinding {
    uint32_t structureSize;
    UmiIntegrationReplyState state;
    UmiIntegrationChannelPeer peer;
    char requester[UMI_INTEGRATION_ID_CAPACITY];
    char responder[UMI_INTEGRATION_ID_CAPACITY];
    char topic[UMI_INTEGRATION_ID_CAPACITY];
    char correlationId[UMI_INTEGRATION_ID_CAPACITY];
} UmiIntegrationReplyBinding;

void UmiIntegrationReplyBindingInit(UmiIntegrationReplyBinding *binding);

/* Only queries and commands with a nonempty, unique correlation ID can be
 * bound. The target must have a confirmed readiness record for this peer.
 * Bind before sending on the verified connection. Errors leave it unchanged. */
UmiStatus UmiIntegrationReplyBindingBegin(UmiIntegrationReplyBinding *binding,
    const UmiIntegrationReadinessGate *gate, const UmiIntegrationChannelPeer *peer,
    const UmiIntegrationEnvelope *request);

/* Apply this after decoding and accepting the wire sequence, before handing
 * the reply to a client. The owner supplies the same live gate and trusted
 * peer metadata obtained for this reply. The response must reverse the
 * applications and preserve the topic and correlation ID. A different peer
 * or stale confirmed instance returns PERMISSION_DENIED, an unmatched reply
 * returns NOT_FOUND, a cancelled binding returns CANCELLED, and a repeated
 * completion returns INVALID_STATE. Errors leave the binding unchanged. */
UmiStatus UmiIntegrationReplyBindingAccept(UmiIntegrationReplyBinding *binding,
    const UmiIntegrationReadinessGate *gate, const UmiIntegrationChannelPeer *peer,
    const UmiIntegrationEnvelope *reply);

/* Idempotent while cancelled. It prevents publishing a later reply and does
 * not assert that the remote process stopped or rolled back the request. */
UmiStatus UmiIntegrationReplyBindingCancel(UmiIntegrationReplyBinding *binding);

#ifdef __cplusplus
}
#endif
#endif
