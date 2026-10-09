/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/diagnostic_session/fixture.h
 * PURPOSE: Provide isolated framed transports and explicit publication sequences for session regression coverage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DIAGNOSTIC_SESSION_TEST_FIXTURE_H
#define UMICOM_DIAGNOSTIC_SESSION_TEST_FIXTURE_H
#include "umicom/language_runtime/diagnostic_session.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
typedef struct DiagnosticFixture
{
    char input[262144], written[262144];
    size_t size, offset, written_bytes, fragment;
    unsigned stops, destroys, reads;
    int running, fail_write, fail_read;
    UmiCancellationToken *cancel;
} DiagnosticFixture;
static UmiStatus DiagnosticWrite(void *context, const void *bytes, size_t size)
{
    DiagnosticFixture *fixture = context;
    if (fixture->fail_write)
        return UMI_STATUS_IO_ERROR;
    if (size >= sizeof fixture->written - fixture->written_bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(fixture->written + fixture->written_bytes, bytes, size);
    fixture->written_bytes += size;
    fixture->written[fixture->written_bytes] = '\0';
    return UMI_STATUS_OK;
}
static UmiStatus DiagnosticRead(void *context, void *bytes, size_t capacity, uint32_t timeout,
                                size_t *out)
{
    (void)timeout;
    DiagnosticFixture *fixture = context;
    ++fixture->reads;
    *out = 0U;
    if (fixture->cancel != NULL)
        umi_cancellation_token_request(fixture->cancel);
    if (fixture->fail_read)
        return UMI_STATUS_IO_ERROR;
    size_t count = fixture->size - fixture->offset;
    if (count == 0U)
        return UMI_STATUS_NOT_FOUND;
    if (count > capacity)
        count = capacity;
    if (fixture->fragment != 0U && count > fixture->fragment)
        count = fixture->fragment;
    memcpy(bytes, fixture->input + fixture->offset, count);
    fixture->offset += count;
    *out = count;
    return UMI_STATUS_OK;
}
static UmiStatus DiagnosticStop(void *context, uint32_t timeout)
{
    (void)timeout;
    DiagnosticFixture *fixture = context;
    fixture->running = 0;
    ++fixture->stops;
    return UMI_STATUS_OK;
}
static int DiagnosticRunning(void *context) { return ((DiagnosticFixture *)context)->running; }
static void DiagnosticDestroy(void *context) { ++((DiagnosticFixture *)context)->destroys; }
static inline void DiagnosticPush(DiagnosticFixture *fixture, const char *json)
{
    size_t bytes = 0U;
    CHECK(umi_language_runtime_frame_encode(json, fixture->input + fixture->size,
                                            sizeof fixture->input - fixture->size,
                                            &bytes) == UMI_STATUS_OK);
    fixture->size += bytes;
}
static inline UmiLanguageRuntimeServer *DiagnosticServer(DiagnosticFixture *fixture)
{
    UmiLanguageServerProfile profile = {0};
    strcpy(profile.id, "diagnostic-fixture");
    strcpy(profile.executable, "fixture");
    profile.enabled = 1;
    UmiLanguageRuntimeTransport transport = {fixture,        DiagnosticWrite,   DiagnosticRead,
                                             DiagnosticStop, DiagnosticRunning, DiagnosticDestroy};
    UmiLanguageRuntimeServer *server = NULL;
    fixture->running = 1;
    CHECK(umi_language_runtime_server_create_with_transport("diagnostic-fixture", &profile,
                                                            "file:///workspace", &transport,
                                                            &server) == UMI_STATUS_OK);
    CHECK(transport.instance == NULL);
    return server;
}
static inline void DiagnosticCaps(DiagnosticFixture *fixture, const char *caps)
{
    char response[4096];
    CHECK(snprintf(response, sizeof response,
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":%s}}", caps) > 0);
    DiagnosticPush(fixture, response);
}
static inline UmiLanguageDiagnosticRequest DiagnosticRequest(void)
{
    return (UmiLanguageDiagnosticRequest){
        "file:///workspace", "file:///workspace/main.c", "c", "abc", 3U, 2000U};
}
static inline void DiagnosticPublication(DiagnosticFixture *fixture, const char *uri,
                                         const char *version, const char *items)
{
    char response[8192];
    int length = snprintf(response, sizeof response,
                          "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                          "publishDiagnostics\",\"params\":{\"uri\":\"%s\",%s\"diagnostics\":%s}}",
                          uri, version, items);
    CHECK(length > 0 && (size_t)length < sizeof response);
    DiagnosticPush(fixture, response);
}
static inline void DiagnosticFinish(DiagnosticFixture *fixture,
                                    UmiLanguageDiagnosticSession *session,
                                    UmiLanguageRuntimeServer *server)
{
    DiagnosticPush(fixture, "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":null}");
    CHECK(UmiLanguageDiagnosticSessionClose(session) == UMI_STATUS_OK);
    unsigned stops = fixture->stops;
    CHECK(UmiLanguageDiagnosticSessionClose(session) == UMI_STATUS_OK && fixture->stops == stops);
    CHECK(strstr(fixture->written, "textDocument/didClose") != NULL);
    CHECK(strstr(fixture->written, "\"method\":\"shutdown\"") != NULL);
    UmiLanguageDiagnosticSessionDestroy(session);
    CHECK(fixture->destroys == 0U);
    umi_language_runtime_server_destroy(server);
    CHECK(fixture->destroys == 1U);
}
#endif
