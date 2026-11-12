/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/cli.c
 *
 * PURPOSE:
 *   Present the same reviewed model workflow in the standalone application and the Umicom CLI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void PrintText(const char *text)
{
    /* Server output and model responses are data, not terminal commands. */
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c == '\n' || c == '\t' || (c >= 32U && c != 127U)) (void)putchar((int)c);
        else (void)printf("\\x%02X", (unsigned)c);
    }
    (void)putchar('\n');
}
static int Failure(UmiStatus status)
{
    (void)fprintf(stderr, "Umicom Models: %s (%d).\n", umi_status_text(status), (int)status);
    return status == UMI_STATUS_INVALID_ARGUMENT ? 2 : 1;
}
static void Help(void)
{
    (void)puts("Umicom Local Models - native CPU text-model manager\n"
        "Use: umicom models COMMAND ...  or  umicom-models COMMAND ...\n"
        "  catalogue\n"
        "  inspect ABSOLUTE_GGUF [SHA256]\n"
        "  download ID NEW_ABSOLUTE_GGUF [--apply]\n"
        "  download-file REPOSITORY COMMIT FILE SHA256 BYTES NEW_ABSOLUTE_GGUF [--apply]\n"
        "  run ID ABSOLUTE_LLAMA_SERVER ABSOLUTE_GGUF PORT CONTEXT THREADS [--apply]\n"
        "  serve ABSOLUTE_LLAMA_SERVER ABSOLUTE_GGUF SHA256 ALIAS PORT CONTEXT THREADS [--apply]\n"
        "  chat PORT ALIAS PROMPT [--send-reviewed-prompt]\n"
        "Download and run preview by default. Directories must already exist.\n"
        "Only public pinned GGUF downloads; no Python, Conda or hidden installer.\n"
        "Serve owns a child process tree. Type status or stop in its console.\n"
        "Submission/running is not readiness. Test chat separately.\n"
        "CPU text inference uses your separately installed trusted llama-server.\n"
        "Image/video/embedding models, cloud fallback and agent tools are not enabled.");
    (void)printf("Compiled downloads: %s; local chat: %s\n",
        UmiModelDownloadAvailable() ? "available" : "unavailable",
        UmiAiWorkspaceLocalProviderAvailable() ? "available" : "unavailable");
}
static void Inspection(const UmiModelInspection *value)
{
    (void)printf("Bytes: %" PRIu64 "\nSHA256: %s\nGGUF: v%u; tensors: %" PRIu64 "; metadata: %" PRIu64 "\n",
        value->bytes, value->sha256, (unsigned)value->gguf_version, value->tensors, value->metadata_entries);
    (void)puts("File identity and basic header checked. Engine compatibility is still unverified.");
}
static bool ApplyArgument(int argc, char **argv, int required, bool *apply)
{
    *apply = argc == required + 1 && strcmp(argv[required], "--apply") == 0;
    return argc == required || *apply;
}
static int Download(const UmiModelSource *source, const char *destination, bool apply)
{
    UmiModelDownloadPlan plan;
    UmiStatus status = UmiModelDownloadPlanCreate(source, destination, &plan);
    if (status != UMI_STATUS_OK) return Failure(status);
    (void)printf("Source: %s\nDestination: %s\nExpected bytes: %" PRIu64 "\nSHA256: %s\n",
        plan.url, plan.destination, plan.source.bytes, plan.source.sha256);
    if (!apply) { (void)puts("Preview only. Add --apply after reviewing the model licence and destination."); return 0; }
    (void)puts("Downloading and verifying; rejected/partial files are retained and never overwritten.");
    (void)fflush(stdout);
    UmiModelInspection inspected;
    status = UmiModelDownload(&plan, true, NULL, &inspected);
    if (status != UMI_STATUS_OK) return Failure(status);
    Inspection(&inspected);
    return 0;
}
static int Serve(const UmiModelRunPlan *plan, bool apply)
{
    const char *arguments[UMI_MODEL_RUN_ARGUMENTS];
    size_t count = 0U;
    UmiStatus status = UmiModelRunArguments(plan, arguments, UMI_MODEL_RUN_ARGUMENTS, &count);
    if (status != UMI_STATUS_OK) return Failure(status);
    (void)printf("Executable: %s\nWorking directory: %s\nGuard: %s\nSHA256: %s\nArguments (separate values):\n",
        plan->executable, plan->directory, plan->guard, plan->sha256);
    for (size_t i = 0U; i < count; ++i) (void)printf("  [%zu] %s\n", i, arguments[i]);
    if (!apply) { (void)puts("Preview only; no file inspection or process launch. Add --apply to start."); return 0; }
    UmiModelSession *session = NULL;
    status = UmiModelSessionStart(plan, true, &session);
    if (status != UMI_STATUS_OK) {
        if (session != NULL) {
            UmiStatus stop = UmiModelSessionStop(&session);
            if (stop != UMI_STATUS_OK) (void)fprintf(stderr, "Cleanup incomplete: %s.\n", umi_status_text(stop));
        }
        return Failure(status);
    }
    (void)printf("Submitted. Endpoint: http://127.0.0.1:%s/v1/chat/completions\nModel alias: %s\n",
        plan->port, plan->alias);
    (void)puts("Keep this console open. Type status to read diagnostics; stop (or stdin EOF) ends the owned process.\n"
        "Wait for the engine's load result and verify a reviewed chat request in another console.");
    char line[64];
    for (;;) {
        (void)fputs("models> ", stdout); (void)fflush(stdout);
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        size_t length = strcspn(line, "\r\n");
        if (length == sizeof(line) - 1U) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            (void)puts("Use status or stop."); continue;
        }
        line[length] = '\0';
        if (strcmp(line, "stop") == 0) break;
        if (strcmp(line, "status") != 0) { (void)puts("Use status or stop."); continue; }
        UmiProcessJobSnapshot snapshot;
        status = UmiModelSessionRead(session, &snapshot);
        if (status != UMI_STATUS_OK) { (void)Failure(status); continue; }
        (void)printf("Process: %s; exit: %d; output truncated: %s\n",
            umi_process_job_state_text(snapshot.state), snapshot.exit_code, snapshot.output_truncated ? "yes" : "no");
        PrintText(snapshot.output);
    }
    bool inputError = ferror(stdin) != 0;
    UmiProcessJobSnapshot finalSnapshot;
    UmiStatus observed = UmiModelSessionRead(session, &finalSnapshot);
    bool failed = observed == UMI_STATUS_OK &&
        (finalSnapshot.state == UMI_PROCESS_JOB_FAILED || finalSnapshot.state == UMI_PROCESS_JOB_TIMED_OUT);
    if (failed) PrintText(finalSnapshot.output);
    UmiStatus stop = UmiModelSessionStop(&session);
    if (stop != UMI_STATUS_OK) return Failure(stop);
    (void)puts("Owned model session stopped. The model and guard file were retained.");
    if (inputError || failed) return Failure(UMI_STATUS_IO_ERROR);
    return observed == UMI_STATUS_OK ? 0 : Failure(observed);
}
static int Chat(const char *portText, const char *alias, const char *prompt, bool approved)
{
    uint64_t port;
    if (!UmiModelNumber(portText, 1024U, 65535U, &port) || !UmiModelNameValid(alias, 128U, false) ||
        !UmiModelTextFits(prompt, UMI_AI_TEXT_CAPACITY) || prompt[0] == '\0') return Failure(UMI_STATUS_INVALID_ARGUMENT);
    (void)printf("Loopback port: %u; model: %s\nReviewed prompt:\n", (unsigned)port, alias);
    PrintText(prompt);
    if (!approved) { (void)puts("Preview only. Add --send-reviewed-prompt to send this text."); return 0; }
    UmiAiRequest *request = calloc(1U, sizeof(*request));
    if (request == NULL) return Failure(UMI_STATUS_OUT_OF_MEMORY);
    UmiAiRuntime runtime;
    UmiAiProvider provider = {0};
    UmiAiMessage message;
    UmiAiResponse response;
    umi_ai_runtime_init(&runtime);
    umi_ai_request_init(request);
    umi_ai_response_init(&response);
    (void)snprintf(request->request_id, sizeof(request->request_id), "%s", "umicom-models-chat");
    (void)snprintf(request->model_id, sizeof(request->model_id), "%s", alias);
    request->max_output_tokens = 256U;
    request->temperature = 0.2;
    request->allow_tools = 0;
    UmiStatus status = umi_ai_message_set(&message, UMI_AI_ROLE_USER, "user", prompt);
    if (status == UMI_STATUS_OK) status = umi_ai_request_add_message(request, &message);
    if (status == UMI_STATUS_OK) status = UmiModelLocalProviderCreate((uint16_t)port, 60000U, NULL, &provider);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_provider_registry_add(&runtime.providers, &provider);
        if (status != UMI_STATUS_OK) provider.destroy(provider.instance);
    }
    if (status == UMI_STATUS_OK) status = umi_ai_runtime_generate(&runtime, "umicom.local-chat", 1, request, &response);
    if (status == UMI_STATUS_OK) {
        PrintText(response.text);
        (void)printf("Finish reason: %s\n", umi_ai_finish_reason_text(response.finish_reason));
    }
    /* No chat history, transcript files or automatic retries: a failed turn
     * cannot be mistaken for a committed successful response by this client. */
    umi_ai_runtime_destroy(&runtime);
    free(request);
    return status == UMI_STATUS_OK ? 0 : Failure(status);
}
int UmiModelsCommand(int argc, char **argv)
{
    if (argc < 0 || argc > 9 || (argc != 0 && argv == NULL)) return Failure(UMI_STATUS_INVALID_ARGUMENT);
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL) return Failure(UMI_STATUS_INVALID_ARGUMENT);
    if (argc == 0 || (argc == 1 && (strcmp(argv[0], "help") == 0 || strcmp(argv[0], "--help") == 0))) { Help(); return 0; }
    if (strcmp(argv[0], "catalogue") == 0 && argc == 1) {
        for (size_t i = 0U; i < UmiModelCatalogueCount(); ++i) {
            const UmiModelCatalogueEntry *entry = UmiModelCatalogueAt(i);
            (void)printf("%s | %s | %s | %" PRIu64 " bytes\n  %s\n",
                entry->id, entry->title, entry->licence, entry->source.bytes, entry->source.sha256);
        }
        return 0;
    }
    if (strcmp(argv[0], "inspect") == 0 && (argc == 2 || argc == 3)) {
        UmiModelInspection inspected;
        UmiStatus status = UmiModelInspect(argv[1], argc == 3 ? argv[2] : NULL, NULL, &inspected);
        if (status != UMI_STATUS_OK) return Failure(status);
        Inspection(&inspected); return 0;
    }
    bool apply = false;
    if (strcmp(argv[0], "download") == 0 && ApplyArgument(argc, argv, 3, &apply)) {
        const UmiModelCatalogueEntry *entry = UmiModelCatalogueFind(argv[1]);
        if (entry == NULL) return Failure(UMI_STATUS_NOT_FOUND);
        (void)printf("Model licence: %s (review the publisher's full terms).\n", entry->licence);
        return Download(&entry->source, argv[2], apply);
    }
    if (strcmp(argv[0], "download-file") == 0 && ApplyArgument(argc, argv, 7, &apply)) {
        UmiModelSource source = {0};
        if (!UmiModelTextFits(argv[1], sizeof(source.repository)) || !UmiModelTextFits(argv[2], sizeof(source.revision)) ||
            !UmiModelTextFits(argv[3], sizeof(source.filename)) || !UmiModelTextFits(argv[4], sizeof(source.sha256)) ||
            !UmiModelNumber(argv[5], 24U, UMI_MODEL_MAX_BYTES, &source.bytes)) return Failure(UMI_STATUS_INVALID_ARGUMENT);
        memcpy(source.repository, argv[1], strlen(argv[1]) + 1U);
        memcpy(source.revision, argv[2], strlen(argv[2]) + 1U);
        memcpy(source.filename, argv[3], strlen(argv[3]) + 1U);
        memcpy(source.sha256, argv[4], strlen(argv[4]) + 1U);
        return Download(&source, argv[6], apply);
    }
    if ((strcmp(argv[0], "run") == 0 && ApplyArgument(argc, argv, 7, &apply)) ||
        (strcmp(argv[0], "serve") == 0 && ApplyArgument(argc, argv, 8, &apply))) {
        bool catalogue = strcmp(argv[0], "run") == 0;
        const UmiModelCatalogueEntry *entry = catalogue ? UmiModelCatalogueFind(argv[1]) : NULL;
        if (catalogue && entry == NULL) return Failure(UMI_STATUS_NOT_FOUND);
        int offset = catalogue ? 4 : 5;
        uint64_t port, context, threads;
        if (!UmiModelNumber(argv[offset], 1024U, 65535U, &port) ||
            !UmiModelNumber(argv[offset + 1], 512U, 32768U, &context) ||
            !UmiModelNumber(argv[offset + 2], 1U, 256U, &threads)) return Failure(UMI_STATUS_INVALID_ARGUMENT);
        UmiModelRunPlan plan;
        UmiStatus status = UmiModelRunPlanCreate(argv[catalogue ? 2 : 1], argv[catalogue ? 3 : 2],
            catalogue ? entry->source.sha256 : argv[3], catalogue ? entry->id : argv[4],
            (uint16_t)port, (uint32_t)context, (uint32_t)threads, &plan);
        return status == UMI_STATUS_OK ? Serve(&plan, apply) : Failure(status);
    }
    if (strcmp(argv[0], "chat") == 0 && (argc == 4 ||
        (argc == 5 && strcmp(argv[4], "--send-reviewed-prompt") == 0)))
        return Chat(argv[1], argv[2], argv[3], argc == 5);
    Help();
    return Failure(UMI_STATUS_INVALID_ARGUMENT);
}
