/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/diagnostic_session/server_fixture.c
 * PURPOSE: Provide a native framed server for persistent source updates and cancellation exercises.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/framing.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/message.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif
static int Send(const char *json)
{
    char *frame = malloc(UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY);
    if (frame == NULL)
        return 0;
    size_t bytes = 0U;
    UmiStatus status =
        umi_language_runtime_frame_encode(json, frame, UMI_LANGUAGE_RUNTIME_FRAME_CAPACITY, &bytes);
    int ok =
        status == UMI_STATUS_OK && fwrite(frame, 1U, bytes, stdout) == bytes && fflush(stdout) == 0;
    free(frame);
    return ok;
}
static int Publish(const char *json, int opening, const char *mode)
{
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(json, strlen(json), NULL, NULL, &tree);
    int params = -1, document = -1, node = -1;
    char *uri = malloc(8192U), *text = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY),
         *reply = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (uri == NULL || text == NULL || reply == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, 0, "params", &params);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, params, "textDocument", &document);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, document, "uri", &node);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, node, uri, 8192U);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeMember(tree, document, "version", &node);
    int64_t version = 0;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, node, &version);
    if (status == UMI_STATUS_OK && opening)
        status = UmiJsonTreeMember(tree, document, "text", &node);
    else if (status == UMI_STATUS_OK)
    {
        status = UmiJsonTreeMember(tree, params, "contentChanges", &node);
        if (status == UMI_STATUS_OK && UmiJsonTreeCount(tree, node) != 1U)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeMember(tree, UmiJsonTreeFirst(tree, node), "text", &node);
    }
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeText(tree, node, text, UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    int ok = 0;
    if (status == UMI_STATUS_OK)
    {
        if (strcmp(mode, "delay") == 0)
            umi_thread_sleep_ms(180U);
        UmiLanguageRuntimeJsonWriter writer;
        umi_language_runtime_json_writer_init(&writer, reply, UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
        umi_language_runtime_json_writer_raw(&writer,
                                             "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/"
                                             "publishDiagnostics\",\"params\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, uri);
        if (strcmp(mode, "unversioned") != 0)
        {
            umi_language_runtime_json_writer_raw(&writer, ",\"version\":");
            umi_language_runtime_json_writer_uint64(&writer, (uint64_t)version);
        }
        umi_language_runtime_json_writer_raw(
            &writer, ",\"diagnostics\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":"
                     "{\"line\":0,\"character\":0}},\"severity\":2,\"message\":");
        umi_language_runtime_json_writer_string(&writer, text);
        umi_language_runtime_json_writer_raw(&writer, "}]}}");
        ok = writer.status == UMI_STATUS_OK && Send(reply);
    }
    UmiJsonTreeDestroy(tree);
    free(uri);
    free(text);
    free(reply);
    return ok;
}
int main(int argc, char **argv)
{
#ifdef _WIN32
    (void)_setmode(_fileno(stdin), _O_BINARY);
    (void)_setmode(_fileno(stdout), _O_BINARY);
#endif
    const char *mode = argc == 2 ? argv[1] : "normal";
    UmiLanguageRuntimeFramer *framer = calloc(1U, sizeof *framer);
    UmiLanguageRuntimeEnvelope *message = malloc(sizeof *message);
    char *json = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (framer == NULL || message == NULL || json == NULL)
        return 2;
    umi_language_runtime_framer_init(framer);
    int result = 0, next;
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
        if (umi_language_runtime_framer_pop(framer, json, UMI_LANGUAGE_RUNTIME_JSON_CAPACITY,
                                            &bytes) != UMI_STATUS_OK ||
            umi_language_runtime_message_parse(json, message) != UMI_STATUS_OK)
        {
            result = 4;
            break;
        }
        if (strcmp(message->method, "initialize") == 0)
        {
            char reply[512];
            snprintf(reply, sizeof reply,
                     "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":{\"capabilities\":{"
                     "\"textDocumentSync\":{\"openClose\":true,\"change\":2},\"positionEncoding\":"
                     "\"utf-16\"}}}",
                     (unsigned long long)message->request_id);
            if (!Send(reply))
            {
                result = 5;
                break;
            }
        }
        else if (strcmp(message->method, "textDocument/didOpen") == 0 ||
                 strcmp(message->method, "textDocument/didChange") == 0)
        {
            if (strcmp(mode, "request") == 0)
            {
                if (!Send("{\"jsonrpc\":\"2.0\",\"id\":99,\"method\":\"workspace/"
                          "configuration\",\"params\":{}}"))
                {
                    result = 6;
                    break;
                }
            }
            else if (!Publish(json, strcmp(message->method, "textDocument/didOpen") == 0, mode))
            {
                result = 7;
                break;
            }
        }
        else if (strcmp(message->method, "shutdown") == 0)
        {
            char reply[128];
            snprintf(reply, sizeof reply, "{\"jsonrpc\":\"2.0\",\"id\":%llu,\"result\":null}",
                     (unsigned long long)message->request_id);
            if (!Send(reply))
            {
                result = 8;
                break;
            }
        }
        else if (strcmp(message->method, "exit") == 0)
            break;
    }
    free(json);
    free(message);
    free(framer);
    return result;
}
