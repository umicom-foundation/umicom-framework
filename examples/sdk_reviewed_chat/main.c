/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_reviewed_chat/main.c
 *
 * PURPOSE:
 *   Demonstrate approved streaming and separately reported model/save outcomes through an installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workspace/reviewed_stream.h"
#include "umicom/ai_workspace/providers.h"
#include "umicom/platform/threading.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile sig_atomic_t interrupted;
static void Interrupt(int number) { (void)number; interrupted = 1; }
static int Failure(const char *step, UmiStatus status)
{
    fprintf(stderr, "%s: %s\n", step, umi_status_text(status));
    return status == UMI_STATUS_CANCELLED ? 130 : 1;
}
int main(int argc, char **argv)
{
    if (argc != 7 || strcmp(argv[1], "--allow-network") != 0) {
        fputs("Umicom reviewed local chat\nUsage: umicom-sdk-reviewed-chat --allow-network DATABASE JOB PORT MODEL \"PROMPT\"\n"
            "Saves reviewed requests in SQLite and asks for RUN before contacting 127.0.0.1.\n", stderr);
        return 2;
    }
    char *end = NULL;
    errno = 0;
    unsigned long port = strtoul(argv[4], &end, 10);
    if (errno != 0 || end == argv[4] || *end != '\0' || port < 1024UL || port > 65535UL)
        return Failure("PORT must be 1024 through 65535", UMI_STATUS_INVALID_ARGUMENT);
    UmiAiLocalTransport *transport = NULL;
    UmiDataServer *data = NULL;
    UmiAiWorkspaceCancellation *cancellation = NULL;
    UmiAiWorkspace *workspace = NULL;
    UmiAiWorkspaceStream *stream = NULL;
    UmiAiRuntime runtime;
    umi_ai_runtime_init(&runtime);
    UmiAiProvider provider = {0};
    bool provider_owned = false;
    UmiStatus status = UmiAiLocalTransportCreate(&transport);
    const char *step = "Initialize HTTP before starting workers";
    if (status != UMI_STATUS_OK) goto cleanup;
    step = "Open local SQLite request records";
    status = umi_data_server_create_sqlite(argv[2], &data);
    if (status != UMI_STATUS_OK) goto cleanup;
    step = "Create cancellation token";
    status = UmiAiWorkspaceCancellationCreate(&cancellation);
    if (status != UMI_STATUS_OK) goto cleanup;
    step = "Create canonical local provider";
    status = UmiAiWorkspaceLocalProviderCreate((uint16_t)port, 60000U, cancellation, &provider);
    if (status != UMI_STATUS_OK) goto cleanup;
    provider_owned = true;
    step = "Register local provider";
    status = umi_ai_provider_registry_add(&runtime.providers, &provider);
    if (status != UMI_STATUS_OK) goto cleanup;
    provider_owned = false; /* Registry owns the adapter from this point. */
    step = "Open reviewed workspace";
    status = UmiAiWorkspaceCreate(data, &runtime, "sdk.reviewed.chat", &workspace);
    if (status != UMI_STATUS_OK) goto cleanup;
    step = "Prepare a new immutable job";
    status = UmiAiWorkspacePrepare(workspace, argv[3], UMI_AI_WORKSPACE_DRAFT,
        "umicom.local-chat", argv[5], NULL, argv[6], "console-user", 512U);
    if (status != UMI_STATUS_OK) goto cleanup;

    printf("Job: %s\nModel: %s\nEndpoint: http://127.0.0.1:%lu/v1/chat/completions\nPrompt: %s\n",
        argv[3], argv[5], port, argv[6]);
    fputs("Type RUN to approve this request. Any other answer leaves it unapproved: ", stdout);
    if (fflush(stdout) != 0) { status = UMI_STATUS_IO_ERROR; step = "Show review"; goto cleanup; }
    char answer[16];
    if (fgets(answer, sizeof(answer), stdin) == NULL ||
        (strcmp(answer, "RUN\n") != 0 && strcmp(answer, "RUN\r\n") != 0)) {
        fputs("Request was not approved; no model request started.\n", stderr);
        status = UMI_STATUS_CANCELLED; step = "Review"; goto cleanup;
    }
    step = "Persist approval";
    status = UmiAiWorkspaceReview(workspace, argv[3], "console-user", true);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (signal(SIGINT, Interrupt) == SIG_ERR) {
        status = UMI_STATUS_UNAVAILABLE; step = "Install Ctrl+C handling"; goto cleanup;
    }
    UmiAiWorkspaceStreamOptions options = {.maximum_wire_bytes=16U * 1024U * 1024U, .allow_network=true};
    step = "Retain approved stream";
    status = UmiAiWorkspaceStreamCreate(workspace, argv[3], transport, &options, &stream);
    if (status != UMI_STATUS_OK) goto cleanup;
    step = "Publish RUNNING and start request";
    status = UmiAiWorkspaceStreamStart(stream);
    size_t printed = 0U;
    while (status == UMI_STATUS_OK) {
        if (interrupted) status = UmiAiWorkspaceStreamCancel(stream);
        if (status != UMI_STATUS_OK) break;
        UmiAiWorkspaceStreamSnapshot observation;
        status = UmiAiWorkspaceStreamPoll(stream, &observation);
        if (status != UMI_STATUS_OK) break;
        char *text = NULL;
        UmiAiResponseStreamSnapshot snapshot;
        status = UmiAiWorkspaceStreamCopyText(stream, &text, &snapshot);
        if (status != UMI_STATUS_OK) break;
        if (snapshot.text_bytes < printed) status = UMI_STATUS_INVALID_STATE;
        else if (snapshot.text_bytes > printed) {
            size_t amount = snapshot.text_bytes - printed;
            if (fwrite(text + printed, 1U, amount, stdout) != amount || fflush(stdout) != 0)
                status = UMI_STATUS_IO_ERROR;
            else printed = snapshot.text_bytes;
        }
        UmiAiResponseStreamTextFree(text);
        if (status != UMI_STATUS_OK || observation.transport.state == UMI_AI_LOCAL_STREAM_FINISHED) break;
        umi_thread_sleep_ms(25U);
    }
    if (status != UMI_STATUS_OK || interrupted) (void)UmiAiWorkspaceStreamCancel(stream);
    /* Start may fail either before or after durable RUNNING. Only a running
     * controller needs joining; prepared and settled owners remain inspectable. */
    UmiAiWorkspaceStreamSnapshot final_snapshot;
    UmiStatus observed = UmiAiWorkspaceStreamPoll(stream, &final_snapshot);
    if (observed != UMI_STATUS_OK)
        return Failure("Observe retained owner; storage retained until process exit", observed);
    if (final_snapshot.phase == UMI_AI_WORKSPACE_STREAM_RUNNING) {
        observed = UmiAiWorkspaceStreamFinish(stream);
        if (observed != UMI_STATUS_OK)
            return Failure("Join/settle request; storage retained until process exit", observed);
        final_snapshot.phase = UMI_AI_WORKSPACE_STREAM_SETTLED;
    }
    if (final_snapshot.phase == UMI_AI_WORKSPACE_STREAM_SETTLED) {
        UmiAiWorkspaceStreamResult result;
        observed = UmiAiWorkspaceStreamReadResult(stream, &result);
        if (observed != UMI_STATUS_OK)
            return Failure("Read retained outcome; storage retained until process exit", observed);
        fprintf(stderr, "\nModel result: %s\nSave result: %s\n",
            umi_status_text(result.operation_status), umi_status_text(result.publication_status));
        if (status == UMI_STATUS_OK) status = result.operation_status;
        if (result.publication_status != UMI_STATUS_OK) {
            fputs("Do not repeat automatically. Reopen and inspect the interrupted request record.\n", stderr);
            status = result.publication_status;
        }
        step = "Reviewed generation";
    }
cleanup:
    /* Release in ownership order. If a checked owner refuses release, stop
     * here and keep all borrowed storage alive until process exit. */
    {
        UmiStatus release = UmiAiWorkspaceStreamDestroy(&stream);
        if (release != UMI_STATUS_OK) return Failure("Release stream; ownership retained", release);
        release = UmiAiWorkspaceRelease(&workspace);
        if (release != UMI_STATUS_OK) return Failure("Release workspace; ownership retained", release);
        release = UmiAiRuntimeRelease(&runtime);
        if (release != UMI_STATUS_OK) return Failure("Release runtime; ownership retained", release);
        if (provider_owned) provider.destroy(provider.instance);
        UmiAiWorkspaceCancellationDestroy(cancellation);
        if (data != NULL) umi_data_server_destroy(data);
        release = UmiAiLocalTransportDestroy(&transport);
        if (status == UMI_STATUS_OK && release != UMI_STATUS_OK) {
            status = release; step = "Release HTTP lifetime";
        }
    }
    return status == UMI_STATUS_OK ? 0 : Failure(step, status);
}
