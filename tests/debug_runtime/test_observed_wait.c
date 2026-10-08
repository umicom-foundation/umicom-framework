/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_observed_wait.c
 * PURPOSE: Verify debugger event observation, refusal and nested response correlation using an in-memory transport.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Probe
{
    UmiDebugRuntimeAdapter *adapter;
    UmiDebugRuntimeEnvelope *nestedResponse;
    const char *mode;
    unsigned calls;
} Probe;
static UmiStatus Observe(void *context, const UmiDebugRuntimeEnvelope *event, uint32_t remaining,
                         int *consumed)
{
    Probe *probe = context;
    ++probe->calls;
    *consumed = 0;
    if (event->kind != UMI_DEBUG_RUNTIME_MESSAGE_EVENT)
        return UMI_STATUS_PARSE_ERROR;
    if (!strcmp(probe->mode, "consume") || !strcmp(probe->mode, "consumed-refusal"))
        *consumed = 1;
    if (!strcmp(probe->mode, "refusal") || !strcmp(probe->mode, "consumed-refusal"))
        return UMI_STATUS_PERMISSION_DENIED;
    if (!strcmp(probe->mode, "nested"))
    {
        *consumed = 1;
        uint64_t sequence = 0U;
        UmiStatus status =
            umi_debug_runtime_adapter_send_request(probe->adapter, "configurationDone", NULL, "", &sequence);
        if (status != UMI_STATUS_OK)
            return status;
        return umi_debug_runtime_adapter_wait_response(probe->adapter, sequence, remaining,
                                                       probe->nestedResponse);
    }
    return UMI_STATUS_OK;
}
static UmiStatus Push(UmiDebugRuntimeMemoryTransport *memory, const char *json)
{
    char frame[2048];
    size_t bytes = 0U;
    UmiStatus status = umi_language_runtime_frame_encode(json, frame, sizeof frame, &bytes);
    return status == UMI_STATUS_OK ? umi_debug_runtime_memory_transport_push_read(memory, frame, bytes)
                                   : status;
}
static int Run(UmiDebugRuntimeAdapter *adapter, UmiDebugRuntimeMemoryTransport *memory,
               UmiDebugRuntimeEnvelope *response, UmiDebugRuntimeEnvelope *nested, const char *mode)
{
    uint64_t request = 0U;
    CHECK(umi_debug_runtime_adapter_send_request(adapter, "restart", NULL, "", &request) == UMI_STATUS_OK);
    CHECK(request == 1U);
    Probe probe = {adapter, nested, mode, 0U};
    response->sequence = 999U;
    if (!strcmp(mode, "invalid"))
    {
        CHECK(UmiDebugRuntimeAdapterWaitObserved(NULL, request, 1000U, Observe, &probe, response) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(response->sequence == 999U);
        return 0;
    }
    if (!strcmp(mode, "unrelated"))
    {
        uint64_t other = 0U;
        CHECK(umi_debug_runtime_adapter_send_request(adapter, "threads", NULL, "", &other) == UMI_STATUS_OK &&
              other == 2U);
        CHECK(Push(memory, "{\"seq\":10,\"type\":\"response\",\"request_seq\":2,\"command\":\"threads\","
                           "\"success\":true,\"body\":{}}") == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "zero") && strcmp(mode, "failed-reply"))
    {
        CHECK(Push(memory, "{\"seq\":10,\"type\":\"event\",\"event\":\"initialized\",\"body\":{}}") ==
              UMI_STATUS_OK);
    }
    CHECK(Push(memory, !strcmp(mode, "failed-reply")
                           ? "{\"seq\":11,\"type\":\"response\",\"request_seq\":1,\"command\":\"restart\","
                             "\"success\":false,\"body\":{}}"
                           : "{\"seq\":11,\"type\":\"response\",\"request_seq\":1,\"command\":\"restart\","
                             "\"success\":true,\"body\":{}}") == UMI_STATUS_OK);
    if (!strcmp(mode, "nested"))
        CHECK(Push(memory, "{\"seq\":12,\"type\":\"response\",\"request_seq\":2,\"command\":"
                           "\"configurationDone\",\"success\":true,\"body\":{}}") == UMI_STATUS_OK);
    if (!strcmp(mode, "zero"))
    {
        CHECK(UmiDebugRuntimeAdapterWaitObserved(adapter, request, 0U, Observe, &probe, response) ==
              UMI_STATUS_TIMEOUT);
        UmiDebugRuntimeAdapterSnapshot snapshot;
        CHECK(umi_debug_runtime_adapter_snapshot(adapter, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.messages_received == 0U && response->sequence == 999U);
    }
    UmiStatus status = UmiDebugRuntimeAdapterWaitObserved(adapter, request, 1000U, Observe, &probe, response);
    if (!strcmp(mode, "refusal") || !strcmp(mode, "consumed-refusal"))
    {
        CHECK(status == UMI_STATUS_PERMISSION_DENIED && response->sequence == 999U && probe.calls == 1U);
        status = umi_debug_runtime_adapter_next_event(adapter, nested);
        CHECK(status == (!strcmp(mode, "refusal") ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND));
        return 0;
    }
    CHECK(status == (!strcmp(mode, "failed-reply") ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK));
    CHECK(response->request_sequence == request && !strcmp(response->command, "restart"));
    if (!strcmp(mode, "unrelated"))
    {
        CHECK(UmiDebugRuntimeAdapterWaitObserved(adapter, 2U, 0U, NULL, NULL, nested) == UMI_STATUS_OK);
        CHECK(!strcmp(nested->command, "threads"));
        return 0;
    }
    if (!strcmp(mode, "decline"))
    {
        CHECK(probe.calls == 1U);
        CHECK(umi_debug_runtime_adapter_next_event(adapter, nested) == UMI_STATUS_OK);
        CHECK(!strcmp(nested->event, "initialized"));
        return 0;
    }
    if (!strcmp(mode, "nested"))
        CHECK(probe.calls == 1U && !strcmp(nested->command, "configurationDone"));
    else if (!strcmp(mode, "consume"))
        CHECK(probe.calls == 1U);
    else if (!strcmp(mode, "zero") || !strcmp(mode, "failed-reply"))
        CHECK(probe.calls == 0U);
    else
        return 2;
    CHECK(umi_debug_runtime_adapter_next_event(adapter, nested) == UMI_STATUS_NOT_FOUND);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiDebugRuntimeMemoryTransport *memory = NULL;
    UmiDebugRuntimeTransport transport;
    UmiDebugRuntimeAdapter *adapter = NULL;
    UmiDebugRuntimeEnvelope *response = calloc(1, sizeof *response), *nested = calloc(1, sizeof *nested);
    if (response == NULL || nested == NULL)
    {
        free(response);
        free(nested);
        return 1;
    }
    if (umi_debug_runtime_memory_transport_create(&memory, &transport) != UMI_STATUS_OK)
    {
        free(response);
        free(nested);
        return 1;
    }
    if (umi_debug_runtime_adapter_create_with_transport("observed", &transport, &adapter) != UMI_STATUS_OK)
    {
        if (transport.destroy != NULL)
            transport.destroy(transport.instance);
        free(response);
        free(nested);
        return 1;
    }
    int result = Run(adapter, memory, response, nested, argv[1]);
    umi_debug_runtime_adapter_destroy(adapter);
    free(response);
    free(nested);
    return result;
}
