/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_dispatcher.c
 *
 * PURPOSE:
 *   Exercise one Suite and Inter-Application Runtime Foundation behaviour.
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

#include "umicom/integration/dispatcher.h"
#include <stdio.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

/*
 * Exercise hit and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus hit(const UmiIntegrationEnvelope *e, void *u) {
    int *count=(int*)u;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (e==NULL||count==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ++*count; return UMI_STATUS_OK;
}

/* A complete envelope is needed before native IPC or any untrusted plugin may
 * reach the existing dispatcher. All wire tests remain in this established
 * CTest executable; no second router, protocol framework or build target is
 * created. The original dispatcher smoke test below is retained. */
static int TestWireCodecAndSequence(void)
{
    UmiIntegrationEnvelope message = {0};
    UmiIntegrationEnvelope restored = {0};
    UmiIntegrationEnvelope unchanged = {0};
    UmiIntegrationWireDecoder decoder;
    UmiIntegrationWireSequenceWindow window;
    uint8_t frame[UMI_INTEGRATION_WIRE_MAX_BYTES] = {0};
    uint8_t another[UMI_INTEGRATION_WIRE_MAX_BYTES] = {0};
    uint8_t modified[UMI_INTEGRATION_WIRE_MAX_BYTES] = {0};
    uint8_t combined[2U * UMI_INTEGRATION_WIRE_MAX_BYTES] = {0};
    size_t length = 0U, otherLength = 0U, consumed = 0U;
    uint64_t sequence = 0U;
    bool completed = false;
    (void)snprintf(message.source_application, sizeof(message.source_application), "org.umicom.studio");
    (void)snprintf(message.target_application, sizeof(message.target_application), "org.umicom.bank");
    (void)snprintf(message.topic, sizeof(message.topic), "account.readonly");
    (void)snprintf(message.correlation_id, sizeof(message.correlation_id), "request-17");
    (void)snprintf(message.payload, sizeof(message.payload), "{\"account\":\"example\",\"amount\":\"12.34\"}");
    message.kind = UMI_INTEGRATION_MESSAGE_QUERY;
    CHECK(umi_integration_envelope_validate(&message) == UMI_STATUS_OK);
    CHECK(UmiIntegrationEnvelopeWireEncode(&message, 7U, NULL, 0U, &length) == UMI_STATUS_OK);
    CHECK(length >= UMI_INTEGRATION_WIRE_HEADER_BYTES && length <= sizeof(frame));
    CHECK(UmiIntegrationEnvelopeWireEncode(&message, 7U, frame, sizeof(frame), &length) == UMI_STATUS_OK);
    CHECK(memcmp(frame, "UMIE", 4U) == 0);
    CHECK(UmiIntegrationEnvelopeWireDecode(frame, length, &restored, &sequence) == UMI_STATUS_OK);
    CHECK(sequence == 7U && memcmp(&message, &restored, sizeof(message)) == 0);
    CHECK(UmiIntegrationEnvelopeWireEncode(&message, 8U, another, sizeof(another), &otherLength) == UMI_STATUS_OK);
    CHECK(length == otherLength && memcmp(frame, another, length) != 0);

    /* A small output buffer cannot receive half a frame. Measurements and
     * staging do not mutate their callers on failure. */
    (void)memset(modified, 0xA5, sizeof(modified));
    size_t sentinelLength = SIZE_MAX;
    CHECK(UmiIntegrationEnvelopeWireEncode(&message, 7U, modified, length - 1U,
                                           &sentinelLength) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(sentinelLength == SIZE_MAX);
    CHECK(modified[0] == 0xA5 && modified[length - 2U] == 0xA5);
    CHECK(UmiIntegrationEnvelopeWireEncode(&message, 0U, modified, sizeof(modified),
                                           &sentinelLength) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiIntegrationEnvelopeWireEncode(&message, 7U, NULL, 1U,
                                           &sentinelLength) == UMI_STATUS_INVALID_ARGUMENT);

    /* Invalid frames must not publish either caller-owned result. */
    unchanged.kind = UMI_INTEGRATION_MESSAGE_EVENT;
    (void)snprintf(unchanged.source_application, sizeof(unchanged.source_application), "unchanged");
    restored = unchanged;
    sequence = 99U;
    (void)memcpy(modified, frame, length);
    modified[0] ^= 1U;
    CHECK(UmiIntegrationEnvelopeWireDecode(modified, length, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&restored, &unchanged, sizeof(unchanged)) == 0 && sequence == 99U);
    (void)memcpy(modified, frame, length);
    modified[4] ^= 2U; /* Unsupported wire format. */
    CHECK(UmiIntegrationEnvelopeWireDecode(modified, length, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    (void)memcpy(modified, frame, length);
    modified[5] = 255U; /* An invalid message kind cannot be dispatched. */
    CHECK(UmiIntegrationEnvelopeWireDecode(modified, length, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    (void)memcpy(modified, frame, length);
    modified[6] = 1U; /* Reserved/unsupported flags. */
    CHECK(UmiIntegrationEnvelopeWireDecode(modified, length, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    (void)memcpy(modified, frame, length);
    modified[28] = 1U; /* Reserved nonzero header bytes. */
    CHECK(UmiIntegrationEnvelopeWireDecode(modified, length, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    (void)memcpy(modified, frame, length);
    modified[8] = 0U; /* Corrupt sequence after removing all of it below. */
    (void)memset(modified + 8U, 0, 8U);
    CHECK(UmiIntegrationEnvelopeWireDecode(modified, length, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    (void)memcpy(modified, frame, length);
    modified[16] = 255U; modified[17] = 255U; /* Oversized first field. */
    CHECK(UmiIntegrationEnvelopeWireDecode(modified, length, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    (void)memcpy(modified, frame, length);
    modified[16] = 0U; modified[17] = 0U; /* Empty required source ID. */
    CHECK(UmiIntegrationEnvelopeWireDecode(modified, length, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    (void)memcpy(modified, frame, length);
    modified[UMI_INTEGRATION_WIRE_HEADER_BYTES] = 0U; /* Embedded NUL. */
    CHECK(UmiIntegrationEnvelopeWireDecode(modified, length, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiIntegrationEnvelopeWireDecode(frame, length - 1U, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    CHECK(UmiIntegrationEnvelopeWireDecode(frame, length + 1U, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&restored, &unchanged, sizeof(unchanged)) == 0 && sequence == 99U);

    /* Existing untrusted in-memory callers must also be bounded before the
     * current dispatcher reaches strcmp or a plugin handler. */
    UmiIntegrationEnvelope malformed = message;
    (void)memset(malformed.topic, 'x', sizeof(malformed.topic));
    CHECK(umi_integration_envelope_validate(&malformed) == UMI_STATUS_PARSE_ERROR);
    malformed = message;
    (void)memset(malformed.payload, 'x', sizeof(malformed.payload));
    CHECK(umi_integration_envelope_validate(&malformed) == UMI_STATUS_PARSE_ERROR);
    malformed = message;
    malformed.kind = (UmiIntegrationMessageKind)99;
    CHECK(umi_integration_envelope_validate(&malformed) == UMI_STATUS_PARSE_ERROR);
    malformed = message;
    malformed.topic[0] = '\n';
    CHECK(umi_integration_envelope_validate(&malformed) == UMI_STATUS_PARSE_ERROR);

    /* Byte streams can fragment or coalesce messages. No callbacks or dispatch
     * run in this decoder; the authenticated owner must choose when to route. */
    UmiIntegrationWireDecoderInit(&decoder);
    CHECK(UmiIntegrationWireDecoderFeed(&decoder, frame, 11U,
          &consumed, &completed, &restored, &sequence) == UMI_STATUS_OK);
    CHECK(consumed == 11U && !completed && decoder.buffered == 11U);
    CHECK(UmiIntegrationWireDecoderFeed(&decoder, frame + 11U, 21U,
          &consumed, &completed, &restored, &sequence) == UMI_STATUS_OK);
    CHECK(consumed == 21U && !completed && decoder.buffered == 32U);
    CHECK(UmiIntegrationWireDecoderFeed(&decoder, frame + 32U, length - 32U,
          &consumed, &completed, &restored, &sequence) == UMI_STATUS_OK);
    CHECK(consumed == length - 32U && completed && sequence == 7U);
    CHECK(memcmp(&message, &restored, sizeof(message)) == 0 && decoder.buffered == 0U);
    (void)memcpy(combined, frame, length);
    (void)memcpy(combined + length, another, otherLength);
    CHECK(UmiIntegrationWireDecoderFeed(&decoder, combined, length + otherLength,
          &consumed, &completed, &restored, &sequence) == UMI_STATUS_OK);
    CHECK(consumed == length && completed && sequence == 7U);
    CHECK(UmiIntegrationWireDecoderFeed(&decoder, combined + consumed, otherLength,
          &consumed, &completed, &restored, &sequence) == UMI_STATUS_OK);
    CHECK(consumed == otherLength && completed && sequence == 8U);

    /* A malformed frame poisons the stream, and the owner must explicitly
     * choose whether to start a new authenticated channel after Reset. */
    (void)memcpy(modified, frame, length);
    modified[4] = 0U;
    CHECK(UmiIntegrationWireDecoderFeed(&decoder, modified, length,
          &consumed, &completed, &restored, &sequence) == UMI_STATUS_PARSE_ERROR);
    CHECK(decoder.poisoned && !completed && consumed == UMI_INTEGRATION_WIRE_HEADER_BYTES);
    CHECK(UmiIntegrationWireDecoderFeed(&decoder, frame, length,
          &consumed, &completed, &restored, &sequence) == UMI_STATUS_INVALID_STATE);
    UmiIntegrationWireDecoderReset(&decoder);
    CHECK(!decoder.poisoned && decoder.buffered == 0U);
    CHECK(UmiIntegrationWireDecoderFeed(&decoder, frame, length,
          &consumed, &completed, &restored, &sequence) == UMI_STATUS_OK);
    CHECK(completed && sequence == 7U);

    /* Replay decisions belong to the OS-authenticated channel, not the
     * untrusted on-wire sequence. The window never authenticates by itself. */
    UmiIntegrationWireSequenceWindowInit(&window);
    CHECK(UmiIntegrationWireSequenceWindowAccept(&window, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiIntegrationWireSequenceWindowAccept(&window, 7U) == UMI_STATUS_OK);
    CHECK(UmiIntegrationWireSequenceWindowAccept(&window, 7U) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiIntegrationWireSequenceWindowAccept(&window, 6U) == UMI_STATUS_INVALID_STATE);
    CHECK(window.lastAccepted == 7U);
    CHECK(UmiIntegrationWireSequenceWindowAccept(&window, 8U) == UMI_STATUS_OK);
    CHECK(UmiIntegrationWireSequenceWindowAccept(&window, UINT64_MAX) == UMI_STATUS_OK);
    CHECK(UmiIntegrationWireSequenceWindowAccept(&window, UINT64_MAX) == UMI_STATUS_INVALID_STATE);
    UmiIntegrationWireSequenceWindowInit(&window);
    CHECK(UmiIntegrationWireSequenceWindowAccept(&window, 1U) == UMI_STATUS_OK);

    /* Maximum bounded message fits without heap allocation. */
    (void)memset(message.payload, 'A', sizeof(message.payload) - 1U);
    message.payload[sizeof(message.payload) - 1U] = '\0';
    CHECK(UmiIntegrationEnvelopeWireEncode(&message, 1U, frame, sizeof(frame), &length) == UMI_STATUS_OK);
    CHECK(length <= UMI_INTEGRATION_WIRE_MAX_BYTES);
    CHECK(UmiIntegrationEnvelopeWireDecode(frame, length, &restored, &sequence) == UMI_STATUS_OK);
    CHECK(sequence == 1U && strcmp(restored.payload, message.payload) == 0);

    /* Deterministic mutation corpus: accepted frames must round-trip back to
     * the same canonical bytes. Every rejected mutation must preserve the
     * caller's previous envelope and sequence. This is not a replacement for
     * fuzzing the real local-IPC adapters with untrusted operating-system IO. */
    uint32_t randomState = UINT32_C(0x5a173b29);
    for (size_t attempt = 0U; attempt < 1024U; ++attempt) {
        randomState = randomState * UINT32_C(1664525) + UINT32_C(1013904223);
        const size_t position = (size_t)(randomState % (uint32_t)length);
        (void)memcpy(modified, frame, length);
        modified[position] ^= (uint8_t)(1U << (randomState % 8U));
        restored = unchanged;
        sequence = 99U;
        const UmiStatus decodeStatus = UmiIntegrationEnvelopeWireDecode(
            modified, length, &restored, &sequence);
        if (decodeStatus == UMI_STATUS_OK) {
            size_t canonicalLength = 0U;
            CHECK(UmiIntegrationEnvelopeWireEncode(&restored, sequence,
                  another, sizeof(another), &canonicalLength) == UMI_STATUS_OK);
            CHECK(canonicalLength == length && memcmp(another, modified, length) == 0);
        } else {
            CHECK(decodeStatus == UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(&restored, &unchanged, sizeof(unchanged)) == 0);
            CHECK(sequence == 99U);
        }
    }
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiIntegrationDispatcher d; UmiIntegrationEnvelope e; int count=0;
    umi_integration_dispatcher_init(&d); umi_integration_envelope_init(&e);
    CHECK(umi_integration_dispatcher_register(&d,"bank","ping",hit,&count)==UMI_STATUS_OK);
    (void)snprintf(e.source_application,sizeof(e.source_application),"studio");
    (void)snprintf(e.target_application,sizeof(e.target_application),"bank");
    (void)snprintf(e.topic,sizeof(e.topic),"ping");
    CHECK(umi_integration_dispatcher_dispatch(&d,&e)==UMI_STATUS_OK);
    CHECK(count==1);
    CHECK(TestWireCodecAndSequence() == 0);
    /* The same legacy dispatcher now refuses an unterminated topic before
     * its matching strcmp, without dispatching a single handler. */
    (void)memset(e.topic, 'x', sizeof(e.topic));
    CHECK(umi_integration_dispatcher_dispatch(&d, &e) == UMI_STATUS_PARSE_ERROR);
    CHECK(count == 1);
    return 0;
}
