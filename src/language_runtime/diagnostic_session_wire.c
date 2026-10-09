/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_session_wire.c
 * PURPOSE: Validate and serialize full source revisions before any language-server write.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "diagnostic_session_internal.h"
#include "umicom/language_runtime/message.h"
#include <stdlib.h>
#include <string.h>

UmiStatus DiagnosticSessionSource(const char *text, size_t bytes, UmiEditorTextPosition *end)
{
    if (text == NULL || end == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, '\0', bytes) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof view;
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = bytes;
    view.capacity = bytes;
    UmiStatus status = UmiEditorTextViewPositionAt(&view, bytes, end);
    if (status == UMI_STATUS_OK && (end->line > INT32_MAX || end->utf16_column > INT32_MAX))
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    return status;
}
static UmiStatus DiagnosticSessionText(const char *text, char *out, size_t capacity)
{
    if (text == NULL || text[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = strlen(text);
    if (bytes >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiEditorTextPosition end;
    UmiStatus status = DiagnosticSessionSource(text, bytes, &end);
    if (status == UMI_STATUS_OK)
        memcpy(out, text, bytes + 1U);
    return status;
}
UmiStatus DiagnosticSessionDocument(const UmiLanguageDiagnosticSession *session, const char *text,
                                    size_t bytes, int32_t version, int opening, char *out)
{
    UmiEditorTextPosition end;
    UmiStatus status = DiagnosticSessionSource(text, bytes, &end);
    if (status != UMI_STATUS_OK)
        return status;
    char *copy = malloc(bytes + 1U), *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (copy == NULL || envelope == NULL)
    {
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, text, bytes);
    copy[bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, out, UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, session->uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":");
    umi_language_runtime_json_writer_uint64(&writer, (uint64_t)version);
    if (opening)
    {
        umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
        umi_language_runtime_json_writer_string(&writer, session->language);
        umi_language_runtime_json_writer_raw(&writer, ",\"text\":");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"contentChanges\":[{");
        if (session->sync_kind == 2)
        {
            /* Replace exactly the preceding UTF-16 range. This also handles
             * astral characters and an empty final line without byte guesses. */
            status = DiagnosticSessionSource(session->source, session->bytes, &end);
            umi_language_runtime_json_writer_raw(
                &writer, "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}},");
        }
        umi_language_runtime_json_writer_raw(&writer, "\"text\":");
    }
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, opening ? "}}" : "}]}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification(
            opening ? "textDocument/didOpen" : "textDocument/didChange", out, envelope,
            UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    free(envelope);
    free(copy);
    return status;
}
UmiStatus DiagnosticSessionPrepare(const UmiLanguageDiagnosticRequest *request,
                                   UmiLanguageDiagnosticSession **out,
                                   DiagnosticSessionWire **out_wire)
{
    *out = NULL;
    *out_wire = NULL;
    if (request == NULL || request->source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageDiagnosticSession *session = calloc(1U, sizeof *session);
    DiagnosticSessionWire *wire = calloc(1U, sizeof *wire);
    if (session == NULL || wire == NULL)
    {
        free(session);
        free(wire);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status =
        DiagnosticSessionText(request->root_uri, session->root, sizeof session->root);
    if (status == UMI_STATUS_OK)
        status = DiagnosticSessionText(request->document_uri, session->uri, sizeof session->uri);
    if (status == UMI_STATUS_OK)
        status = DiagnosticSessionText(request->language_id, session->language,
                                       sizeof session->language);
    if (status == UMI_STATUS_OK)
        status = DiagnosticSessionDocument(session, request->source, request->source_bytes, 1, 1,
                                           wire->document);
    if (status == UMI_STATUS_OK)
    {
        session->source = malloc(request->source_bytes + 1U);
        if (session->source == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
        {
            memcpy(session->source, request->source, request->source_bytes);
            session->source[request->source_bytes] = '\0';
            session->bytes = request->source_bytes;
        }
    }
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof wire->initialize);
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, session->root);
    /* Advertise only the features this session owns. No dynamic registrations,
     * edits, configuration callbacks or work-done requests are promised. */
    umi_language_runtime_json_writer_raw(
        &writer,
        ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
        "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
        "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
        "\"publishDiagnostics\":{\"relatedInformation\":true,\"versionSupport\":true,"
        "\"codeDescriptionSupport\":true,\"dataSupport\":true,\"tagSupport\":{\"valueSet\":[1,2]}}}"
        "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status != UMI_STATUS_OK)
    {
        free(session->source);
        free(session);
        free(wire);
        return status;
    }
    *out = session;
    *out_wire = wire;
    return UMI_STATUS_OK;
}
