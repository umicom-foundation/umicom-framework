/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_attach/adapter_fixture.c
 * PURPOSE: Provide an inert DAP peer that records attach and detach requests without touching a target.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/message.h"
#include "umicom/language_runtime/framing.h"
#include "umicom/language_runtime/json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif
static uint64_t sequence = 1U;
static int Send(const char *json)
{
    char *frame = malloc(UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY);
    if (frame == NULL)
        return 0;
    size_t bytes = 0U;
    UmiStatus status =
        umi_language_runtime_frame_encode(json, frame, UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY, &bytes);
    int good =
        status == UMI_STATUS_OK && fwrite(frame, 1U, bytes, stdout) == bytes && fflush(stdout) == 0;
    free(frame);
    return good;
}
static int Reply(uint64_t request, const char *command, int success, const char *body)
{
    char *json = malloc(UMI_DEBUG_RUNTIME_JSON_CAPACITY);
    if (json == NULL)
        return 0;
    UmiStatus status = umi_debug_runtime_build_response(sequence++, request, command, success,
                                                        success ? "" : "fixture refusal", body,
                                                        json, UMI_DEBUG_RUNTIME_JSON_CAPACITY);
    int good = status == UMI_STATUS_OK && Send(json);
    free(json);
    return good;
}
static int Event(const char *name, const char *body)
{
    char json[512];
    int bytes = snprintf(json, sizeof json,
                         "{\"seq\":%llu,\"type\":\"event\",\"event\":\"%s\",\"body\":%s}",
                         (unsigned long long)sequence++, name, body);
    return bytes > 0 && (size_t)bytes < sizeof json && Send(json);
}
int main(void)
{
#ifdef _WIN32
    (void)_setmode(_fileno(stdin), _O_BINARY);
    (void)_setmode(_fileno(stdout), _O_BINARY);
#endif
    /* The test gives each child an isolated working directory. No environment
     * variables, target handles, user debugger settings or real PID are used. */
    char mode[32] = "normal";
    FILE *settings = fopen("attach-fixture-mode.txt", "rb");
    if (settings != NULL)
    {
        size_t count = fread(mode, 1U, sizeof mode - 1U, settings);
        mode[count] = '\0';
        fclose(settings);
    }
    /* Tests clear the transcript before the first connection. Append across a
     * reconnect so assertions can distinguish requests from both connections. */
    FILE *log = fopen("attach-fixture-requests.jsonl", "ab");
    UmiLanguageRuntimeFramer *framer = calloc(1U, sizeof *framer);
    UmiDebugRuntimeEnvelope *request = malloc(sizeof *request);
    char *json = malloc(UMI_DEBUG_RUNTIME_JSON_CAPACITY);
    if (log == NULL || framer == NULL || request == NULL || json == NULL)
        return 2;
    umi_language_runtime_framer_init(framer);
    int result = 0, next;
    uint64_t deferred = 0U;
    unsigned exception_requests = 0U;
    while ((next = getchar()) != EOF)
    {
        unsigned char byte = (unsigned char)next;
        if (umi_language_runtime_framer_feed(framer, &byte, 1U) != UMI_STATUS_OK)
        {
            result = 3;
            break;
        }
        if (!umi_language_runtime_framer_has_message(framer))
            continue;
        size_t bytes = 0U;
        if (umi_language_runtime_framer_pop(framer, json, UMI_DEBUG_RUNTIME_JSON_CAPACITY,
                                            &bytes) != UMI_STATUS_OK ||
            umi_debug_runtime_message_parse(json, request) != UMI_STATUS_OK)
        {
            result = 4;
            break;
        }
        if (fwrite(json, 1U, bytes, log) != bytes || fputc('\n', log) == EOF || fflush(log) != 0)
        {
            result = 5;
            break;
        }
        int sent = 0;
        if (strcmp(request->command, "initialize") == 0)
            sent = Reply(
                request->sequence, "initialize", 1,
                strncmp(mode, "exceptions", 10U) == 0
                    ? "{\"supportsConfigurationDoneRequest\":true,\"exceptionBreakpointFilters\":["
                      "{\"filter\":\"caught\",\"label\":\"Caught exception\",\"default\":true},"
                      "{\"filter\":\"uncaught\",\"label\":\"Uncaught exception\"}]}"
                : strncmp(mode, "functions", 9U) == 0
                    ? strcmp(mode, "functions-no-condition") == 0
                          ? "{\"supportsConfigurationDoneRequest\":true,"
                            "\"supportsFunctionBreakpoints\":true}"
                          : "{\"supportsConfigurationDoneRequest\":true,"
                            "\"supportsFunctionBreakpoints\":true,"
                            "\"supportsConditionalBreakpoints\":true,"
                            "\"supportsHitConditionalBreakpoints\":true}"
                : strncmp(mode, "modules", 7U) == 0
                    ? "{\"supportsConfigurationDoneRequest\":true,\"supportsModulesRequest\":true}"
                : strncmp(mode, "information", 11U) == 0
                    ? "{\"supportsConfigurationDoneRequest\":true,\"supportsExceptionInfoRequest\":"
                      "true}"
                : strncmp(mode, "sources", 7U) == 0
                    ? "{\"supportsConfigurationDoneRequest\":true,\"supportsLoadedSourcesRequest\":"
                      "true,\"supportsRestartRequest\":true}"
                    : "{\"supportsConfigurationDoneRequest\":true}");
        else if (strcmp(request->command, "attach") == 0)
        {
            if (strcmp(mode, "refuse") == 0)
                sent = Reply(request->sequence, "attach", 0, "{}");
            else if (strcmp(mode, "timeout") == 0)
                sent = 1;
            else
            {
                sent = Event("initialized", "{}");
                if (strcmp(mode, "deferred") == 0)
                    deferred = request->sequence;
                else
                    sent = sent && Reply(request->sequence, "attach", 1, "{}");
            }
        }
        else if (strcmp(request->command, "setExceptionBreakpoints") == 0)
        {
            ++exception_requests;
            if (exception_requests > 1U && strcmp(mode, "exceptions-refuse") == 0)
                sent = Reply(request->sequence, "setExceptionBreakpoints", 0, "{}");
            else if (exception_requests > 1U && strcmp(mode, "exceptions-timeout") == 0)
                sent = 1;
            else if (exception_requests > 1U && strcmp(mode, "exceptions-malformed") == 0)
                sent =
                    Reply(request->sequence, "setExceptionBreakpoints", 1, "{\"breakpoints\":[]}");
            else if (exception_requests > 1U && strcmp(mode, "exceptions-unverified") == 0)
                sent = Reply(request->sequence, "setExceptionBreakpoints", 1,
                             "{\"breakpoints\":[{\"verified\":false,\"message\":\"fixture could "
                             "not install filter\"}]}");
            else
                sent = Reply(request->sequence, "setExceptionBreakpoints", 1, "{}");
        }
        else if (strcmp(request->command, "setFunctionBreakpoints") == 0)
        {
            if (strcmp(mode, "functions-refuse") == 0)
                sent = Reply(request->sequence, "setFunctionBreakpoints", 0, "{}");
            else if (strcmp(mode, "functions-timeout") == 0)
                sent = 1;
            else if (strcmp(mode, "functions-malformed") == 0)
                sent =
                    Reply(request->sequence, "setFunctionBreakpoints", 1, "{\"breakpoints\":[]}");
            else
            {
                UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
                if (document == NULL ||
                    umi_language_runtime_json_parse(json, document) != UMI_STATUS_OK)
                {
                    free(document);
                    result = 8;
                    break;
                }
                int arguments = umi_language_runtime_json_object_get(document, 0, "arguments");
                int breakpoints =
                    umi_language_runtime_json_object_get(document, arguments, "breakpoints");
                size_t count = umi_language_runtime_json_array_count(document, breakpoints);
                char body[8192];
                size_t used = (size_t)snprintf(body, sizeof body, "{\"breakpoints\":[");
                for (size_t i = 0U; i < count && i < 32U; ++i)
                {
                    int written =
                        snprintf(body + used, sizeof body - used,
                                 "%s{\"id\":%zu,\"verified\":%s,\"message\":\"fixture result\"}",
                                 i ? "," : "", i + 100U,
                                 strcmp(mode, "functions-unverified") == 0 ? "false" : "true");
                    if (written <= 0 || (size_t)written >= sizeof body - used)
                    {
                        result = 9;
                        break;
                    }
                    used += (size_t)written;
                }
                if (result == 0)
                {
                    (void)snprintf(body + used, sizeof body - used, "]}");
                    sent = Reply(request->sequence, "setFunctionBreakpoints", 1, body);
                }
                free(document);
            }
        }

        else if (strcmp(request->command, "loadedSources") == 0)
        {
            if (strcmp(mode, "sources-list-exit") == 0 && !Event("terminated", "{}"))
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "sources-refuse") == 0)
                sent = Reply(request->sequence, "loadedSources", 0, "{}");
            else if (strcmp(mode, "sources-timeout") == 0)
                sent = 1;
            else if (strcmp(mode, "sources-malformed") == 0)
                sent = Reply(request->sequence, "loadedSources", 1, "{\"sources\":{}}");
            else
                sent = Reply(request->sequence, "loadedSources", 1,
                             "{\"sources\":[{\"name\":\"generated.c\",\"path\":\"/remote/"
                             "generated.c\",\"sourceReference\":42},"
                             "{\"name\":\"local.c\",\"path\":\"/untrusted/local.c\"}]}");
        }
        else if (strcmp(request->command, "restart") == 0)
        {
            sent = Event("initialized", "{}");
            sent = sent && Reply(request->sequence, "restart", 1, "{}");
        }

        else if (strcmp(request->command, "exceptionInfo") == 0)
        {
            if (strcmp(mode, "information-refuse") == 0)
                sent = Reply(request->sequence, "exceptionInfo", 0, "{}");
            else if (strcmp(mode, "information-timeout") == 0)
                sent = 1;
            else if (strcmp(mode, "information-malformed") == 0)
                sent = Reply(request->sequence, "exceptionInfo", 1, "{\"exceptionId\":false}");
            else
            {
                if (strcmp(mode, "information-continued") == 0 &&
                    !Event("continued", "{\"threadId\":1,\"allThreadsContinued\":true}"))
                {
                    result = 6;
                    break;
                }
                sent = Reply(
                    request->sequence, "exceptionInfo", 1,
                    "{\"exceptionId\":\"ArithmeticError\",\"description\":\"Fixture exception\","
                    "\"breakMode\":\"always\",\"details\":{\"message\":\"fixture zero divisor\","
                    "\"typeName\":\"ArithmeticError\",\"evaluateName\":\"dangerous()\","
                    "\"stackTrace\":\"generated.c:7\\n\",\"innerException\":[{\"message\":\"inner "
                    "cause\"}]}}");
            }
        }
        else if (strcmp(request->command, "source") == 0)
        {
            if (strcmp(mode, "sources-content-exit") == 0 && !Event("terminated", "{}"))
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "sources-content-refuse") == 0)
                sent = Reply(request->sequence, "source", 0, "{}");
            else if (strcmp(mode, "sources-content-timeout") == 0)
                sent = 1;
            else if (strcmp(mode, "sources-content-invalid") == 0)
                sent = Reply(request->sequence, "source", 1, "{\"content\":false}");
            else
                sent = Reply(request->sequence, "source", 1,
                             "{\"content\":\"int generated(void) { return 42; "
                             "}\\n\",\"mimeType\":\"text/x-c\"}");
        }
        else if (strcmp(request->command, "modules") == 0)
        {
            if (strcmp(mode, "modules-inflight-exit") == 0 && !Event("terminated", "{}"))
            {
                result = 6;
                break;
            }
            if (strcmp(mode, "modules-refuse") == 0)
                sent = Reply(request->sequence, "modules", 0, "{}");
            else if (strcmp(mode, "modules-timeout") == 0)
                sent = 1;
            else if (strcmp(mode, "modules-malformed") == 0)
                sent = Reply(request->sequence, "modules", 1, "{\"modules\":{}}");
            else
            {
                UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
                if (document == NULL ||
                    umi_language_runtime_json_parse(json, document) != UMI_STATUS_OK)
                {
                    free(document);
                    result = 10;
                    break;
                }
                int arguments = umi_language_runtime_json_object_get(document, 0, "arguments");
                int token =
                    umi_language_runtime_json_object_get(document, arguments, "startModule");
                int64_t first = 0, count = 0;
                if (umi_language_runtime_json_int64(document, token, &first) != UMI_STATUS_OK)
                {
                    free(document);
                    result = 11;
                    break;
                }
                token = umi_language_runtime_json_object_get(document, arguments, "moduleCount");
                if (umi_language_runtime_json_int64(document, token, &count) != UMI_STATUS_OK ||
                    first < 0 || count < 1 || count > 512)
                {
                    free(document);
                    result = 12;
                    break;
                }
                char body[8192];
                size_t used = (size_t)snprintf(body, sizeof body, "{\"modules\":[");
                int64_t remaining = first < 35 ? 35 - first : 0;
                if (count > remaining)
                    count = remaining;
                for (int64_t i = 0; i < count; ++i)
                {
                    int written =
                        snprintf(body + used, sizeof body - used,
                                 "%s{\"id\":%lld,\"name\":\"fixture-%lld\",\"symbolStatus\":"
                                 "\"Symbols loaded\"}",
                                 i ? "," : "", (long long)(first + i), (long long)(first + i));
                    if (written <= 0 || (size_t)written >= sizeof body - used)
                    {
                        result = 13;
                        break;
                    }
                    used += (size_t)written;
                }
                if (result == 0)
                {
                    snprintf(body + used, sizeof body - used,
                             strcmp(mode, "modules-unknown") == 0 ? "]}"
                                                                  : "],\"totalModules\":35}");
                    sent = Reply(request->sequence, "modules", 1, body);
                }
                free(document);
            }
        }
        else if (strcmp(request->command, "configurationDone") == 0)
        {
            sent = Reply(request->sequence, "configurationDone", 1, "{}");
            if (deferred != 0U)
                sent = sent && Reply(deferred, "attach", 1, "{}");
            sent =
                sent &&
                Event("stopped",
                      strncmp(mode, "information", 11U) == 0 &&
                              strcmp(mode, "information-other-stop") != 0
                          ? "{\"reason\":\"exception\",\"threadId\":1,\"allThreadsStopped\":true}"
                          : "{\"reason\":\"pause\",\"threadId\":1,\"allThreadsStopped\":true}");
        }
        else if (strcmp(request->command, "disconnect") == 0)
        {
            sent = Reply(request->sequence, "disconnect", 1, "{}");
            if (!sent)
                result = 6;
            break;
        }
        else
            sent = Reply(request->sequence, request->command, 1, "{}");
        if (!sent)
        {
            result = 7;
            break;
        }
    }
    fclose(log);
    free(json);
    free(request);
    free(framer);
    return result;
}
