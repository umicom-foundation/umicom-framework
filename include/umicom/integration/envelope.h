/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/envelope.h
 *
 * PURPOSE:
 *   Carry bounded command/event/query data across application boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_ENVELOPE_H
#define UMICOM_INTEGRATION_ENVELOPE_H

#include "umicom/base/status.h"
#include "umicom/integration/types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the integration envelope data shared with callers of this public contract.
 */
typedef struct UmiIntegrationEnvelope {
    UmiIntegrationMessageKind kind;
    char source_application[UMI_INTEGRATION_ID_CAPACITY];
    char target_application[UMI_INTEGRATION_ID_CAPACITY];
    char topic[UMI_INTEGRATION_ID_CAPACITY];
    char correlation_id[UMI_INTEGRATION_ID_CAPACITY];
    char payload[UMI_INTEGRATION_PAYLOAD_CAPACITY];
} UmiIntegrationEnvelope;

/**
 * Initialise integration envelope from caller-provided values so later operations receive
 * a known state.
 */
void umi_integration_envelope_init(UmiIntegrationEnvelope *envelope);
/**
 * Check that integration envelope satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_integration_envelope_validate(
    const UmiIntegrationEnvelope *envelope);

/* The original in-memory envelope remains ABI compatible. The following
 * transport-neutral codec turns its bounded textual fields into one canonical
 * byte frame for a future native local transport. It is NOT an authentication,
 * confidentiality, authorisation, replay-cache or socket implementation.
 * A trusted transport owner must independently authenticate the OS peer and
 * bind it to the expected application/process instance before dispatching. */
#define UMI_INTEGRATION_WIRE_HEADER_BYTES 32U
#define UMI_INTEGRATION_WIRE_MAX_BYTES (UMI_INTEGRATION_WIRE_HEADER_BYTES + \
    (4U * (UMI_INTEGRATION_ID_CAPACITY - 1U)) + \
    (UMI_INTEGRATION_PAYLOAD_CAPACITY - 1U))

/* Frames contain a fixed-size header, network-order sequence/field lengths,
 * and five length-delimited text fields. NUL terminators are never transmitted.
 * The format discriminator is internal protocol metadata, NOT a function/API
 * release number. The sender owns a strictly positive per-channel sequence.
 *
 * Pass output==NULL, capacity==0 to measure the required frame size. Success
 * writes outLength; any other failure leaves output and outLength unchanged.
 * The call allocates no heap memory. Output and input may overlap; outLength
 * must not alias either of them. */
UmiStatus UmiIntegrationEnvelopeWireEncode(
    const UmiIntegrationEnvelope *envelope, uint64_t sequence,
    uint8_t *output, size_t capacity, size_t *outLength);

/* Decode EXACTLY one complete frame. No trailing bytes, embedded NULs,
 * unsupported flags, bad lengths, invalid message kinds or empty required IDs
 * are accepted. The outputs remain unchanged on every error. The sequence is
 * supplied as evidence only; it does not authenticate the sender. */
UmiStatus UmiIntegrationEnvelopeWireDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiIntegrationEnvelope *outEnvelope, uint64_t *outSequence);

/* The owning authenticated channel tracks replay independently from messages.
 * Initialise after a NEW and independently verified transport connection.
 * Accept requires a strictly increasing, nonzero sequence. A rejected frame
 * does not advance the window. This check is not a substitute for OS peer
 * credentials, MACs, encryption, capability checks or process-generation proof.
 * Serialise access on the transport's owning thread. */
typedef struct UmiIntegrationWireSequenceWindow {
    uint32_t structureSize;
    uint64_t lastAccepted;
} UmiIntegrationWireSequenceWindow;

void UmiIntegrationWireSequenceWindowInit(
    UmiIntegrationWireSequenceWindow *window);
UmiStatus UmiIntegrationWireSequenceWindowAccept(
    UmiIntegrationWireSequenceWindow *window, uint64_t sequence);

/* TCP, Unix sockets and named pipes can split/coalesce frames. This bounded
 * decoder retains at most ONE incomplete frame without allocating memory.
 * Feed stops at the first complete frame and reports consumed bytes; the
 * caller feeds any remaining bytes again. A malformed frame poisons the
 * decoder, which must be explicitly Reset before receiving another channel
 * frame. Never carry a poisoned decoder into a different authenticated peer.
 * No user callback, dispatcher or network operation is invoked here. */
typedef struct UmiIntegrationWireDecoder {
    uint32_t structureSize;
    size_t buffered;
    size_t expected;
    bool poisoned;
    uint8_t bytes[UMI_INTEGRATION_WIRE_MAX_BYTES];
} UmiIntegrationWireDecoder;

void UmiIntegrationWireDecoderInit(UmiIntegrationWireDecoder *decoder);
void UmiIntegrationWireDecoderReset(UmiIntegrationWireDecoder *decoder);
UmiStatus UmiIntegrationWireDecoderFeed(
    UmiIntegrationWireDecoder *decoder,
    const uint8_t *chunk, size_t chunkLength,
    size_t *outConsumed, bool *outComplete,
    UmiIntegrationEnvelope *outEnvelope, uint64_t *outSequence);

#ifdef __cplusplus
}
#endif

#endif
