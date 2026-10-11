/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/envelope.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
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

#include "umicom/integration/envelope.h"

#include <limits.h>
#include <string.h>

/*
 * Initialise integration envelope from caller-provided values so later operations receive
 * a known state.
 */
void umi_integration_envelope_init(UmiIntegrationEnvelope *envelope)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (envelope != NULL) {
        (void)memset(envelope, 0, sizeof(*envelope));
    }
}

/* SOURCE PRESERVATION: The original contract validator is retained below.
 * It checked whether three IDs were empty, but assumed every fixed-sized
 * public C field contained a terminating NUL. A malformed envelope could
 * reach strcmp-based routing without bounded validation. The replacement
 * keeps the existing entry point and status contract, and checks all fields
 * before a caller may route or serialise the envelope. Do not delete this
 * historical implementation without the project owner's approval. */
#if 0
/*
 * Check that integration envelope satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_integration_envelope_validate(
    const UmiIntegrationEnvelope *envelope)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (envelope == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (envelope->source_application[0] == '\0' ||
        envelope->target_application[0] == '\0' ||
        envelope->topic[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
#endif

/* The fixed public envelope may arrive from plugin, IPC or stored data.
 * Search only its actual field bounds: strlen/strcmp on unterminated input
 * would read unrelated memory before a security policy could reject it. */
static bool TextFieldValid(const char *text, size_t capacity,
                           bool required, bool identity)
{
    const char *end;
    size_t length;
    if (text == NULL || capacity == 0U)
        return false;
    end = (const char *)memchr(text, '\0', capacity);
    if (end == NULL)
        return false;
    length = (size_t)(end - text);
    if (required && length == 0U)
        return false;
    if (identity) {
        for (size_t index = 0U; index < length; ++index) {
            const unsigned char ch = (unsigned char)text[index];
            if (ch < 32U || ch == 127U)
                return false;
        }
    }
    return true;
}

/* Preserve the established validator name and its parse-error semantics. */
UmiStatus umi_integration_envelope_validate(
    const UmiIntegrationEnvelope *envelope)
{
    if (envelope == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (envelope->kind != UMI_INTEGRATION_MESSAGE_COMMAND &&
        envelope->kind != UMI_INTEGRATION_MESSAGE_EVENT &&
        envelope->kind != UMI_INTEGRATION_MESSAGE_QUERY &&
        envelope->kind != UMI_INTEGRATION_MESSAGE_RESPONSE)
        return UMI_STATUS_PARSE_ERROR;
    if (!TextFieldValid(envelope->source_application,
                        sizeof(envelope->source_application), true, true) ||
        !TextFieldValid(envelope->target_application,
                        sizeof(envelope->target_application), true, true) ||
        !TextFieldValid(envelope->topic,
                        sizeof(envelope->topic), true, true) ||
        !TextFieldValid(envelope->correlation_id,
                        sizeof(envelope->correlation_id), false, true) ||
        !TextFieldValid(envelope->payload,
                        sizeof(envelope->payload), false, false))
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}

/* A canonical envelope frame has no native C padding or implicit NUL bytes.
 * Byte offsets:
 *  0..3   magic "UMIE"
 *  4      stable format discriminator (implementation currently writes 1)
 *  5      message kind
 *  6..7   reserved flags, zero
 *  8..15  unsigned sequence in network byte order
 * 16..25  five unsigned 16-bit network-order field lengths
 * 26..31  reserved, zero
 * 32..    source, target, topic, correlation and payload byte strings.
 * This format is intentionally NOT a general-purpose HTTP/LSP frame. The
 * existing protocol/framing module continues to own Content-Length framing. */
static const uint8_t WireMagic[4] = {'U', 'M', 'I', 'E'};
#define UMI_INTEGRATION_WIRE_FORMAT 1U
#define UMI_INTEGRATION_WIRE_FIELD_COUNT 5U

static void WriteUnsigned16(uint8_t *bytes, uint16_t value)
{
    bytes[0] = (uint8_t)(value >> 8U);
    bytes[1] = (uint8_t)value;
}

static uint16_t ReadUnsigned16(const uint8_t *bytes)
{
    return (uint16_t)(((uint16_t)bytes[0] << 8U) | (uint16_t)bytes[1]);
}

static void WriteUnsigned64(uint8_t *bytes, uint64_t value)
{
    for (unsigned index = 0U; index < 8U; ++index)
        bytes[index] = (uint8_t)(value >> ((7U - index) * 8U));
}

static uint64_t ReadUnsigned64(const uint8_t *bytes)
{
    uint64_t value = 0U;
    for (unsigned index = 0U; index < 8U; ++index)
        value = (value << 8U) | (uint64_t)bytes[index];
    return value;
}

/* Strict header parsing is shared by one-shot decode and streaming feed.
 * The declared lengths are untrusted until all five bounds and the canonical
 * reserved/format bytes have been checked. This function needs only 32 bytes. */
static UmiStatus ParseWireHeader(const uint8_t *bytes,
                                 size_t lengths[UMI_INTEGRATION_WIRE_FIELD_COUNT],
                                 uint64_t *sequence, size_t *outFrameSize)
{
    static const size_t capacities[UMI_INTEGRATION_WIRE_FIELD_COUNT] = {
        UMI_INTEGRATION_ID_CAPACITY, UMI_INTEGRATION_ID_CAPACITY,
        UMI_INTEGRATION_ID_CAPACITY, UMI_INTEGRATION_ID_CAPACITY,
        UMI_INTEGRATION_PAYLOAD_CAPACITY
    };
    size_t total = UMI_INTEGRATION_WIRE_HEADER_BYTES;
    if (memcmp(bytes, WireMagic, sizeof(WireMagic)) != 0 ||
        bytes[4] != UMI_INTEGRATION_WIRE_FORMAT ||
        bytes[5] > (uint8_t)UMI_INTEGRATION_MESSAGE_RESPONSE ||
        bytes[6] != 0U || bytes[7] != 0U)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t index = 26U; index < UMI_INTEGRATION_WIRE_HEADER_BYTES; ++index)
        if (bytes[index] != 0U)
            return UMI_STATUS_PARSE_ERROR;
    *sequence = ReadUnsigned64(bytes + 8U);
    if (*sequence == 0U)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t index = 0U; index < UMI_INTEGRATION_WIRE_FIELD_COUNT; ++index) {
        const size_t amount = ReadUnsigned16(bytes + 16U + index * 2U);
        if (amount >= capacities[index] || (index < 3U && amount == 0U))
            return UMI_STATUS_PARSE_ERROR;
        lengths[index] = amount;
        total += amount; /* Bounded by the validated public field capacities. */
    }
    if (total > UMI_INTEGRATION_WIRE_MAX_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    *outFrameSize = total;
    return UMI_STATUS_OK;
}

UmiStatus UmiIntegrationEnvelopeWireEncode(
    const UmiIntegrationEnvelope *envelope, uint64_t sequence,
    uint8_t *output, size_t capacity, size_t *outLength)
{
    const char *fields[UMI_INTEGRATION_WIRE_FIELD_COUNT];
    size_t lengths[UMI_INTEGRATION_WIRE_FIELD_COUNT];
    uint8_t frame[UMI_INTEGRATION_WIRE_MAX_BYTES] = {0};
    size_t frameSize = UMI_INTEGRATION_WIRE_HEADER_BYTES;
    size_t offset;
    if (envelope == NULL || outLength == NULL || sequence == 0U ||
        (output == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_integration_envelope_validate(envelope);
    if (status != UMI_STATUS_OK)
        return status;
    fields[0] = envelope->source_application;
    fields[1] = envelope->target_application;
    fields[2] = envelope->topic;
    fields[3] = envelope->correlation_id;
    fields[4] = envelope->payload;
    for (size_t index = 0U; index < UMI_INTEGRATION_WIRE_FIELD_COUNT; ++index) {
        lengths[index] = strlen(fields[index]); /* Safe after bounded validation. */
        if (lengths[index] > UINT16_MAX)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        frameSize += lengths[index];
    }
    if (frameSize > sizeof(frame))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (output == NULL) {
        *outLength = frameSize; /* A read-only sizing operation. */
        return UMI_STATUS_OK;
    }
    if (capacity < frameSize)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(frame, WireMagic, sizeof(WireMagic));
    frame[4] = UMI_INTEGRATION_WIRE_FORMAT;
    frame[5] = (uint8_t)envelope->kind;
    WriteUnsigned64(frame + 8U, sequence);
    for (size_t index = 0U; index < UMI_INTEGRATION_WIRE_FIELD_COUNT; ++index)
        WriteUnsigned16(frame + 16U + index * 2U, (uint16_t)lengths[index]);
    offset = UMI_INTEGRATION_WIRE_HEADER_BYTES;
    for (size_t index = 0U; index < UMI_INTEGRATION_WIRE_FIELD_COUNT; ++index) {
        (void)memcpy(frame + offset, fields[index], lengths[index]);
        offset += lengths[index];
    }
    /* Only one complete, valid frame is ever published. memmove supports a
     * caller reusing a suitable buffer overlapping its input envelope. */
    (void)memmove(output, frame, frameSize);
    *outLength = frameSize;
    return UMI_STATUS_OK;
}

UmiStatus UmiIntegrationEnvelopeWireDecode(
    const uint8_t *bytes, size_t byteCount,
    UmiIntegrationEnvelope *outEnvelope, uint64_t *outSequence)
{
    size_t lengths[UMI_INTEGRATION_WIRE_FIELD_COUNT] = {0};
    size_t frameSize = 0U;
    size_t offset = UMI_INTEGRATION_WIRE_HEADER_BYTES;
    uint64_t sequence = 0U;
    UmiIntegrationEnvelope candidate = {0};
    char *fields[UMI_INTEGRATION_WIRE_FIELD_COUNT];
    if (bytes == NULL || outEnvelope == NULL || outSequence == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (byteCount < UMI_INTEGRATION_WIRE_HEADER_BYTES ||
        byteCount > UMI_INTEGRATION_WIRE_MAX_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = ParseWireHeader(bytes, lengths, &sequence, &frameSize);
    if (status != UMI_STATUS_OK || frameSize != byteCount)
        return UMI_STATUS_PARSE_ERROR;
    candidate.kind = (UmiIntegrationMessageKind)bytes[5];
    fields[0] = candidate.source_application;
    fields[1] = candidate.target_application;
    fields[2] = candidate.topic;
    fields[3] = candidate.correlation_id;
    fields[4] = candidate.payload;
    for (size_t index = 0U; index < UMI_INTEGRATION_WIRE_FIELD_COUNT; ++index) {
        if (memchr(bytes + offset, '\0', lengths[index]) != NULL)
            return UMI_STATUS_PARSE_ERROR;
        if (lengths[index] != 0U)
            (void)memcpy(fields[index], bytes + offset, lengths[index]);
        /* Candidate is zeroed: its next byte remains a terminator, because
         * the header already proved amount < destination field capacity. */
        offset += lengths[index];
    }
    if (umi_integration_envelope_validate(&candidate) != UMI_STATUS_OK)
        return UMI_STATUS_PARSE_ERROR;
    *outEnvelope = candidate;
    *outSequence = sequence;
    return UMI_STATUS_OK;
}

void UmiIntegrationWireSequenceWindowInit(
    UmiIntegrationWireSequenceWindow *window)
{
    if (window != NULL) {
        (void)memset(window, 0, sizeof(*window));
        window->structureSize = (uint32_t)sizeof(*window);
    }
}

UmiStatus UmiIntegrationWireSequenceWindowAccept(
    UmiIntegrationWireSequenceWindow *window, uint64_t sequence)
{
    if (window == NULL || sequence == 0U ||
        window->structureSize != sizeof(*window))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (sequence <= window->lastAccepted)
        return UMI_STATUS_INVALID_STATE;
    window->lastAccepted = sequence;
    return UMI_STATUS_OK;
}

/* Wipe retained frames when a peer closes, the decoder is reset, or a
 * successfully decoded message is copied out. This is hygiene, not a claim
 * of compiler-guaranteed secret erasure or a cryptographic primitive. */
void UmiIntegrationWireDecoderInit(UmiIntegrationWireDecoder *decoder)
{
    if (decoder != NULL) {
        (void)memset(decoder, 0, sizeof(*decoder));
        decoder->structureSize = (uint32_t)sizeof(*decoder);
    }
}

void UmiIntegrationWireDecoderReset(UmiIntegrationWireDecoder *decoder)
{
    UmiIntegrationWireDecoderInit(decoder);
}

UmiStatus UmiIntegrationWireDecoderFeed(
    UmiIntegrationWireDecoder *decoder,
    const uint8_t *chunk, size_t chunkLength,
    size_t *outConsumed, bool *outComplete,
    UmiIntegrationEnvelope *outEnvelope, uint64_t *outSequence)
{
    size_t consumed = 0U;
    if (decoder == NULL || outConsumed == NULL || outComplete == NULL ||
        outEnvelope == NULL || outSequence == NULL ||
        (chunk == NULL && chunkLength != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    *outConsumed = 0U;
    *outComplete = false;
    if (decoder->structureSize != sizeof(*decoder) ||
        decoder->poisoned || decoder->buffered > sizeof(decoder->bytes) ||
        (decoder->expected == 0U &&
         decoder->buffered > UMI_INTEGRATION_WIRE_HEADER_BYTES) ||
        (decoder->expected != 0U &&
         (decoder->expected <= UMI_INTEGRATION_WIRE_HEADER_BYTES ||
          decoder->expected > sizeof(decoder->bytes) ||
          decoder->buffered > decoder->expected)))
        return UMI_STATUS_INVALID_STATE;

    while (consumed < chunkLength) {
        const size_t goal = decoder->expected != 0U
            ? decoder->expected : UMI_INTEGRATION_WIRE_HEADER_BYTES;
        const size_t wanted = goal - decoder->buffered;
        size_t amount = chunkLength - consumed;
        if (amount > wanted)
            amount = wanted;
        if (amount != 0U) {
            (void)memcpy(decoder->bytes + decoder->buffered,
                         chunk + consumed, amount);
            decoder->buffered += amount;
            consumed += amount;
        }
        if (decoder->buffered == UMI_INTEGRATION_WIRE_HEADER_BYTES &&
            decoder->expected == 0U) {
            size_t fieldLengths[UMI_INTEGRATION_WIRE_FIELD_COUNT] = {0};
            uint64_t sequence = 0U;
            size_t frameSize = 0U;
            if (ParseWireHeader(decoder->bytes, fieldLengths,
                                &sequence, &frameSize) != UMI_STATUS_OK) {
                decoder->poisoned = true;
                *outConsumed = consumed;
                return UMI_STATUS_PARSE_ERROR;
            }
            decoder->expected = frameSize;
        }
        if (decoder->expected != 0U && decoder->buffered == decoder->expected) {
            UmiIntegrationEnvelope candidate;
            uint64_t sequence = 0U;
            const UmiStatus status = UmiIntegrationEnvelopeWireDecode(
                decoder->bytes, decoder->buffered, &candidate, &sequence);
            if (status != UMI_STATUS_OK) {
                decoder->poisoned = true;
                *outConsumed = consumed;
                return status;
            }
            UmiIntegrationWireDecoderReset(decoder);
            *outEnvelope = candidate;
            *outSequence = sequence;
            *outConsumed = consumed;
            *outComplete = true;
            return UMI_STATUS_OK;
        }
    }
    *outConsumed = consumed;
    return UMI_STATUS_OK;
}
