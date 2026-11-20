/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration/test_native_channel.c
 *
 * PURPOSE:
 *   Qualify readiness and reply binding across the owned native child pipes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/reply_binding.h"
#include "umicom/integration/session.h"
#include "umicom/platform/clock.h"
#include "umicom/platform/process_channel.h"
#include "umicom/platform/secure_random.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIXTURE_WAIT_MS 6000U
#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        goto finish; \
    } \
} while (0)

/* The registry alone exceeds the default thread stack on some hosts. */
typedef struct TestSuite {
    UmiIntegrationRegistry registry;
    UmiIntegrationApplication application;
    UmiIntegrationSuiteDefinition definition;
    UmiIntegrationSuiteRuntime runtime;
} TestSuite;

static uint64_t NowMs(const UmiClock *clock)
{
    return clock->monotonic_nanoseconds(clock) / UINT64_C(1000000);
}

/* Limit the whole frame, not each individual chunk. Ask the native channel
 * for only the current frame bytes so another frame cannot be discarded. */
static UmiStatus ReadReply(UmiProcessChannel *channel, const UmiClock *clock,
    UmiIntegrationWireSequenceWindow *window, UmiIntegrationEnvelope *response)
{
    UmiIntegrationWireDecoder decoder;
    UmiIntegrationWireDecoderInit(&decoder);
    uint64_t started = NowMs(clock);
    if (started == 0U) return UMI_STATUS_IO_ERROR;
    for (;;) {
        uint64_t now = NowMs(clock);
        if (now < started) return UMI_STATUS_IO_ERROR;
        if (now - started >= FIXTURE_WAIT_MS) return UMI_STATUS_TIMEOUT;
        unsigned remaining = (unsigned)(FIXTURE_WAIT_MS - (now - started));
        const size_t currentLength = decoder.expected != 0U ? decoder.expected : UMI_INTEGRATION_WIRE_HEADER_BYTES;
        uint8_t chunk[UMI_INTEGRATION_WIRE_MAX_BYTES];
        size_t received = 0U;
        UmiStatus status = UmiProcessChannelRead(channel, chunk, currentLength - decoder.buffered, &received, remaining);
        if (status != UMI_STATUS_OK) return status;
        if (received == 0U) {
            status = UmiIntegrationWireDecoderFinish(&decoder);
            return status == UMI_STATUS_OK ? UMI_STATUS_NOT_FOUND : status;
        }

        /* EOF and parse refusals cannot advance the persistent sequence. */
        bool complete = false;
        size_t consumed = 0U;
        uint64_t sequence = 0U;
        status = UmiIntegrationWireDecoderFeed(&decoder, chunk, received, &consumed, &complete, response, &sequence);
        if (status != UMI_STATUS_OK) return status;
        if (consumed != received) return UMI_STATUS_INTERNAL_ERROR;
        if (complete) return UmiIntegrationWireSequenceWindowAccept(window, sequence);
    }
}

static UmiStatus WriteQuery(UmiProcessChannel *channel, const UmiIntegrationEnvelope *request, uint64_t sequence)
{
    /* The native write accepts one bounded protocol frame. Propagate a
     * partial-write failure and let the owner close the connection. */
    uint8_t frame[UMI_INTEGRATION_WIRE_MAX_BYTES];
    size_t length = 0U;
    UmiStatus status = UmiIntegrationEnvelopeWireEncode(request, sequence, frame, sizeof frame, &length);
    if (status != UMI_STATUS_OK) return status;
    return UmiProcessChannelWrite(channel, frame, length, FIXTURE_WAIT_MS);
}

/* Use the existing OS random service. The readiness query carries a canonical
 * hexadecimal representation that is safe in the codec's text field. */
static UmiStatus CreateChallenge(UmiIntegrationEnvelope *request,
    uint8_t challenge[UMI_INTEGRATION_READINESS_CHALLENGE_BYTES])
{
    const char digits[] = "0123456789abcdef";
    UmiStatus status = UmiSecureRandomBytes(challenge, UMI_INTEGRATION_READINESS_CHALLENGE_BYTES);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0; index < UMI_INTEGRATION_READINESS_CHALLENGE_BYTES; ++index) {
        request->payload[2U * index] = digits[challenge[index] >> 4U];
        request->payload[2U * index + 1U] = digits[challenge[index] & 15U];
    }
    request->payload[2U * UMI_INTEGRATION_READINESS_CHALLENGE_BYTES] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus WaitForExit(UmiProcessChannel *channel, UmiClock *clock, UmiProcessChannelSnapshot *snapshot)
{
    uint64_t started = NowMs(clock);
    if (started == 0U) return UMI_STATUS_IO_ERROR;
    for (;;) {
        UmiStatus status = UmiProcessChannelPoll(channel, snapshot);
        if (status != UMI_STATUS_OK || !snapshot->running) return status;
        uint64_t now = NowMs(clock);
        if (now < started) return UMI_STATUS_IO_ERROR;
        if (now - started >= FIXTURE_WAIT_MS) return UMI_STATUS_TIMEOUT;
        /* Poll boundedly without spinning while the child closes its pipes. */
        status = clock->sleep_milliseconds(clock, 10U);
        if (status != UMI_STATUS_OK) return status;
    }
}

int main(int argc, char **argv)
{
    if (argc != 4) return EXIT_FAILURE;
    const char *mode = argv[1];
    const bool version = strcmp(mode, "wire-version") == 0;
    const bool truncated = strcmp(mode, "truncated") == 0;
    const bool cancelled = strcmp(mode, "cancelled") == 0;
    const bool other = strcmp(mode, "different-topic") == 0;
    if (!version && !truncated && !cancelled && !other && strcmp(mode, "ready") != 0)
        return EXIT_FAILURE;

    int outcome = EXIT_FAILURE;
    TestSuite *suite = calloc(1U, sizeof *suite);
    UmiIntegrationReadinessGate *gate = NULL;
    UmiProcessChannel *channel = NULL;
    UmiClock clock = umi_clock_system();
    CHECK(suite != NULL);
    umi_integration_registry_init(&suite->registry);
    umi_integration_application_init(&suite->application);
    CHECK(umi_integration_application_set_identity(&suite->application, "channel-worker", "Channel worker") == UMI_STATUS_OK);
    suite->application.enabled = true;
    CHECK(umi_integration_registry_register(&suite->registry, &suite->application) == UMI_STATUS_OK);
    umi_integration_suite_init(&suite->definition, "local-channel", "Local channel");
    CHECK(umi_integration_suite_add_member(&suite->definition, "channel-worker", UMI_INTEGRATION_DEPENDENCY_REQUIRED, 0U) == UMI_STATUS_OK);
    CHECK(umi_integration_suite_runtime_prepare(&suite->runtime, &suite->definition, &suite->registry) == UMI_STATUS_OK);
    CHECK(UmiIntegrationReadinessGateCreate(&suite->runtime, &gate) == UMI_STATUS_OK);

    /* CTest supplies the exact known worker executable. The native owner
     * creates its dedicated pipes and retains the process handle or PID. */
    UmiProcessChannelRequest launch = {argv[2], &mode, 1U, argv[3]};
    CHECK(UmiProcessChannelOpen(&launch, &channel) == UMI_STATUS_OK);
    UmiProcessChannelSnapshot snapshot;
    CHECK(UmiProcessChannelPoll(channel, &snapshot) == UMI_STATUS_OK && snapshot.running && snapshot.processId != 0U);
    const UmiIntegrationChannelPeer peer = {1U, snapshot.processId, 1U};
    UmiIntegrationSession session;
    CHECK(umi_integration_session_init(&session, "native-fixture", "channel-host") == UMI_STATUS_OK);
    UmiIntegrationWireSequenceWindow sequence;
    UmiIntegrationWireSequenceWindowInit(&sequence);
    UmiIntegrationEnvelope request = {.kind = UMI_INTEGRATION_MESSAGE_QUERY,
        .source_application = "channel-host", .target_application = "channel-worker", .topic = "service.readiness"};
    CHECK(umi_integration_session_next_correlation(&session, request.correlation_id, sizeof request.correlation_id) == UMI_STATUS_OK);
    uint8_t challenge[UMI_INTEGRATION_READINESS_CHALLENGE_BYTES];
    CHECK(CreateChallenge(&request, challenge) == UMI_STATUS_OK);
    uint64_t armed = NowMs(&clock);
    CHECK(armed != 0U);
    CHECK(UmiIntegrationReadinessGateArm(gate, "channel-worker", peer.processToken, peer.processGeneration,
        challenge, armed, FIXTURE_WAIT_MS) == UMI_STATUS_OK);
    CHECK(WriteQuery(channel, &request, 1U) == UMI_STATUS_OK);
    UmiIntegrationEnvelope reply;
    CHECK(ReadReply(channel, &clock, &sequence, &reply) == UMI_STATUS_OK);
    CHECK(reply.kind == UMI_INTEGRATION_MESSAGE_RESPONSE && strcmp(reply.topic, request.topic) == 0 &&
        strcmp(reply.source_application, request.target_application) == 0 &&
        strcmp(reply.target_application, request.source_application) == 0 &&
        strcmp(reply.correlation_id, request.correlation_id) == 0 && strcmp(reply.payload, request.payload) == 0);
    /* The exact echo is evidence from our launched child's private stdout.
     * Confirm only while that same owned native process is still running. */
    CHECK(UmiProcessChannelPoll(channel, &snapshot) == UMI_STATUS_OK && snapshot.running && snapshot.processId == peer.processToken);
    CHECK(UmiIntegrationReadinessGateConfirm(gate, "channel-worker", peer.processToken, peer.processGeneration,
        challenge, NowMs(&clock), true) == UMI_STATUS_OK);
    CHECK(UmiIntegrationReadinessGateIsReady(gate));

    /* Send the fixed status query. Exercise one native response outcome and
     * explicitly retire any request that cannot be delivered to its caller. */
    memcpy(request.topic, "service.status", sizeof "service.status");
    request.payload[0] = '\0';
    CHECK(umi_integration_session_next_correlation(&session, request.correlation_id, sizeof request.correlation_id) == UMI_STATUS_OK);
    UmiIntegrationReplyBinding binding;
    UmiIntegrationReplyBindingInit(&binding);
    CHECK(UmiIntegrationReplyBindingBegin(&binding, gate, &peer, &request) == UMI_STATUS_OK);
    CHECK(WriteQuery(channel, &request, 2U) == UMI_STATUS_OK);
    if (cancelled) CHECK(UmiIntegrationReplyBindingCancel(&binding) == UMI_STATUS_OK);
    if (truncated) CHECK(UmiProcessChannelCloseInput(channel) == UMI_STATUS_OK);
    UmiStatus status = ReadReply(channel, &clock, &sequence, &reply);
    if (version || truncated) {
        CHECK(status == UMI_STATUS_PARSE_ERROR && sequence.lastAccepted == 1U);
        CHECK(binding.state == UMI_INTEGRATION_REPLY_PENDING);
        CHECK(UmiIntegrationReplyBindingCancel(&binding) == UMI_STATUS_OK);
    } else {
        CHECK(status == UMI_STATUS_OK && sequence.lastAccepted == 2U);
        status = UmiIntegrationReplyBindingAccept(&binding, gate, &peer, &reply);
        if (cancelled) CHECK(status == UMI_STATUS_CANCELLED);
        else if (other) {
            CHECK(status == UMI_STATUS_NOT_FOUND);
            CHECK(UmiIntegrationReplyBindingCancel(&binding) == UMI_STATUS_OK);
        } else CHECK(status == UMI_STATUS_OK && strcmp(reply.payload, "ready") == 0);
    }

    /* The helper observes EOF, then the native owner observes a zero exit.
     * Reflect the real exit in the same gate before releasing the channel. */
    CHECK(UmiProcessChannelCloseInput(channel) == UMI_STATUS_OK);
    CHECK(WaitForExit(channel, &clock, &snapshot) == UMI_STATUS_OK && snapshot.exitCode == 0 && !snapshot.terminated);
    CHECK(UmiIntegrationReadinessGateExit(gate, "channel-worker", peer.processToken, peer.processGeneration,
        snapshot.exitCode) == UMI_STATUS_OK);
    CHECK(!UmiIntegrationReadinessGateIsReady(gate));
    printf("Native service channel completed %s with owned readiness, response and exit.\n", mode);
    outcome = EXIT_SUCCESS;

finish:
    if (channel != NULL) UmiProcessChannelDestroy(channel);
    UmiIntegrationReadinessGateDestroy(gate);
    umi_clock_dispose(&clock);
    free(suite);
    return outcome;
}
