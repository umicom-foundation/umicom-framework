/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/language_lifecycle/main.c
 *
 * PURPOSE:
 *   Demonstrate validated language handshakes and unconditional cleanup through an installed SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/server_manager.h"
#include "umicom/language_runtime/memory_transport.h"
#include <stdio.h>
#include <string.h>

static int Expect(UmiStatus actual, UmiStatus expected, const char *step)
{
    if (actual == expected) return 1;
    fprintf(stderr, "%s: expected status %d, received %d\n", step, (int)expected, (int)actual);
    return 0;
}
static UmiStatus Supply(UmiLanguageRuntimeMemoryTransport *memory, const char *json)
{
    char frame[512]; size_t count = 0U;
    UmiStatus status = umi_language_runtime_frame_encode(json, frame, sizeof(frame), &count);
    return status == UMI_STATUS_OK ?
        umi_language_runtime_memory_transport_push_read(memory, frame, count) : status;
}
int main(int argc, char **argv)
{
    if (argc > 2) return 2;
    const char *mode = argc == 1 ? "valid" : argv[1];
    int invalid_initialize = strcmp(mode, "invalid-initialize") == 0;
    int invalid_shutdown = strcmp(mode, "invalid-shutdown") == 0;
    if (!invalid_initialize && !invalid_shutdown && strcmp(mode, "valid") != 0) return 2;
    UmiLanguageRuntimeMemoryTransport *memory = NULL;
    UmiLanguageRuntimeTransport transport = {0};
    UmiLanguageRuntimeServer *server = NULL;
    int result = 1;
    if (!Expect(umi_language_runtime_memory_transport_create(&memory, &transport),
        UMI_STATUS_OK, "create replay transport")) goto finished;
    UmiLanguageServerProfile profile = {0};
    (void)snprintf(profile.id, sizeof(profile.id), "%s", "learning-replay");
    (void)snprintf(profile.executable, sizeof(profile.executable), "%s", "not-launched");
    profile.enabled = 1;
    /* The connection owns the transport only after creation succeeds. The
     * memory pointer remains borrowed for supplying and inspecting bytes. */
    if (!Expect(umi_language_runtime_server_create_with_transport("learning-server",
        &profile, "file:///learning", &transport, &server), UMI_STATUS_OK, "create server"))
        goto finished;
    if (transport.instance != NULL) goto finished;
    const char *initialize = invalid_initialize ?
        "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":null}}" :
        "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{\"hoverProvider\":true}}}";
    if (!Expect(Supply(memory, initialize), UMI_STATUS_OK, "supply initialize reply")) goto finished;
    UmiLanguageRuntimeInitializeResult capabilities, before;
    memset(&before, 0x39, sizeof(before)); capabilities = before;
    UmiStatus expected = invalid_initialize ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    if (!Expect(UmiLanguageRuntimeServerInitialize(server, "file:///learning", 0U,
        NULL, &capabilities), expected, "initialize")) goto finished;
    UmiLanguageRuntimeServerSnapshot snapshot;
    if (!Expect(umi_language_runtime_server_snapshot(server, &snapshot), UMI_STATUS_OK,
        "inspect initialized state")) goto finished;
    if (invalid_initialize)
    {
        if (memcmp(&capabilities, &before, sizeof(before)) != 0 ||
            snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_FAILED) goto finished;
        puts("Initialization refused: the previous capabilities remain unchanged.");
        if (!Expect(umi_language_runtime_server_stop(server, 0U), UMI_STATUS_OK,
            "close refused connection")) goto finished;
    }
    else
    {
        if (!capabilities.hover || snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_READY) goto finished;
        puts("Initialization accepted: the connection is ready and advertises hover support.");
        const char *shutdown = invalid_shutdown ?
            "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":false}" :
            "{\"jsonrpc\":\"2.0\",\"id\":2,\"result\":null}";
        if (!Expect(Supply(memory, shutdown), UMI_STATUS_OK, "supply shutdown reply")) goto finished;
        expected = invalid_shutdown ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
        if (!Expect(UmiLanguageRuntimeServerShutdown(server, 0U), expected, "shutdown")) goto finished;
        puts(invalid_shutdown ? "Shutdown reply refused; transport cleanup still completed." :
            "Shutdown accepted; transport cleanup completed.");
    }
    if (!Expect(umi_language_runtime_server_snapshot(server, &snapshot), UMI_STATUS_OK,
        "inspect stopped state")) goto finished;
    if (snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_STOPPED ||
        umi_language_runtime_server_is_running(server)) goto finished;
    char written[4096]; size_t count = 0U;
    if (!Expect(umi_language_runtime_memory_transport_written(memory, written, sizeof(written),
        &count), UMI_STATUS_OK, "inspect sent messages")) goto finished;
    if (count == 0U ||
        ((strstr(written, "\"method\":\"initialized\"") != NULL) != !invalid_initialize) ||
        ((strstr(written, "\"method\":\"exit\"") != NULL) != (!invalid_initialize && !invalid_shutdown)))
        goto finished;
    result = 0;
finished:
    /* Do not free the borrowed memory pointer separately. A failed constructor
     * leaves the original transport here; a successful one moves it to server. */
    umi_language_runtime_server_destroy(server);
    if (transport.instance != NULL) transport.destroy(transport.instance);
    return result;
}
