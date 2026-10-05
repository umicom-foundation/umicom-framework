/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_server_lifecycle.c
 * PURPOSE: Exercise handshake correlation, cancellation, timeout budgets and manager ownership without external tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/server_manager.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                                         \
    do                                                                                                       \
    {                                                                                                        \
        if (!(value))                                                                                        \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                                      \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)

typedef struct Fixture
{
    char input[8192], written[8192];
    size_t size, offset, writeSize, fragment;
    unsigned reads, stops, destroys, delay;
    uint32_t largestTimeout, firstTimeout, lastTimeout;
    int running, failWrite, failRead, oversize;
    UmiCancellationToken *cancel;
} Fixture;

static UmiStatus Write(void *instance, const void *bytes, size_t size)
{
    Fixture *fixture = instance;
    if (fixture->failWrite)
        return UMI_STATUS_IO_ERROR;
    CHECK(size < sizeof(fixture->written) - fixture->writeSize);
    memcpy(fixture->written + fixture->writeSize, bytes, size);
    fixture->writeSize += size;
    fixture->written[fixture->writeSize] = '\0';
    return UMI_STATUS_OK;
}
static UmiStatus Read(void *instance, void *bytes, size_t capacity, uint32_t timeout, size_t *out)
{
    Fixture *fixture = instance;
    if (fixture->reads == 0U)
        fixture->firstTimeout = timeout;
    fixture->lastTimeout = timeout;
    ++fixture->reads;
    if (timeout > fixture->largestTimeout)
        fixture->largestTimeout = timeout;
    *out = 0U;
    if (fixture->cancel != NULL)
        umi_cancellation_token_request(fixture->cancel);
    if (fixture->failRead)
        return UMI_STATUS_IO_ERROR;
    if (fixture->oversize)
    {
        *out = capacity + 1U;
        return UMI_STATUS_OK;
    }
    if (fixture->delay != 0U)
    {
        UmiClock clock = umi_clock_system();
        (void)clock.sleep_milliseconds(&clock, fixture->delay);
    }
    if (fixture->offset == fixture->size)
        return UMI_STATUS_NOT_FOUND;
    size_t count = fixture->size - fixture->offset;
    if (count > capacity)
        count = capacity;
    if (fixture->fragment != 0U && count > fixture->fragment)
        count = fixture->fragment;
    memcpy(bytes, fixture->input + fixture->offset, count);
    fixture->offset += count;
    *out = count;
    return UMI_STATUS_OK;
}
static UmiStatus Stop(void *instance, uint32_t timeout)
{
    Fixture *fixture = instance;
    (void)timeout;
    ++fixture->stops;
    fixture->running = 0;
    return UMI_STATUS_OK;
}
static int Running(void *instance) { return ((Fixture *)instance)->running; }
static void Destroy(void *instance) { ++((Fixture *)instance)->destroys; }

static void Push(Fixture *fixture, const char *json)
{
    size_t size = 0U;
    CHECK(umi_language_runtime_frame_encode(json, fixture->input + fixture->size,
                                            sizeof(fixture->input) - fixture->size, &size) == UMI_STATUS_OK);
    fixture->size += size;
}
static UmiLanguageRuntimeServer *Server(Fixture *fixture, const char *root)
{
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "fixture");
    strcpy(profile.executable, "fixture");
    profile.enabled = 1;
    UmiLanguageRuntimeTransport transport = {fixture, Write, Read, Stop, Running, Destroy};
    UmiLanguageRuntimeServer *server = NULL;
    fixture->running = 1;
    CHECK(umi_language_runtime_server_create_with_transport("fixture-server", &profile, root, &transport,
                                                            &server) == UMI_STATUS_OK);
    CHECK(transport.instance == NULL);
    return server;
}
static UmiLanguageRuntimeServerState State(UmiLanguageRuntimeServer *server)
{
    UmiLanguageRuntimeServerSnapshot snapshot;
    CHECK(umi_language_runtime_server_snapshot(server, &snapshot) == UMI_STATUS_OK);
    return snapshot.state;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    const char *known[] = {"valid",
                           "zero",
                           "fragmented",
                           "notification",
                           "wrong-id",
                           "request-id",
                           "cancel-before",
                           "cancel-during",
                           "timeout",
                           "write-error",
                           "read-error",
                           "server-error",
                           "invalid-result",
                           "receive-atomic",
                           "receive-oversize",
                           "receive-deadline",
                           "attach-starting",
                           "attach-ready",
                           "attach-duplicate",
                           "attach-root",
                           "attach-failed",
                           "configured-disabled",
                           "configured-invalid",
                           "configured-cancel",
                           "shutdown-ok",
                           "shutdown-timeout",
                           "shutdown-write",
                           "shutdown-error",
                           "create-root-long",
                           "create-id-long",
                           "create-invalid-profile",
                           "create-ownership",
                           "configured-preserved"};
    int accepted = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            accepted = 1;
    if (!accepted)
        return 2;
    Fixture fixture = {0};
    UmiLanguageRuntimeServer *server = Server(&fixture, "file:///project");
    UmiLanguageRuntimeInitializeResult capabilities, before;
    memset(&before, 0x39, sizeof(before));
    capabilities = before;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);

    if (strncmp(mode, "create-", 7U) == 0)
    {
        Fixture candidateFixture = {0};
        UmiLanguageRuntimeTransport transport = {&candidateFixture, Write, Read, Stop, Running, Destroy};
        UmiLanguageServerProfile profile = {0};
        strcpy(profile.id, "candidate");
        strcpy(profile.executable, "fixture");
        char longText[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY + 1U];
        memset(longText, 'x', sizeof(longText) - 1U);
        longText[sizeof(longText) - 1U] = '\0';
        const char *id = strcmp(mode, "create-id-long") == 0 ? longText : "candidate";
        const char *root = strcmp(mode, "create-root-long") == 0 ? longText : "file:///project";
        UmiStatus constructorStatus = UMI_STATUS_OK;
        if (strcmp(mode, "create-invalid-profile") == 0)
        {
            memset(profile.arguments, 'x', sizeof(profile.arguments));
            constructorStatus = UMI_STATUS_INVALID_ARGUMENT;
        }
        else if (strcmp(mode, "create-ownership") != 0)
            constructorStatus = UMI_STATUS_CAPACITY_EXCEEDED;
        UmiLanguageRuntimeServer *candidate = NULL;
        CHECK(umi_language_runtime_server_create_with_transport(id, &profile, root, &transport, &candidate) ==
              constructorStatus);
        if (constructorStatus == UMI_STATUS_OK)
        {
            CHECK(transport.instance == NULL && candidate != NULL);
            umi_language_runtime_server_destroy(candidate);
            CHECK(candidateFixture.destroys == 1U);
        }
        else
        {
            CHECK(candidate == NULL && transport.instance == &candidateFixture &&
                  candidateFixture.destroys == 0U);
            transport.destroy(transport.instance);
        }
        umi_language_runtime_server_destroy(server);
        umi_cancellation_token_destroy(cancel);
        return 0;
    }
    if (strcmp(mode, "configured-preserved") == 0)
    {
        UmiLanguageService *language = NULL;
        UmiLanguageRuntimeServerManager *manager = NULL;
        CHECK(umi_language_service_create(&language) == UMI_STATUS_OK);
        UmiLanguageServerProfile choice = *umi_language_runtime_builtin_profile_for_language("c"), actual;
        strcpy(choice.executable, "user-selected-executable");
        strcpy(choice.arguments, "--selected");
        choice.enabled = 0;
        CHECK(umi_language_server_profile_registry_upsert(umi_language_service_server_profiles(language),
                                                          &choice) == UMI_STATUS_OK);
        CHECK(umi_language_runtime_server_manager_create(language, &manager) == UMI_STATUS_OK);
        CHECK(umi_language_server_profile_registry_find(umi_language_service_server_profiles(language),
                                                        choice.id, &actual) == UMI_STATUS_OK);
        CHECK(strcmp(actual.executable, choice.executable) == 0 &&
              strcmp(actual.arguments, choice.arguments) == 0 && !actual.enabled);
        UmiLanguageRuntimeServer *result = NULL;
        CHECK(umi_language_runtime_server_manager_start_for_language(manager, "c", "file:///project", NULL,
                                                                     0U, &result) == UMI_STATUS_UNAVAILABLE &&
              result == NULL);
        umi_language_runtime_server_manager_destroy(manager);
        umi_language_service_destroy(language);
        umi_language_runtime_server_destroy(server);
        umi_cancellation_token_destroy(cancel);
        return 0;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strncmp(mode, "attach-", 7U) == 0 || strcmp(mode, "configured-disabled") == 0 ||
        strcmp(mode, "configured-invalid") == 0 || strcmp(mode, "configured-cancel") == 0)
    {
        UmiLanguageService *language = NULL;
        UmiLanguageRuntimeServerManager *manager = NULL;
        CHECK(umi_language_service_create(&language) == UMI_STATUS_OK);
        CHECK(umi_language_runtime_server_manager_create(language, &manager) == UMI_STATUS_OK);
        if (strncmp(mode, "configured-", 11U) == 0)
        {
            UmiLanguageServerProfile profile = {0};
            strcpy(profile.id, "fixture");
            strcpy(profile.executable, "must-not-start");
            profile.enabled = strcmp(mode, "configured-disabled") != 0;
            expected = UMI_STATUS_UNAVAILABLE;
            if (strcmp(mode, "configured-invalid") == 0)
            {
                memset(profile.arguments, 'x', sizeof(profile.arguments));
                expected = UMI_STATUS_INVALID_ARGUMENT;
            }
            if (strcmp(mode, "configured-cancel") == 0)
            {
                umi_cancellation_token_request(cancel);
                expected = UMI_STATUS_CANCELLED;
            }
            UmiLanguageRuntimeServer *result = server;
            CHECK(UmiLanguageRuntimeServerManagerStartProfile(manager, "c", &profile, "file:///project", NULL,
                                                              0U, cancel, &result) == expected &&
                  result == NULL);
            CHECK(umi_language_runtime_server_manager_count(manager) == 0U);
        }
        else
        {
            UmiLanguageRuntimeInitializeResult supplied = {0};
            supplied.completion = 1;
            if (strcmp(mode, "attach-ready") == 0)
            {
                CHECK(umi_language_runtime_server_transition(
                          server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING) == UMI_STATUS_OK);
                CHECK(umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY) ==
                      UMI_STATUS_OK);
            }
            const char *root = strcmp(mode, "attach-root") == 0 ? "file:///other" : "file:///project";
            if (strcmp(mode, "attach-failed") == 0)
                CHECK(umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_FAILED) ==
                      UMI_STATUS_OK);
            expected = strcmp(mode, "attach-root") == 0     ? UMI_STATUS_INVALID_ARGUMENT
                       : strcmp(mode, "attach-failed") == 0 ? UMI_STATUS_INVALID_STATE
                                                            : UMI_STATUS_OK;
            CHECK(umi_language_runtime_server_manager_attach(manager, "c", root, server, &supplied) ==
                  expected);
            if (expected == UMI_STATUS_OK)
            {
                CHECK(State(server) == UMI_LANGUAGE_RUNTIME_SERVER_READY);
                CHECK(umi_language_runtime_server_manager_count(manager) == 1U);
                if (strcmp(mode, "attach-duplicate") == 0)
                {
                    CHECK(umi_language_runtime_server_manager_attach(manager, "cpp", "file:///project",
                                                                     server,
                                                                     &supplied) == UMI_STATUS_ALREADY_EXISTS);
                    CHECK(umi_language_runtime_server_manager_count(manager) == 1U);
                }
                server = NULL; /* Ownership moved only on successful attachment. */
            }
        }
        umi_language_runtime_server_manager_destroy(manager);
        umi_language_service_destroy(language);
        umi_language_runtime_server_destroy(server);
        CHECK(fixture.destroys == 1U);
        umi_cancellation_token_destroy(cancel);
        return 0;
    }
    if (strncmp(mode, "shutdown-", 9U) == 0)
    {
        CHECK(umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING) ==
              UMI_STATUS_OK);
        CHECK(umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY) ==
              UMI_STATUS_OK);
        if (strcmp(mode, "shutdown-ok") == 0)
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":null}");
        else if (strcmp(mode, "shutdown-write") == 0)
        {
            fixture.failWrite = 1;
            expected = UMI_STATUS_IO_ERROR;
        }
        else if (strcmp(mode, "shutdown-error") == 0)
        {
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"error\":{\"code\":-1,\"message\":\"no\"}}");
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else if (strcmp(mode, "shutdown-timeout") == 0)
            expected = UMI_STATUS_TIMEOUT;
        else
            return 2;
        CHECK(UmiLanguageRuntimeServerShutdown(server, 0U) == expected);
        CHECK(fixture.stops == 1U && !fixture.running);
        CHECK(State(server) == UMI_LANGUAGE_RUNTIME_SERVER_STOPPED);
        CHECK((strstr(fixture.written, "\"method\":\"exit\"") != NULL) == (expected == UMI_STATUS_OK));
    }
    else if (strcmp(mode, "receive-atomic") == 0 || strcmp(mode, "receive-oversize") == 0 ||
             strcmp(mode, "receive-deadline") == 0)
    {
        UmiLanguageRuntimeEnvelope *output = malloc(sizeof(*output)), *old = malloc(sizeof(*old));
        CHECK(output != NULL && old != NULL);
        memset(old, 0x47, sizeof(*old));
        *output = *old;
        if (strcmp(mode, "receive-atomic") == 0)
        {
            Push(&fixture, "{");
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "receive-oversize") == 0)
        {
            fixture.oversize = 1;
            expected = UMI_STATUS_INVALID_STATE;
        }
        else
        {
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":null}");
            fixture.fragment = 1U;
            fixture.delay = 3U;
            expected = UMI_STATUS_NOT_FOUND;
        }
        CHECK(umi_language_runtime_server_receive(server, 8U, output) == expected);
        CHECK(memcmp(old, output, sizeof(*old)) == 0);
        if (strcmp(mode, "receive-deadline") == 0)
        {
            CHECK(fixture.reads < fixture.size && fixture.lastTimeout <= fixture.firstTimeout);
            fixture.fragment = fixture.delay = 0U;
            CHECK(umi_language_runtime_server_receive(server, 0U, output) == UMI_STATUS_OK);
        }
        free(old);
        free(output);
    }
    else
    {
        uint32_t timeout = 100U;
        if (strcmp(mode, "cancel-before") == 0)
        {
            umi_cancellation_token_request(cancel);
            expected = UMI_STATUS_CANCELLED;
        }
        else if (strcmp(mode, "cancel-during") == 0)
        {
            fixture.cancel = cancel;
            expected = UMI_STATUS_CANCELLED;
        }
        else if (strcmp(mode, "timeout") == 0)
        {
            timeout = 2U;
            expected = UMI_STATUS_TIMEOUT;
        }
        else if (strcmp(mode, "write-error") == 0)
        {
            fixture.failWrite = 1;
            expected = UMI_STATUS_IO_ERROR;
        }
        else if (strcmp(mode, "read-error") == 0)
        {
            fixture.failRead = 1;
            expected = UMI_STATUS_IO_ERROR;
        }
        else if (strcmp(mode, "server-error") == 0)
        {
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"error\":{\"code\":-1,\"message\":\"no\"}}");
            expected = UMI_STATUS_UNAVAILABLE;
        }
        else if (strcmp(mode, "invalid-result") == 0)
        {
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":null}");
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else
        {
            if (strcmp(mode, "notification") == 0)
                Push(&fixture, "{\"jsonrpc\":\"2.0\",\"method\":\"window/"
                               "logMessage\",\"params\":{\"message\":\"starting\"}}");
            if (strcmp(mode, "wrong-id") == 0)
                Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":99,\"result\":null}");
            if (strcmp(mode, "request-id") == 0)
                Push(&fixture,
                     "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"window/showMessageRequest\",\"params\":{}}");
            Push(&fixture, "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{"
                           "\"completionProvider\":{},\"hoverProvider\":true}}}");
            if (strcmp(mode, "fragmented") == 0)
                fixture.fragment = 7U;
            if (strcmp(mode, "zero") == 0)
                timeout = 0U;
        }
        UmiStatus status =
            UmiLanguageRuntimeServerInitialize(server, "file:///project", timeout, cancel, &capabilities);
        CHECK(status == expected);
        CHECK(fixture.largestTimeout <= 50U);
        if (status == UMI_STATUS_OK)
        {
            CHECK(capabilities.completion && capabilities.hover);
            CHECK(State(server) == UMI_LANGUAGE_RUNTIME_SERVER_READY);
            CHECK(strstr(fixture.written, "\"method\":\"initialized\"") != NULL);
            CHECK(UmiLanguageRuntimeServerInitialize(server, "file:///project", 0U, NULL, &capabilities) ==
                  UMI_STATUS_INVALID_STATE);
        }
        else
        {
            CHECK(memcmp(&capabilities, &before, sizeof(before)) == 0);
            CHECK(strstr(fixture.written, "\"method\":\"initialized\"") == NULL);
            CHECK(State(server) == (strcmp(mode, "cancel-before") == 0 ? UMI_LANGUAGE_RUNTIME_SERVER_STARTING
                                                                       : UMI_LANGUAGE_RUNTIME_SERVER_FAILED));
        }
    }
    umi_language_runtime_server_destroy(server);
    CHECK(fixture.destroys == 1U);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
