/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration/native_channel_worker.c
 *
 * PURPOSE:
 *   Answer bounded readiness and status queries in a real native test child.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/integration/envelope.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

/* The helper reads just one frame at a time from its dedicated inherited
 * input. The Framework codec owns bounds, format admission and decoding. */
static bool ReadQuery(UmiIntegrationWireDecoder *decoder, UmiIntegrationWireSequenceWindow *window,
    const char *topic, UmiIntegrationEnvelope *request)
{
    bool complete = false;
    uint64_t sequence = 0U;
    while (!complete) {
        int value = getc(stdin);
        if (value == EOF) return false;
        uint8_t byte = (uint8_t)value;
        size_t used = 0U;
        if (UmiIntegrationWireDecoderFeed(decoder, &byte, 1U, &used, &complete, request, &sequence) != UMI_STATUS_OK ||
            used != 1U) return false;
    }
    return UmiIntegrationWireSequenceWindowAccept(window, sequence) == UMI_STATUS_OK &&
        request->kind == UMI_INTEGRATION_MESSAGE_QUERY &&
        strcmp(request->source_application, "channel-host") == 0 &&
        strcmp(request->target_application, "channel-worker") == 0 &&
        strcmp(request->topic, topic) == 0 && request->correlation_id[0] != '\0';
}

/* The child implements only the fixed teaching protocol. It never acts on
 * request text as a command, file path, network destination or executable. */
static bool Reply(const UmiIntegrationEnvelope *request, const char *payload,
    uint64_t sequence, const char *testCase)
{
    UmiIntegrationEnvelope response = {.kind = UMI_INTEGRATION_MESSAGE_RESPONSE,
        .source_application = "channel-worker", .target_application = "channel-host"};
    const char *topic = strcmp(testCase, "different-topic") == 0 ? "service.other" : request->topic;
    memcpy(response.topic, topic, strlen(topic) + 1U);
    memcpy(response.correlation_id, request->correlation_id, sizeof response.correlation_id);
    size_t payloadLength = strlen(payload);
    if (payloadLength >= sizeof response.payload) return false;
    memcpy(response.payload, payload, payloadLength + 1U);

    uint8_t frame[UMI_INTEGRATION_WIRE_MAX_BYTES];
    size_t length = 0U;
    if (UmiIntegrationEnvelopeWireEncode(&response, sequence, frame, sizeof frame, &length) != UMI_STATUS_OK)
        return false;
    /* The two deliberately invalid test outputs occur only after readiness. */
    if (strcmp(testCase, "wire-version") == 0) frame[4] = 0U;
    if (strcmp(testCase, "truncated") == 0) --length;
    return fwrite(frame, 1U, length, stdout) == length && fflush(stdout) == 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return EXIT_FAILURE;
#ifdef _WIN32
    /* Wire bytes must bypass CRT newline and Ctrl-Z transformations. */
    if (_setmode(_fileno(stdin), _O_BINARY) == -1 || _setmode(_fileno(stdout), _O_BINARY) == -1)
        return EXIT_FAILURE;
#endif
    UmiIntegrationWireDecoder decoder;
    UmiIntegrationWireSequenceWindow sequence;
    UmiIntegrationWireDecoderInit(&decoder);
    UmiIntegrationWireSequenceWindowInit(&sequence);
    UmiIntegrationEnvelope request;

    /* Echo the host's fresh challenge before taking the status request. */
    if (!ReadQuery(&decoder, &sequence, "service.readiness", &request) ||
        strlen(request.payload) != 64U || !Reply(&request, request.payload, 1U, "ready"))
        return EXIT_FAILURE;
    if (!ReadQuery(&decoder, &sequence, "service.status", &request) ||
        !Reply(&request, "ready", 2U, argv[1]))
        return EXIT_FAILURE;

    /* Keep the legitimate child alive until the host closes its owned input.
     * The truncated frame therefore ends at a real EOF, not an arbitrary read. */
    return getc(stdin) == EOF && feof(stdin) ? EXIT_SUCCESS : EXIT_FAILURE;
}
