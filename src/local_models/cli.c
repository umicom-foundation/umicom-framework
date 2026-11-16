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
#include "umicom/local_models/runtime.h"
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
#include "umicom/model_formats/safetensors_file.h"
#endif
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
/* A review is incomplete when its output cannot be delivered. Both console
 * hosts share this gate before network or process side effects. */
static bool ReviewOutputComplete(void)
{
    return fflush(stdout) == 0 && !ferror(stdout);
}

static void Help(void)
{
    (void)puts("Umicom Local Models - native CPU text-model manager\n"
        "Use: umicom models COMMAND ...  or  umicom-models COMMAND ...\n"
        "  catalogue\n"
        "  inspect ABSOLUTE_GGUF [SHA256]\n"
        "  inspect-safetensors ABSOLUTE_FILE\n"
        "  download ID NEW_ABSOLUTE_GGUF [--apply]\n"
        "  download-file REPOSITORY COMMIT FILE SHA256 BYTES NEW_ABSOLUTE_GGUF [--apply]\n"
        "  run ID ABSOLUTE_LLAMA_SERVER ABSOLUTE_GGUF PORT CONTEXT THREADS [--apply]\n"
        "  serve ABSOLUTE_LLAMA_SERVER ABSOLUTE_GGUF SHA256 ALIAS PORT CONTEXT THREADS [--apply]\n"
        "  describe ID ABSOLUTE_LLAMA_SERVER ABSOLUTE_GGUF PORT CONTEXT THREADS\n"
        "  describe-file ABSOLUTE_LLAMA_SERVER ABSOLUTE_GGUF SHA256 ALIAS PORT CONTEXT THREADS\n"
        "  chat PORT ALIAS PROMPT [--send-reviewed-prompt]\n"
        "  health PORT TIMEOUT_MS [--probe]\n"
        "  wait PORT ATTEMPTS INTERVAL_MS TIMEOUT_MS [--probe]\n"
        "Describe prints unavailable runtime metadata only; no --apply form exists.\n"
        "Download and run preview by default. Directories must already exist.\n"
        "Only public pinned GGUF downloads; no Python, Conda or hidden installer.\n"
        "Serve owns a child process tree. Type status or stop in its console.\n"
        "Submission/running is not readiness. Test chat separately.\n"
        "CPU text inference uses your separately installed trusted llama-server.\n"
        "Image/video/embedding models, cloud fallback and agent tools are not enabled.");
    (void)printf("Compiled downloads: %s; local chat: %s\n",
        UmiModelDownloadAvailable() ? "available" : "unavailable",
        UmiAiWorkspaceLocalProviderAvailable() ? "available" : "unavailable");
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
    (void)puts("Safetensors file inspection: available (metadata and digest only).");
#else
    (void)puts("Safetensors file inspection: unavailable in this build.");
#endif
}
static void Inspection(const UmiModelInspection *value)
{
    (void)printf("Bytes: %" PRIu64 "\nSHA256: %s\nGGUF: v%u; tensors: %" PRIu64 "; metadata: %" PRIu64 "\n",
        value->bytes, value->sha256, (unsigned)value->gguf_version, value->tensors, value->metadata_entries);
    (void)puts("File identity and basic header checked. Engine compatibility is still unverified.");
}

/* Both console hosts delegate model parsing and file ownership to Framework.
 * Report observed identity only; successful inspection does not admit a model
 * to an inference engine or verify the publisher of its bytes. */
static int InspectSafetensors(const char *path)
{
#ifdef UMICOM_MODELS_HAS_TENSOR_INSPECTION
    UmiSafetensorsFileInspection inspected;
    UmiStatus status=UmiSafetensorsFileInspect(path,NULL,NULL,&inspected);
    if(status!=UMI_STATUS_OK) return Failure(status);
    char digest[UMI_SHA256_HEX_CAPACITY];
    UmiSha256Hex(inspected.file.sha256,digest);
    (void)printf("Bytes: %" PRIu64 "\nSHA256: %s\nSafetensors: tensors: %zu; metadata: %zu; elements: %" PRIu64 "\n",
        inspected.file.bytes,digest,inspected.layout.tensors,
        inspected.layout.metadata_entries,inspected.layout.elements);
    (void)printf("Header bytes: %" PRIu64 "; tensor data bytes: %" PRIu64 "; maximum rank: %zu\n",
        inspected.layout.header_bytes,inspected.layout.data_bytes,inspected.layout.maximum_rank);
    (void)puts("Observed file identity and layout only. Publisher, numerical values and runtime compatibility remain unverified.");
    return ReviewOutputComplete()?0:Failure(UMI_STATUS_IO_ERROR);
#else
    (void)path;
    return Failure(UMI_STATUS_NOT_IMPLEMENTED);
#endif
}

static bool ApplyArgument(int argc, char **argv, int required, bool *apply)
{
    *apply = argc == required + 1 && strcmp(argv[required], "--apply") == 0;
    return argc == required || *apply;
}
/* Download now refuses failed review output before network/file creation and reports final output failures. The former implementation is retained for owner review. */
#if 0
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
#endif
static int Download(const UmiModelSource *source, const char *destination, bool apply)
{
    UmiModelDownloadPlan plan;
    UmiStatus status = UmiModelDownloadPlanCreate(source, destination, &plan);
    if (status != UMI_STATUS_OK) return Failure(status);
    (void)printf("Source: %s\nDestination: %s\nExpected bytes: %" PRIu64 "\nSHA256: %s\n",
        plan.url, plan.destination, plan.source.bytes, plan.source.sha256);
    if (!apply) { (void)puts("Preview only. Add --apply after reviewing the model licence and destination."); return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR); }
    if (!ReviewOutputComplete()) return Failure(UMI_STATUS_IO_ERROR);
    (void)puts("Downloading and verifying; rejected/partial files are retained and never overwritten.");
    if (!ReviewOutputComplete()) return Failure(UMI_STATUS_IO_ERROR);
    UmiModelInspection inspected;
    status = UmiModelDownload(&plan, true, NULL, &inspected);
    if (status != UMI_STATUS_OK) return Failure(status);
    Inspection(&inspected);
    return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR);
}
static int Health(const char *portText, const char *timeoutText, bool approved)
{
    uint64_t port, timeout;
    if (!UmiModelNumber(portText, 1024U, 65535U, &port) ||
        !UmiModelNumber(timeoutText, 100U, 60000U, &timeout))
        return Failure(UMI_STATUS_INVALID_ARGUMENT);
    (void)printf("GET http://127.0.0.1:%u/health; timeout: %u ms\n", (unsigned)port, (unsigned)timeout);
    (void)puts("Endpoint report only. Process ownership, model identity and inference remain unverified.");
    if (!approved) {
        (void)puts("Preview only. Add --probe to make one loopback request; no prompt is sent.");
        return fflush(stdout) == 0 && !ferror(stdout) ? 0 : Failure(UMI_STATUS_IO_ERROR);
    }
    /* A failed preview must not silently precede a network action. */
    if (fflush(stdout) != 0 || ferror(stdout)) return Failure(UMI_STATUS_IO_ERROR);
    UmiAiLocalHealth observed = {0};
    UmiStatus status = UmiModelHealthProbe((uint16_t)port, (uint32_t)timeout, true, NULL, &observed);
    if (status != UMI_STATUS_OK) return Failure(status);
    (void)printf("HTTP %u; endpoint reports %s.\n", (unsigned)observed.http_status,
        observed.endpoint_reports_ready ? "ready" : "unavailable");
    if (fflush(stdout) != 0 || ferror(stdout)) return Failure(UMI_STATUS_IO_ERROR);
    return observed.endpoint_reports_ready ? 0 : 1;
}

static int Readiness(const char *portText, const char *attemptsText,
    const char *intervalText, const char *timeoutText, bool approved)
{
    uint64_t port, attempts, interval, timeout;
    if (!UmiModelNumber(portText, 1024U, 65535U, &port) ||
        !UmiModelNumber(attemptsText, 1U, 64U, &attempts) ||
        !UmiModelNumber(intervalText, 50U, 5000U, &interval) ||
        !UmiModelNumber(timeoutText, 100U, 60000U, &timeout))
        return Failure(UMI_STATUS_INVALID_ARGUMENT);
    UmiModelReadinessPlan plan;
    UmiStatus status = UmiModelReadinessPlanCreate((uint16_t)port, (uint32_t)attempts,
        (uint32_t)interval, (uint32_t)timeout, &plan);
    if (status != UMI_STATUS_OK) return Failure(status);
    uint32_t budget = 0U;
    status = UmiModelReadinessBudget(&plan, &budget);
    if (status != UMI_STATUS_OK) return Failure(status);
    (void)printf("GET http://127.0.0.1:%u/health; at most %u probes; timeout %u ms each; pause %u ms\n",
        (unsigned)port, (unsigned)attempts, (unsigned)timeout, (unsigned)interval);
    (void)printf("Nominal budget: %u ms plus scheduling/cleanup; no model identity or inference proof.\n",
        (unsigned)budget);
    if (!approved) {
        (void)puts("Preview only. Add --probe to wait explicitly; no prompt, model launch or stop is performed.");
        return fflush(stdout) == 0 && !ferror(stdout) ? 0 : Failure(UMI_STATUS_IO_ERROR);
    }
    /* Both console hosts use the shared wait and its admission contract.
     * Refuse side effects if the review text could not be delivered. */
    if (fflush(stdout) != 0 || ferror(stdout)) return Failure(UMI_STATUS_IO_ERROR);
    UmiModelReadinessReport report = {0};
    status = UmiModelReadinessWait(&plan, true, NULL, &report);
    (void)printf("Probes completed: %u; last probe: %s\n", (unsigned)report.attempts_completed,
        report.attempts_completed ? umi_status_text(report.last_probe_status) : "none");
    if (report.has_health)
        (void)printf("Last endpoint report: HTTP %u; %s.\n", (unsigned)report.last_health.http_status,
            report.last_health.endpoint_reports_ready ? "ready" : "unavailable");
    if (fflush(stdout) != 0 || ferror(stdout)) return Failure(UMI_STATUS_IO_ERROR);
    return status == UMI_STATUS_OK ? 0 : Failure(status);
}

/* Process launch requires a completed review; later output failure leaves the console loop through checked session cleanup. The former implementation is retained for owner review. */
#if 0
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
#endif
/* The console now keeps failed start/stop cleanup in a caller-owned session slot instead of losing a local pointer on return. The former implementation is retained for owner review. */
#if 0
static int Serve(const UmiModelRunPlan *plan, bool apply)
{
    const char *arguments[UMI_MODEL_RUN_ARGUMENTS];
    size_t count = 0U;
    UmiStatus status = UmiModelRunArguments(plan, arguments, UMI_MODEL_RUN_ARGUMENTS, &count);
    if (status != UMI_STATUS_OK) return Failure(status);
    (void)printf("Executable: %s\nWorking directory: %s\nGuard: %s\nSHA256: %s\nArguments (separate values):\n",
        plan->executable, plan->directory, plan->guard, plan->sha256);
    for (size_t i = 0U; i < count; ++i) (void)printf("  [%zu] %s\n", i, arguments[i]);
    if (!apply) { (void)puts("Preview only; no file inspection or process launch. Add --apply to start."); return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR); }
    if (!ReviewOutputComplete()) return Failure(UMI_STATUS_IO_ERROR);
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
    bool outputError = !ReviewOutputComplete();
    char line[64];
    for (; !outputError;) {
        (void)fputs("models> ", stdout);
        if (!ReviewOutputComplete()) { outputError = true; break; }
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
    if (!ReviewOutputComplete()) outputError = true;
    if (inputError || outputError || failed) return Failure(UMI_STATUS_IO_ERROR);
    return observed == UMI_STATUS_OK ? 0 : Failure(observed);
}
#endif
static int Serve(const UmiModelRunPlan *plan, bool apply, UmiModelSession **inOutSession)
{
    const char *arguments[UMI_MODEL_RUN_ARGUMENTS];
    size_t count = 0U;
    UmiStatus status = UmiModelRunArguments(plan, arguments, UMI_MODEL_RUN_ARGUMENTS, &count);
    if (status != UMI_STATUS_OK) return Failure(status);
    (void)printf("Executable: %s\nWorking directory: %s\nGuard: %s\nSHA256: %s\nArguments (separate values):\n",
        plan->executable, plan->directory, plan->guard, plan->sha256);
    for (size_t i = 0U; i < count; ++i) (void)printf("  [%zu] %s\n", i, arguments[i]);
    if (!apply) { (void)puts("Preview only; no file inspection or process launch. Add --apply to start."); return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR); }
    if (!ReviewOutputComplete()) return Failure(UMI_STATUS_IO_ERROR);
    status = UmiModelSessionStart(plan, true, inOutSession);
    if (status != UMI_STATUS_OK) {
        if (*inOutSession != NULL) {
            UmiStatus stop = UmiModelSessionStop(inOutSession);
            if (stop != UMI_STATUS_OK) (void)fprintf(stderr, "Cleanup incomplete: %s.\n", umi_status_text(stop));
        }
        return Failure(status);
    }
    (void)printf("Submitted. Endpoint: http://127.0.0.1:%s/v1/chat/completions\nModel alias: %s\n",
        plan->port, plan->alias);
    (void)puts("Keep this console open. Type status to read diagnostics; stop (or stdin EOF) ends the owned process.\n"
        "Wait for the engine's load result and verify a reviewed chat request in another console.");
    bool outputError = !ReviewOutputComplete();
    char line[64];
    for (; !outputError;) {
        (void)fputs("models> ", stdout);
        if (!ReviewOutputComplete()) { outputError = true; break; }
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
        status = UmiModelSessionRead(*inOutSession, &snapshot);
        if (status != UMI_STATUS_OK) { (void)Failure(status); continue; }
        (void)printf("Process: %s; exit: %d; output truncated: %s\n",
            umi_process_job_state_text(snapshot.state), snapshot.exit_code, snapshot.output_truncated ? "yes" : "no");
        PrintText(snapshot.output);
    }
    bool inputError = ferror(stdin) != 0;
    UmiProcessJobSnapshot finalSnapshot;
    UmiStatus observed = UmiModelSessionRead(*inOutSession, &finalSnapshot);
    bool failed = observed == UMI_STATUS_OK &&
        (finalSnapshot.state == UMI_PROCESS_JOB_FAILED || finalSnapshot.state == UMI_PROCESS_JOB_TIMED_OUT);
    if (failed) PrintText(finalSnapshot.output);
    UmiStatus stop = UmiModelSessionStop(inOutSession);
    if (stop != UMI_STATUS_OK) return Failure(stop);
    (void)puts("Owned model session stopped. The model and guard file were retained.");
    if (!ReviewOutputComplete()) outputError = true;
    if (inputError || outputError || failed) return Failure(UMI_STATUS_IO_ERROR);
    return observed == UMI_STATUS_OK ? 0 : Failure(observed);
}
/* Prompt sending requires delivered review output; final output errors are reported after runtime cleanup. The former implementation is retained for owner review. */
#if 0
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
#endif
static int Chat(const char *portText, const char *alias, const char *prompt, bool approved)
{
    uint64_t port;
    if (!UmiModelNumber(portText, 1024U, 65535U, &port) || !UmiModelNameValid(alias, 128U, false) ||
        !UmiModelTextFits(prompt, UMI_AI_TEXT_CAPACITY) || prompt[0] == '\0') return Failure(UMI_STATUS_INVALID_ARGUMENT);
    (void)printf("Loopback port: %u; model: %s\nReviewed prompt:\n", (unsigned)port, alias);
    PrintText(prompt);
    if (!approved) { (void)puts("Preview only. Add --send-reviewed-prompt to send this text."); return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR); }
    if (!ReviewOutputComplete()) return Failure(UMI_STATUS_IO_ERROR);
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
    if (!ReviewOutputComplete() && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    return status == UMI_STATUS_OK ? 0 : Failure(status);
}
/* The CLI prints the shared descriptor; applications can store the same value
 * with conditional catalogue admission. Neither operation starts a runtime. */
static int Describe(const UmiModelRunPlan *plan, const char *label)
{
    UmiAiRuntimeDescriptor descriptor;
    UmiStatus status = UmiModelRuntimeDescriptorCreate(plan, plan->alias, label, 256U, &descriptor);
    if (status != UMI_STATUS_OK) return Failure(status);
    (void)printf("Runtime: %s\nProvider: %s\nModel alias: %s\nEndpoint: %s\n",
        descriptor.runtime_id, descriptor.provider_id, descriptor.model_id, descriptor.endpoint);
    (void)fputs("Display name: ", stdout); PrintText(descriptor.display_name);
    (void)printf("Context tokens: %u; planned output limit: %u\nExpected model SHA256: %s\n",
        (unsigned)descriptor.context_tokens, (unsigned)descriptor.max_output_tokens, plan->sha256);
    (void)puts("Configured: yes; health: unavailable; revision: unassigned.\n"
        "Description only: no file inspection, catalogue write, provider registration, network or launch.\n"
        "Capabilities describe the planned chat/stream adapter; model support remains unverified.");
    return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR);
}

/* Add pure runtime description commands and propagate console-output failures while preserving the established run/serve interfaces. The former implementation is retained for owner review. */
#if 0
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
    if (strcmp(argv[0], "health") == 0 && (argc == 3 ||
        (argc == 4 && strcmp(argv[3], "--probe") == 0)))
        return Health(argv[1], argv[2], argc == 4);
    if (strcmp(argv[0], "wait") == 0 && (argc == 5 ||
        (argc == 6 && strcmp(argv[5], "--probe") == 0)))
        return Readiness(argv[1], argv[2], argv[3], argv[4], argc == 6);
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
#endif
/* An explicit console owner propagates retained sessions to embedding callers and refuses overlapping commands. The former implementation is retained for owner review. */
#if 0
int UmiModelsCommand(int argc, char **argv)
{
    if (argc < 0 || argc > 9 || (argc != 0 && argv == NULL)) return Failure(UMI_STATUS_INVALID_ARGUMENT);
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL) return Failure(UMI_STATUS_INVALID_ARGUMENT);
    if (argc == 0 || (argc == 1 && (strcmp(argv[0], "help") == 0 || strcmp(argv[0], "--help") == 0))) { Help(); return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR); }
    if (strcmp(argv[0], "catalogue") == 0 && argc == 1) {
        for (size_t i = 0U; i < UmiModelCatalogueCount(); ++i) {
            const UmiModelCatalogueEntry *entry = UmiModelCatalogueAt(i);
            (void)printf("%s | %s | %s | %" PRIu64 " bytes\n  %s\n",
                entry->id, entry->title, entry->licence, entry->source.bytes, entry->source.sha256);
        }
        return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR);
    }
    if (strcmp(argv[0], "inspect") == 0 && (argc == 2 || argc == 3)) {
        UmiModelInspection inspected;
        UmiStatus status = UmiModelInspect(argv[1], argc == 3 ? argv[2] : NULL, NULL, &inspected);
        if (status != UMI_STATUS_OK) return Failure(status);
        Inspection(&inspected); return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR);
    }
    if (strcmp(argv[0], "health") == 0 && (argc == 3 ||
        (argc == 4 && strcmp(argv[3], "--probe") == 0)))
        return Health(argv[1], argv[2], argc == 4);
    if (strcmp(argv[0], "wait") == 0 && (argc == 5 ||
        (argc == 6 && strcmp(argv[5], "--probe") == 0)))
        return Readiness(argv[1], argv[2], argv[3], argv[4], argc == 6);
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
        (strcmp(argv[0], "serve") == 0 && ApplyArgument(argc, argv, 8, &apply)) ||
        (strcmp(argv[0], "describe") == 0 && argc == 7) ||
        (strcmp(argv[0], "describe-file") == 0 && argc == 8)) {
        bool describe = strcmp(argv[0], "describe") == 0 || strcmp(argv[0], "describe-file") == 0;
        bool catalogue = strcmp(argv[0], "run") == 0 || strcmp(argv[0], "describe") == 0;
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
        if (status != UMI_STATUS_OK) return Failure(status);
        return describe ? Describe(&plan, entry != NULL ? entry->title : plan.alias) : Serve(&plan, apply);
    }
    if (strcmp(argv[0], "chat") == 0 && (argc == 4 ||
        (argc == 5 && strcmp(argv[4], "--send-reviewed-prompt") == 0)))
        return Chat(argv[1], argv[2], argv[3], argc == 5);
    Help();
    return Failure(UMI_STATUS_INVALID_ARGUMENT);
}
#endif
int UmiModelsCommandWithSession(int argc, char **argv, UmiModelSession **inOutSession)
{
    /* An occupied slot is a cleanup obligation, not permission to replace it.
     * Refuse even a different command before parsing or writing the owner. */
    if (inOutSession == NULL) return Failure(UMI_STATUS_INVALID_ARGUMENT);
    if (*inOutSession != NULL) return Failure(UMI_STATUS_BUSY);
    if (argc < 0 || argc > 9 || (argc != 0 && argv == NULL)) return Failure(UMI_STATUS_INVALID_ARGUMENT);
    for (int i = 0; i < argc; ++i)
        if (argv[i] == NULL) return Failure(UMI_STATUS_INVALID_ARGUMENT);
    if (argc == 0 || (argc == 1 && (strcmp(argv[0], "help") == 0 || strcmp(argv[0], "--help") == 0))) { Help(); return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR); }
    if (strcmp(argv[0], "catalogue") == 0 && argc == 1) {
        for (size_t i = 0U; i < UmiModelCatalogueCount(); ++i) {
            const UmiModelCatalogueEntry *entry = UmiModelCatalogueAt(i);
            (void)printf("%s | %s | %s | %" PRIu64 " bytes\n  %s\n",
                entry->id, entry->title, entry->licence, entry->source.bytes, entry->source.sha256);
        }
        return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR);
    }
    if (strcmp(argv[0], "inspect") == 0 && (argc == 2 || argc == 3)) {
        UmiModelInspection inspected;
        UmiStatus status = UmiModelInspect(argv[1], argc == 3 ? argv[2] : NULL, NULL, &inspected);
        if (status != UMI_STATUS_OK) return Failure(status);
        Inspection(&inspected); return ReviewOutputComplete() ? 0 : Failure(UMI_STATUS_IO_ERROR);
    }
    if (strcmp(argv[0], "inspect-safetensors") == 0 && argc == 2)
        return InspectSafetensors(argv[1]);
    if (strcmp(argv[0], "health") == 0 && (argc == 3 ||
        (argc == 4 && strcmp(argv[3], "--probe") == 0)))
        return Health(argv[1], argv[2], argc == 4);
    if (strcmp(argv[0], "wait") == 0 && (argc == 5 ||
        (argc == 6 && strcmp(argv[5], "--probe") == 0)))
        return Readiness(argv[1], argv[2], argv[3], argv[4], argc == 6);
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
        (strcmp(argv[0], "serve") == 0 && ApplyArgument(argc, argv, 8, &apply)) ||
        (strcmp(argv[0], "describe") == 0 && argc == 7) ||
        (strcmp(argv[0], "describe-file") == 0 && argc == 8)) {
        bool describe = strcmp(argv[0], "describe") == 0 || strcmp(argv[0], "describe-file") == 0;
        bool catalogue = strcmp(argv[0], "run") == 0 || strcmp(argv[0], "describe") == 0;
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
        if (status != UMI_STATUS_OK) return Failure(status);
        return describe ? Describe(&plan, entry != NULL ? entry->title : plan.alias) : Serve(&plan, apply, inOutSession);
    }
    if (strcmp(argv[0], "chat") == 0 && (argc == 4 ||
        (argc == 5 && strcmp(argv[4], "--send-reviewed-prompt") == 0)))
        return Chat(argv[1], argv[2], argv[3], argc == 5);
    Help();
    return Failure(UMI_STATUS_INVALID_ARGUMENT);
}

/* The historical command entry has no output-owner argument. Keep its failure
 * obligation reachable for a host to inspect/retry, rather than discarding it.
 * All command calls and compatibility-owner access are serialized by the host. */
static UmiModelSession *legacy_command_session;

int UmiModelsCommand(int argc, char **argv)
{
    return UmiModelsCommandWithSession(argc, argv, &legacy_command_session);
}

bool UmiModelsCommandHasRetainedSession(void)
{
    return legacy_command_session != NULL;
}

UmiStatus UmiModelsCommandRelease(void)
{
    return UmiModelSessionStop(&legacy_command_session);
}
