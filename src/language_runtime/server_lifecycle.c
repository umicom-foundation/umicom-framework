/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/server_lifecycle.c
 * PURPOSE: Coordinate a cancellable native language handshake and deterministic direct-child cleanup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/server_manager.h"
#include "umicom/platform/clock.h"
#include <stdlib.h>
#include <string.h>

/* Replies share one monotonic deadline. A short read slice lets a GUI's worker
 * observe cancellation even when the child is quiet. A memory/test transport
 * may return immediately, so yield after an empty read to avoid spinning. */
static UmiStatus LanguageAwaitReply(UmiLanguageRuntimeServer *server, uint64_t request, UmiClock *clock,
                                    uint64_t start, uint32_t timeoutMs, const UmiCancellationToken *cancel,
                                    UmiLanguageRuntimeEnvelope *reply)
{
    unsigned messages = 0U;
    for (;;)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        uint64_t elapsed = (clock->monotonic_nanoseconds(clock) - start) / 1000000U;
        if (elapsed >= timeoutMs && messages != 0U)
            return UMI_STATUS_TIMEOUT;
        uint32_t remaining = elapsed >= timeoutMs ? 0U : timeoutMs - (uint32_t)elapsed;
        uint32_t slice = remaining > 50U ? 50U : remaining;
        UmiStatus status = umi_language_runtime_server_receive(server, slice, reply);
        if (status == UMI_STATUS_OK)
        {
            ++messages;
            if (reply->request_id == request && (reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE ||
                                                 reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR))
                return reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR ? UMI_STATUS_UNAVAILABLE
                                                                         : UMI_STATUS_OK;
            /* Bound floods even if a clock or synthetic transport cannot
             * advance. This does not turn unrelated messages into success. */
            if (messages >= 4096U)
                return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        else if (status != UMI_STATUS_NOT_FOUND)
            return status;
        if (!umi_language_runtime_server_is_running(server))
            return UMI_STATUS_UNAVAILABLE;
        elapsed = (clock->monotonic_nanoseconds(clock) - start) / 1000000U;
        if (elapsed >= timeoutMs)
            return UMI_STATUS_TIMEOUT;
        if (status == UMI_STATUS_NOT_FOUND)
            (void)clock->sleep_milliseconds(clock, 1U);
    }
}

UmiStatus UmiLanguageRuntimeServerInitialize(UmiLanguageRuntimeServer *server, const char *rootUri,
                                             uint32_t timeoutMs, const UmiCancellationToken *cancel,
                                             UmiLanguageRuntimeInitializeResult *outCapabilities)
{
    if (server == NULL || rootUri == NULL || rootUri[0] == '\0' || outCapabilities == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiLanguageRuntimeServerSnapshot snapshot;
    UmiStatus status = umi_language_runtime_server_snapshot(server, &snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    if (snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_STARTING || strcmp(snapshot.root_uri, rootUri) != 0)
        return UMI_STATUS_INVALID_STATE;
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiClock clock = umi_clock_system();
    uint64_t start = clock.monotonic_nanoseconds(&clock), request = 0U;
    UmiLanguageRuntimeInitializeResult capabilities = {0};
    status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialize(server, rootUri, &request);
    if (status == UMI_STATUS_OK)
        status = LanguageAwaitReply(server, request, &clock, start, timeoutMs, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_decode_initialize(reply->json, &capabilities);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
        *outCapabilities = capabilities;
    else
        (void)umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_FAILED);
    free(reply);
    return status;
}

UmiStatus UmiLanguageRuntimeServerShutdown(UmiLanguageRuntimeServer *server, uint32_t timeoutMs)
{
    if (server == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiClock clock = umi_clock_system();
    uint64_t start = clock.monotonic_nanoseconds(&clock), request = 0U;
    UmiStatus status = UMI_STATUS_OK;
    if (umi_language_runtime_server_is_running(server))
    {
        UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
        if (reply == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            status = umi_language_runtime_request_shutdown(server, &request);
            if (status == UMI_STATUS_OK)
                status = LanguageAwaitReply(server, request, &clock, start, timeoutMs, NULL, reply);
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_request_exit(server);
            free(reply);
        }
    }
    uint64_t elapsed = (clock.monotonic_nanoseconds(&clock) - start) / 1000000U;
    uint32_t remaining = elapsed >= timeoutMs ? 0U : timeoutMs - (uint32_t)elapsed;
    /* Cleanup is unconditional. A malformed or absent shutdown reply must not
     * leave a language server running after its owner closes the workspace. */
    UmiStatus cleanup = umi_language_runtime_server_stop(server, remaining);
    return status == UMI_STATUS_OK ? cleanup : status;
}
