/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/completion_query.c
 * PURPOSE: Keep native completion transport, input capture and cleanup in the shared language runtime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/completion_query.h"
#include "completion_catalogue_internal.h"
#include "server_profile_internal.h"
#include "umicom/language_runtime/formatting_query.h"
#include "umicom/language_runtime/hover_query.h"
#include "umicom/language_runtime/navigation_query.h"
#include "umicom/language_runtime/symbol_query.h"
#include "umicom/language_runtime/call_query.h"
#include "umicom/language_runtime/signature_query.h"
#include "umicom/language_runtime/rename_query.h"
#include "umicom/language_runtime/code_action_query.h"
#include "umicom/language_runtime/diagnostic_query.h"
#include "umicom/language_runtime/diagnostic_context.h"
#include "umicom/editor/text_position_index.h"
#include "umicom/language_runtime/response_tree.h"
#include "umicom/editor/text_position.h"
#include "umicom/platform/clock.h"
#include <stdlib.h>
#include <string.h>

/* The transport owner is shared by completion and formatting. Each operation
 * declares only the capability and parameters it actually supports, then
 * decodes into an owned result before the common close/shutdown boundary. */
/* A named operation replaces overlapping mode flags as source tools grow.
 * Retain the previous layout for review; callers now select exactly one kind. */
#if 0
typedef struct SourceQueryPolicy
{
    int formatting;
    uint32_t tab_size;
    int insert_spaces;
    int hover;
} SourceQueryPolicy;
#endif
typedef enum SourceQueryKind
{
    SOURCE_QUERY_COMPLETION,
    SOURCE_QUERY_FORMATTING,
    SOURCE_QUERY_HOVER,
    SOURCE_QUERY_DEFINITION,
    SOURCE_QUERY_REFERENCES,
    SOURCE_QUERY_TYPE_DEFINITION,
    SOURCE_QUERY_IMPLEMENTATION,
    SOURCE_QUERY_SELECTION_RANGES,
    SOURCE_QUERY_FOLDING_RANGES,
    SOURCE_QUERY_SYMBOLS,
    SOURCE_QUERY_CALL_HIERARCHY,
    SOURCE_QUERY_SIGNATURE,
    SOURCE_QUERY_RENAME,
    SOURCE_QUERY_CODE_ACTION,
    SOURCE_QUERY_DIAGNOSTICS
} SourceQueryKind;
typedef struct SourceQueryPolicy
{
    SourceQueryKind kind;
    uint32_t tab_size;
    int insert_spaces, include_declaration;
    const char *rename_name;
    const char *workspace_symbol_query;
    size_t range_start, range_end;
    int range_formatting;
    UmiLanguageCodeActionFilter action_filter;
    const char *action_only_kind;
    size_t resolve_action_limit;
    UmiLanguageCallDirection call_direction;
    int diagnostic_context;
    int pull_diagnostics;
    const UmiLanguageQuerySource *additional_sources;
    size_t source_count;
} SourceQueryPolicy;
typedef struct SourceQueryOutput
{
    UmiLanguageCompletionCatalogue *completion;
    UmiLanguageTextEditPreview *formatting;
    UmiLanguageHoverDocument *hover;
    UmiLanguageLocationCatalogue *locations;
    UmiLanguageSymbolCatalogue *symbols;
    UmiLanguageCallResult *calls;
    UmiLanguageSignatureCatalogue *signatures;
    UmiLanguageWorkspaceEditCatalogue *workspace_edits;
    UmiLanguageCodeActionCatalogue *actions;
    UmiLanguageDiagnosticCatalogue *diagnostics;
} SourceQueryOutput;
/* Preflight all additional source notifications before process ownership.
 * Fixed per-message bounds match the existing native transport contract. */
typedef struct SourceQueryDocumentWire
{
    char open[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY], close[8192];
} SourceQueryDocumentWire;
typedef struct CompletionQueryWire
{
    char initialize[8192], open[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY];
    char position[8192], close[8192], prepare[8192];
    SourceQueryDocumentWire *additional;
    size_t additional_count;
} CompletionQueryWire;
static UmiStatus QueryText(const char *text, size_t bytes)
{
    if (text == NULL || memchr(text, '\0', bytes) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = bytes;
    view.capacity = bytes;
    UmiEditorTextPosition end;
    return UmiEditorTextViewPositionAt(&view, bytes, &end);
}
#include "rename_query_support.inc"
#include "query_sources.inc"

/* Operation-specific capabilities and parameters now share source validation and JSON sizing.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    umi_language_runtime_json_writer_raw(
        &writer,
        ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
        "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
        "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
        "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
        "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":[1]}}"
        ","
        "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}}");
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
    umi_language_runtime_json_writer_uint64(&writer, cursor.line);
    umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
    umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
    umi_language_runtime_json_writer_raw(&writer, "},\"context\":{\"triggerKind\":1}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Source-information requests now share owned draft serialization while advertising only their implemented capability.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->formatting)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->formatting)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"context\":{\"triggerKind\":1}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Named source operations share exact UTF-16 positions and advertise only their implemented capability.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->hover)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->formatting)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->formatting)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             policy->hover ? "}}" : "},\"context\":{\"triggerKind\":1}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Document-symbol queries send only document identity and advertise the hierarchy, kinds and tags their owned reader supports.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Parameter help reuses captured source validation and advertises only the signature features its complete reader supports.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Rename reuses source capture and bounded JSON preflight; separate preparation parameters preserve the protocol contract.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Code actions validate both captured range endpoints and advertise literal proposals without deferred resolution or command execution.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Diagnostic capture advertises the publication fields it retains, without assuming a pull-diagnostic provider or persistent workspace configuration.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Explicit diagnostic-backed actions advertise preservation of diagnostic data only while reusing that same server session.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Preflight every captured draft notification before launching or taking ownership of a temporary source connection.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        free(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        free(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Advertise explicit pull diagnostics only for callers selecting that mode; retain notification-based diagnostic queries unchanged.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Advertise retained diagnostic data and explicit pull capability together only when code actions will use that data within this same connection.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Project symbol discovery uses explicit workspace capabilities and bounded search text while retaining primary draft synchronization and native message preflight.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Negotiate type and implementation location links using the same captured UTF-16 source protocol as definitions.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        size_t query_bytes = strlen(policy->workspace_symbol_query);
        status = query_bytes > 4096U ? UMI_STATUS_CAPACITY_EXCEEDED
                                     : QueryText(policy->workspace_symbol_query, query_bytes);
    }
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_SYMBOLS && policy->workspace_symbol_query != NULL)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"symbol\":{"
            "\"dynamicRegistration\":false,\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,"
            "14,15,16,17,18,19,20,21,22,23,24,25,26]},\"tagSupport\":{\"valueSet\":[1]}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context &&
             policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        /* Workspace symbols take a query rather than a document identifier.
         * Build that independent request only after validating its full text. */
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"query\":");
        umi_language_runtime_json_writer_string(&writer, policy->workspace_symbol_query);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Selected-range formatting negotiates its own capability and converts exact captured byte boundaries before process ownership; full-document formatting remains available.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        size_t query_bytes = strlen(policy->workspace_symbol_query);
        status = query_bytes > 4096U ? UMI_STATUS_CAPACITY_EXCEEDED
                                     : QueryText(policy->workspace_symbol_query, query_bytes);
    }
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_SYMBOLS && policy->workspace_symbol_query != NULL)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"symbol\":{"
            "\"dynamicRegistration\":false,\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,"
            "14,15,16,17,18,19,20,21,22,23,24,25,26]},\"tagSupport\":{\"valueSet\":[1]}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context &&
             policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
             policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_TYPE_DEFINITION
                         ? "\"typeDefinition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_IMPLEMENTATION
                         ? "\"implementation\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        /* Workspace symbols take a query rather than a document identifier.
         * Build that independent request only after validating its full text. */
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"query\":");
        umi_language_runtime_json_writer_string(&writer, policy->workspace_symbol_query);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Encode an explicit action-family request without altering the captured range, trigger or default empty diagnostic context.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        size_t query_bytes = strlen(policy->workspace_symbol_query);
        status = query_bytes > 4096U ? UMI_STATUS_CAPACITY_EXCEEDED
                                     : QueryText(policy->workspace_symbol_query, query_bytes);
    }
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition format_start = {0}, format_end = {0};
    if (status == UMI_STATUS_OK && policy->range_formatting)
    {
        if (policy->range_start > policy->range_end || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_start, &format_start);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &format_end);
        if (status == UMI_STATUS_OK &&
            (format_start.line > INT32_MAX || format_start.utf16_column > INT32_MAX ||
             format_end.line > INT32_MAX || format_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_SYMBOLS && policy->workspace_symbol_query != NULL)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"symbol\":{"
            "\"dynamicRegistration\":false,\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,"
            "14,15,16,17,18,19,20,21,22,23,24,25,26]},\"tagSupport\":{\"valueSet\":[1]}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context &&
             policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
             policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_TYPE_DEFINITION
                         ? "\"typeDefinition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_IMPLEMENTATION
                         ? "\"implementation\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING && policy->range_formatting)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rangeFormatting\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer,
                                             "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        /* The range guides the formatter; returned edits remain complete so
         * surrounding indentation changes can be reviewed rather than clipped. */
        if (policy->range_formatting)
        {
            umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}");
        }
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        /* Workspace symbols take a query rather than a document identifier.
         * Build that independent request only after validating its full text. */
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"query\":");
        umi_language_runtime_json_writer_string(&writer, policy->workspace_symbol_query);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Negotiate syntax-selection support and encode exactly one captured caret in the protocol positions array before process ownership.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        size_t query_bytes = strlen(policy->workspace_symbol_query);
        status = query_bytes > 4096U ? UMI_STATUS_CAPACITY_EXCEEDED
                                     : QueryText(policy->workspace_symbol_query, query_bytes);
    }
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition format_start = {0}, format_end = {0};
    if (status == UMI_STATUS_OK && policy->range_formatting)
    {
        if (policy->range_start > policy->range_end || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_start, &format_start);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &format_end);
        if (status == UMI_STATUS_OK &&
            (format_start.line > INT32_MAX || format_start.utf16_column > INT32_MAX ||
             format_end.line > INT32_MAX || format_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_SYMBOLS && policy->workspace_symbol_query != NULL)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"symbol\":{"
            "\"dynamicRegistration\":false,\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,"
            "14,15,16,17,18,19,20,21,22,23,24,25,26]},\"tagSupport\":{\"valueSet\":[1]}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context &&
             policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
             policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_TYPE_DEFINITION
                         ? "\"typeDefinition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_IMPLEMENTATION
                         ? "\"implementation\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING && policy->range_formatting)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rangeFormatting\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1");
        if (policy->action_only_kind != NULL && policy->action_only_kind[0] != '\0')
        {
            umi_language_runtime_json_writer_raw(&writer, ",\"only\":[");
            umi_language_runtime_json_writer_string(&writer, policy->action_only_kind);
            umi_language_runtime_json_writer_raw(&writer, "]");
        }
        umi_language_runtime_json_writer_raw(&writer, "}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        /* The range guides the formatter; returned edits remain complete so
         * surrounding indentation changes can be reviewed rather than clipped. */
        if (policy->range_formatting)
        {
            umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}");
        }
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        /* Workspace symbols take a query rather than a document identifier.
         * Build that independent request only after validating its full text. */
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"query\":");
        umi_language_runtime_json_writer_string(&writer, policy->workspace_symbol_query);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Advertise edit-only resolution only for an explicit resolving query, keeping compatibility requests and diagnostic scope unchanged.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        size_t query_bytes = strlen(policy->workspace_symbol_query);
        status = query_bytes > 4096U ? UMI_STATUS_CAPACITY_EXCEEDED
                                     : QueryText(policy->workspace_symbol_query, query_bytes);
    }
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition format_start = {0}, format_end = {0};
    if (status == UMI_STATUS_OK && policy->range_formatting)
    {
        if (policy->range_start > policy->range_end || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_start, &format_start);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &format_end);
        if (status == UMI_STATUS_OK &&
            (format_start.line > INT32_MAX || format_start.utf16_column > INT32_MAX ||
             format_end.line > INT32_MAX || format_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_SYMBOLS && policy->workspace_symbol_query != NULL)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"symbol\":{"
            "\"dynamicRegistration\":false,\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,"
            "14,15,16,17,18,19,20,21,22,23,24,25,26]},\"tagSupport\":{\"valueSet\":[1]}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context &&
             policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"selectionRange\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
             policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_TYPE_DEFINITION
                         ? "\"typeDefinition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_IMPLEMENTATION
                         ? "\"implementation\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING && policy->range_formatting)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rangeFormatting\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1");
        if (policy->action_only_kind != NULL && policy->action_only_kind[0] != '\0')
        {
            umi_language_runtime_json_writer_raw(&writer, ",\"only\":[");
            umi_language_runtime_json_writer_string(&writer, policy->action_only_kind);
            umi_language_runtime_json_writer_raw(&writer, "]");
        }
        umi_language_runtime_json_writer_raw(&writer, "}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        /* The range guides the formatter; returned edits remain complete so
         * surrounding indentation changes can be reviewed rather than clipped. */
        if (policy->range_formatting)
        {
            umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}");
        }
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"positions\":[{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "}]}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        /* Workspace symbols take a query rather than a document identifier.
         * Build that independent request only after validating its full text. */
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"query\":");
        umi_language_runtime_json_writer_string(&writer, policy->workspace_symbol_query);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Advertise direct call inspection explicitly while preserving captured-source validation and request limits.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        size_t query_bytes = strlen(policy->workspace_symbol_query);
        status = query_bytes > 4096U ? UMI_STATUS_CAPACITY_EXCEEDED
                                     : QueryText(policy->workspace_symbol_query, query_bytes);
    }
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition format_start = {0}, format_end = {0};
    if (status == UMI_STATUS_OK && policy->range_formatting)
    {
        if (policy->range_start > policy->range_end || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_start, &format_start);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &format_end);
        if (status == UMI_STATUS_OK &&
            (format_start.line > INT32_MAX || format_start.utf16_column > INT32_MAX ||
             format_end.line > INT32_MAX || format_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->resolve_action_limit != 0U)
    {
        /* Advertise only the property this owner can resolve. Provider commands
         * remain unavailable; complete text edits still need explicit review. */
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":true,"
            "\"resolveSupport\":{\"properties\":[\"edit\"]},\"honorsChangeAnnotations\":true}");
        if (policy->diagnostic_context)
        {
            umi_language_runtime_json_writer_raw(
                &writer,
                policy->pull_diagnostics
                    ? ",\"diagnostic\":{\"dynamicRegistration\":false,\"relatedDocumentSupport\":false,"
                      "\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
                      "\"codeDescriptionSupport\":true,\"dataSupport\":true}"
                    : ",\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,"
                      "2]},"
                      "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":true}");
        }
        umi_language_runtime_json_writer_raw(&writer, "}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS && policy->workspace_symbol_query != NULL)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"symbol\":{"
            "\"dynamicRegistration\":false,\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,"
            "14,15,16,17,18,19,20,21,22,23,24,25,26]},\"tagSupport\":{\"valueSet\":[1]}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context &&
             policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"selectionRange\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
             policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_TYPE_DEFINITION
                         ? "\"typeDefinition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_IMPLEMENTATION
                         ? "\"implementation\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING && policy->range_formatting)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rangeFormatting\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1");
        if (policy->action_only_kind != NULL && policy->action_only_kind[0] != '\0')
        {
            umi_language_runtime_json_writer_raw(&writer, ",\"only\":[");
            umi_language_runtime_json_writer_string(&writer, policy->action_only_kind);
            umi_language_runtime_json_writer_raw(&writer, "]");
        }
        umi_language_runtime_json_writer_raw(&writer, "}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        /* The range guides the formatter; returned edits remain complete so
         * surrounding indentation changes can be reviewed rather than clipped. */
        if (policy->range_formatting)
        {
            umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}");
        }
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"positions\":[{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "}]}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        /* Workspace symbols take a query rather than a document identifier.
         * Build that independent request only after validating its full text. */
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"query\":");
        umi_language_runtime_json_writer_string(&writer, policy->workspace_symbol_query);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
/* Negotiate complete-line folding and preflight document-only parameters before launching a language process.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        size_t query_bytes = strlen(policy->workspace_symbol_query);
        status = query_bytes > 4096U ? UMI_STATUS_CAPACITY_EXCEEDED
                                     : QueryText(policy->workspace_symbol_query, query_bytes);
    }
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition format_start = {0}, format_end = {0};
    if (status == UMI_STATUS_OK && policy->range_formatting)
    {
        if (policy->range_start > policy->range_end || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_start, &format_start);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &format_end);
        if (status == UMI_STATUS_OK &&
            (format_start.line > INT32_MAX || format_start.utf16_column > INT32_MAX ||
             format_end.line > INT32_MAX || format_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->resolve_action_limit != 0U)
    {
        /* Advertise only the property this owner can resolve. Provider commands
         * remain unavailable; complete text edits still need explicit review. */
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":true,"
            "\"resolveSupport\":{\"properties\":[\"edit\"]},\"honorsChangeAnnotations\":true}");
        if (policy->diagnostic_context)
        {
            umi_language_runtime_json_writer_raw(
                &writer,
                policy->pull_diagnostics
                    ? ",\"diagnostic\":{\"dynamicRegistration\":false,\"relatedDocumentSupport\":false,"
                      "\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
                      "\"codeDescriptionSupport\":true,\"dataSupport\":true}"
                    : ",\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,"
                      "2]},"
                      "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":true}");
        }
        umi_language_runtime_json_writer_raw(&writer, "}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS && policy->workspace_symbol_query != NULL)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"symbol\":{"
            "\"dynamicRegistration\":false,\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,"
            "14,15,16,17,18,19,20,21,22,23,24,25,26]},\"tagSupport\":{\"valueSet\":[1]}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context &&
             policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CALL_HIERARCHY)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"callHierarchy\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"selectionRange\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
             policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_TYPE_DEFINITION
                         ? "\"typeDefinition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_IMPLEMENTATION
                         ? "\"implementation\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING && policy->range_formatting)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rangeFormatting\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1");
        if (policy->action_only_kind != NULL && policy->action_only_kind[0] != '\0')
        {
            umi_language_runtime_json_writer_raw(&writer, ",\"only\":[");
            umi_language_runtime_json_writer_string(&writer, policy->action_only_kind);
            umi_language_runtime_json_writer_raw(&writer, "]");
        }
        umi_language_runtime_json_writer_raw(&writer, "}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        /* The range guides the formatter; returned edits remain complete so
         * surrounding indentation changes can be reviewed rather than clipped. */
        if (policy->range_formatting)
        {
            umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}");
        }
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"positions\":[{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "}]}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        /* Workspace symbols take a query rather than a document identifier.
         * Build that independent request only after validating its full text. */
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"query\":");
        umi_language_runtime_json_writer_string(&writer, policy->workspace_symbol_query);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus QueryPrepare(const UmiLanguageCompletionRequest *request, const UmiCancellationToken *cancel,
                              const SourceQueryPolicy *policy, CompletionQueryWire **out)
{
    *out = NULL;
    if (request == NULL || request->source == NULL || request->root_uri == NULL ||
        request->document_uri == NULL || request->language_id == NULL || request->root_uri[0] == '\0' ||
        request->document_uri[0] == '\0' || request->language_id[0] == '\0' ||
        request->cursor_offset > request->source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (request->source_bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY ||
        strlen(request->root_uri) >= UMI_LANGUAGE_RUNTIME_PATH_CAPACITY ||
        strlen(request->document_uri) >= UMI_EDITOR_SOURCE_URI_CAPACITY ||
        strlen(request->language_id) >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status =
        policy->kind == SOURCE_QUERY_RENAME ? QueryRenameName(policy->rename_name) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        status = QueryText(request->source, request->source_bytes);
    if (status == UMI_STATUS_OK)
        status = QueryText(request->root_uri, strlen(request->root_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->document_uri, strlen(request->document_uri));
    if (status == UMI_STATUS_OK)
        status = QueryText(request->language_id, strlen(request->language_id));
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        size_t query_bytes = strlen(policy->workspace_symbol_query);
        status = query_bytes > 4096U ? UMI_STATUS_CAPACITY_EXCEEDED
                                     : QueryText(policy->workspace_symbol_query, query_bytes);
    }
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = request->source;
    view.byte_count = request->source_bytes;
    view.capacity = request->source_bytes;
    UmiEditorTextPosition range_end = {0};
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        if (policy->range_end < request->cursor_offset || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        else
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &range_end);
        if (status == UMI_STATUS_OK && (range_end.line > INT32_MAX || range_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition format_start = {0}, format_end = {0};
    if (status == UMI_STATUS_OK && policy->range_formatting)
    {
        if (policy->range_start > policy->range_end || policy->range_end > request->source_bytes)
            status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_start, &format_start);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, policy->range_end, &format_end);
        if (status == UMI_STATUS_OK &&
            (format_start.line > INT32_MAX || format_start.utf16_column > INT32_MAX ||
             format_end.line > INT32_MAX || format_end.utf16_column > INT32_MAX))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiEditorTextPosition cursor;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &cursor);
    if (status != UMI_STATUS_OK)
        return status;
    if (cursor.line > INT32_MAX || cursor.utf16_column > INT32_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    CompletionQueryWire *wire = calloc(1U, sizeof(*wire));
    char *copy = malloc(request->source_bytes + 1U);
    char *envelope = malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (wire == NULL || copy == NULL || envelope == NULL)
    {
        QueryWireDestroy(wire);
        free(copy);
        free(envelope);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, request->source, request->source_bytes);
    copy[request->source_bytes] = '\0';
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, wire->initialize, sizeof(wire->initialize));
    umi_language_runtime_json_writer_raw(
        &writer, "{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\"},\"rootUri\":");
    umi_language_runtime_json_writer_string(&writer, request->root_uri);
    /* Capabilities describe this workflow's actual editor contract. Existing
     * broader runtime initialization remains available to its other clients. */
    if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->resolve_action_limit != 0U)
    {
        /* Advertise only the property this owner can resolve. Provider commands
         * remain unavailable; complete text edits still need explicit review. */
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":true,"
            "\"resolveSupport\":{\"properties\":[\"edit\"]},\"honorsChangeAnnotations\":true}");
        if (policy->diagnostic_context)
        {
            umi_language_runtime_json_writer_raw(
                &writer,
                policy->pull_diagnostics
                    ? ",\"diagnostic\":{\"dynamicRegistration\":false,\"relatedDocumentSupport\":false,"
                      "\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
                      "\"codeDescriptionSupport\":true,\"dataSupport\":true}"
                    : ",\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,"
                      "2]},"
                      "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":true}");
        }
        umi_language_runtime_json_writer_raw(&writer, "}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS && policy->workspace_symbol_query != NULL)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"symbol\":{"
            "\"dynamicRegistration\":false,\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,"
            "14,15,16,17,18,19,20,21,22,23,24,25,26]},\"tagSupport\":{\"valueSet\":[1]}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context &&
             policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"diagnostics\":{\"refreshSupport\":false}},\"textDocument\":{\"synchronization\":{"
            "\"dynamicRegistration\":false,\"didSave\":false},\"diagnostic\":{\"dynamicRegistration\":false,"
            "\"relatedDocumentSupport\":false,\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]}"
            ","
            "\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},\"workspace\":{"
            "\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,\"workspaceEdit\":{"
            "\"documentChanges\":true,\"resourceOperations\":[],\"changeAnnotationSupport\":{"
            "\"groupsOnLabel\":false}}},\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,"
            "\"didSave\":false},\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{"
            "\"codeActionKind\":{\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\","
            "\"refactor.inline\",\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source."
            "fixAll\"]}},\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true},\"publishDiagnostics\":{\"relatedInformation\":true,"
            "\"tagSupport\":{\"valueSet\":[1,2]},\"versionSupport\":true,\"codeDescriptionSupport\":true,"
            "\"dataSupport\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DIAGNOSTICS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"publishDiagnostics\":{\"relatedInformation\":true,\"tagSupport\":{\"valueSet\":[1,2]},"
            "\"versionSupport\":true,\"codeDescriptionSupport\":true,\"dataSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"codeAction\":{\"dynamicRegistration\":false,\"codeActionLiteralSupport\":{\"codeActionKind\":{"
            "\"valueSet\":[\"\",\"quickfix\",\"refactor\",\"refactor.extract\",\"refactor.inline\","
            "\"refactor.rewrite\",\"source\",\"source.organizeImports\",\"source.fixAll\"]}},"
            "\"isPreferredSupport\":true,\"disabledSupport\":true,\"dataSupport\":false,"
            "\"honorsChangeAnnotations\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_RENAME)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false,"
            "\"workspaceEdit\":{\"documentChanges\":true,\"resourceOperations\":[],"
            "\"changeAnnotationSupport\":{\"groupsOnLabel\":false}}},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rename\":{\"dynamicRegistration\":false,\"prepareSupport\":true,\"honorsChangeAnnotations\":"
            "true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SIGNATURE)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"signatureHelp\":{\"dynamicRegistration\":false,\"contextSupport\":false,"
            "\"signatureInformation\":{\"documentationFormat\":[\"plaintext\",\"markdown\"],"
            "\"parameterInformation\":{\"labelOffsetSupport\":true},\"activeParameterSupport\":true}}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_CALL_HIERARCHY)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"callHierarchy\":{\"dynamicRegistration\":false}}}}");
    }
    else if(policy->kind==SOURCE_QUERY_FOLDING_RANGES)
    {
        /* The editor folds full lines and uses its own visible header, so do
         * not advertise custom collapsed text or character-level folding. */
        umi_language_runtime_json_writer_raw(&writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"foldingRange\":{\"dynamicRegistration\":false,\"rangeLimit\":4096,\"lineFoldingOnly\":true}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"selectionRange\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"documentSymbol\":{\"dynamicRegistration\":false,\"hierarchicalDocumentSymbolSupport\":true,"
            "\"symbolKind\":{\"valueSet\":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,"
            "26]},"
            "\"tagSupport\":{\"valueSet\":[1]},\"labelSupport\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
             policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},");
        umi_language_runtime_json_writer_raw(
            &writer, policy->kind == SOURCE_QUERY_TYPE_DEFINITION
                         ? "\"typeDefinition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_IMPLEMENTATION
                         ? "\"implementation\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                     : policy->kind == SOURCE_QUERY_DEFINITION
                         ? "\"definition\":{\"dynamicRegistration\":false,\"linkSupport\":true}}}}"
                         : "\"references\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_HOVER)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"hover\":{\"dynamicRegistration\":false,\"contentFormat\":[\"plaintext\"]}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING && policy->range_formatting)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"rangeFormatting\":{\"dynamicRegistration\":false}}}}");
    }
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"formatting\":{\"dynamicRegistration\":false}}}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(
            &writer,
            ",\"capabilities\":{\"general\":{\"positionEncodings\":[\"utf-16\"]},"
            "\"workspace\":{\"applyEdit\":false,\"configuration\":false,\"workspaceFolders\":false},"
            "\"textDocument\":{\"synchronization\":{\"dynamicRegistration\":false,\"didSave\":false},"
            "\"completion\":{\"dynamicRegistration\":false,\"completionItem\":{"
            "\"snippetSupport\":false,\"insertReplaceSupport\":true,\"insertTextModeSupport\":{\"valueSet\":["
            "1]}}"
            ","
            "\"completionList\":{\"itemDefaults\":[\"editRange\",\"insertTextFormat\",\"insertTextMode\"]}}}}"
            "}");
    }
    status = writer.status;
    umi_language_runtime_json_writer_init(&writer, wire->open, sizeof(wire->open));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, ",\"languageId\":");
    umi_language_runtime_json_writer_string(&writer, request->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"version\":1,\"text\":");
    umi_language_runtime_json_writer_string(&writer, copy);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    /* Preflight the full envelope too: an otherwise valid parameters object
     * can leave too little room for the method and JSON-RPC wrapper. */
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_build_notification("textDocument/didOpen", wire->open, envelope,
                                                         UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    if (policy->kind == SOURCE_QUERY_CODE_ACTION)
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, range_end.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "}},\"context\":{\"diagnostics\":[],\"triggerKind\":1");
        if (policy->action_only_kind != NULL && policy->action_only_kind[0] != '\0')
        {
            umi_language_runtime_json_writer_raw(&writer, ",\"only\":[");
            umi_language_runtime_json_writer_string(&writer, policy->action_only_kind);
            umi_language_runtime_json_writer_raw(&writer, "]");
        }
        umi_language_runtime_json_writer_raw(&writer, "}}");
    }
    else if (policy->kind == SOURCE_QUERY_SYMBOLS)
        umi_language_runtime_json_writer_raw(&writer, "}}");
    else if (policy->kind == SOURCE_QUERY_FORMATTING)
    {
        /* The range guides the formatter; returned edits remain complete so
         * surrounding indentation changes can be reviewed rather than clipped. */
        if (policy->range_formatting)
        {
            umi_language_runtime_json_writer_raw(&writer, "},\"range\":{\"start\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_start.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "},\"end\":{\"line\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.line);
            umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
            umi_language_runtime_json_writer_uint64(&writer, format_end.utf16_column);
            umi_language_runtime_json_writer_raw(&writer, "}");
        }
        umi_language_runtime_json_writer_raw(&writer, "},\"options\":{\"tabSize\":");
        umi_language_runtime_json_writer_uint64(&writer, policy->tab_size);
        umi_language_runtime_json_writer_raw(&writer, policy->insert_spaces ? ",\"insertSpaces\":true}}"
                                                                            : ",\"insertSpaces\":false}}");
    }
    else
    {
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        if (policy->kind == SOURCE_QUERY_REFERENCES)
            umi_language_runtime_json_writer_raw(
                &writer, policy->include_declaration ? "},\"context\":{\"includeDeclaration\":true}}"
                                                     : "},\"context\":{\"includeDeclaration\":false}}");
        else
            umi_language_runtime_json_writer_raw(&writer, policy->kind == SOURCE_QUERY_COMPLETION
                                                              ? "},\"context\":{\"triggerKind\":1}}"
                                                              : "}}");
    }
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if(status==UMI_STATUS_OK && policy->kind==SOURCE_QUERY_FOLDING_RANGES) {
        umi_language_runtime_json_writer_init(&writer,wire->position,sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer,"{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer,request->document_uri);
        umi_language_runtime_json_writer_raw(&writer,"}}");
        status=writer.status;
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"positions\":[{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "}]}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
    {
        /* Preparation uses only a position. The replacement name belongs to
         * the later rename request and must not leak into preparation fields. */
        memcpy(wire->prepare, wire->position, sizeof(wire->prepare));
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
        umi_language_runtime_json_writer_string(&writer, request->document_uri);
        umi_language_runtime_json_writer_raw(&writer, "},\"position\":{\"line\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.line);
        umi_language_runtime_json_writer_raw(&writer, ",\"character\":");
        umi_language_runtime_json_writer_uint64(&writer, cursor.utf16_column);
        umi_language_runtime_json_writer_raw(&writer, "},\"newName\":");
        umi_language_runtime_json_writer_string(&writer, policy->rename_name);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    if (status == UMI_STATUS_OK && policy->workspace_symbol_query != NULL)
    {
        /* Workspace symbols take a query rather than a document identifier.
         * Build that independent request only after validating its full text. */
        umi_language_runtime_json_writer_init(&writer, wire->position, sizeof(wire->position));
        umi_language_runtime_json_writer_raw(&writer, "{\"query\":");
        umi_language_runtime_json_writer_string(&writer, policy->workspace_symbol_query);
        umi_language_runtime_json_writer_raw(&writer, "}");
        status = writer.status;
    }
    umi_language_runtime_json_writer_init(&writer, wire->close, sizeof(wire->close));
    umi_language_runtime_json_writer_raw(&writer, "{\"textDocument\":{\"uri\":");
    umi_language_runtime_json_writer_string(&writer, request->document_uri);
    umi_language_runtime_json_writer_raw(&writer, "}}");
    if (status == UMI_STATUS_OK)
        status = writer.status;
    if (status == UMI_STATUS_OK)
        status = QueryPrepareSources(request, policy, cancel, wire);
    free(copy);
    free(envelope);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        QueryWireDestroy(wire);
        return status;
    }
    *out = wire;
    return UMI_STATUS_OK;
}
static UmiStatus QueryAwait(UmiLanguageRuntimeServer *server, uint64_t id, UmiClock *clock, uint64_t started,
                            uint32_t timeout, const UmiCancellationToken *cancel,
                            UmiLanguageRuntimeEnvelope *reply)
{
    unsigned messages = 0U;
    for (;;)
    {
        if (CompletionCancelled(cancel))
            return UMI_STATUS_CANCELLED;
        uint64_t elapsed = (clock->monotonic_nanoseconds(clock) - started) / 1000000U;
        /* A positive timeout belongs to the entire query, including time
         * already spent initializing. Do not renew it for a buffered reply. */
        if (timeout != 0U && elapsed >= timeout)
            return UMI_STATUS_TIMEOUT;
        uint32_t remaining = elapsed >= timeout ? 0U : timeout - (uint32_t)elapsed;
        UmiStatus status =
            umi_language_runtime_server_receive(server, remaining > 50U ? 50U : remaining, reply);
        if (status == UMI_STATUS_OK)
        {
            if (reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_REQUEST)
                return UMI_STATUS_NOT_IMPLEMENTED;
            if ((reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_RESPONSE ||
                 reply->kind == UMI_LANGUAGE_RUNTIME_MESSAGE_ERROR) &&
                reply->request_id == id)
                return UMI_STATUS_OK;
            if (++messages >= 4096U)
                return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        else if (status != UMI_STATUS_NOT_FOUND)
            return status;
        if (!umi_language_runtime_server_is_running(server))
            return UMI_STATUS_UNAVAILABLE;
        if ((clock->monotonic_nanoseconds(clock) - started) / 1000000U >= timeout)
            return UMI_STATUS_TIMEOUT;
        if (status == UMI_STATUS_NOT_FOUND)
            (void)clock->sleep_milliseconds(clock, 1U);
    }
}
/* Shared response validation now precedes operation-specific capability negotiation.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    UmiStatus status = UmiJsonTreeCreate(json, strlen(json), &limits, NULL, &tree);
    if (status != UMI_STATUS_OK)
        return status;
    int protocol, identity, result, error, capabilities, encoding, completion, sync;
    char text[16];
    int64_t id;
    status = CompletionMember(tree, 0, "jsonrpc", &protocol);
    if (status == UMI_STATUS_OK)
        status = CompletionText(tree, protocol, text, sizeof(text));
    if (status == UMI_STATUS_OK && strcmp(text, "2.0") != 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, 0, "id", &identity);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeInteger(tree, identity, &id);
    if (status == UMI_STATUS_OK && (id < 1 || (uint64_t)id != request_id))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, 0, "result", &result);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, 0, "error", &error);
    if (status == UMI_STATUS_OK && error >= 0)
    {
        status = result >= 0 ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_UNAVAILABLE;
    }
    if (status == UMI_STATUS_OK && result < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "completionProvider", &completion);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Hover accepts boolean or object capability declarations through the same validated initialization path.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities,
                                  policy->formatting ? "documentFormattingProvider" : "completionProvider",
                                  &completion);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int supported = 0;
        if (!policy->formatting || UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK ||
            !supported)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Definition and reference capabilities use the same validated initialization and synchronization contract.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities,
                                  policy->hover        ? "hoverProvider"
                                  : policy->formatting ? "documentFormattingProvider"
                                                       : "completionProvider",
                                  &completion);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int supported = 0;
        if ((!policy->formatting && !policy->hover) ||
            UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Outline requests require the advertised document-symbol provider before sending captured source.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities,
                                  policy->kind == SOURCE_QUERY_DEFINITION   ? "definitionProvider"
                                  : policy->kind == SOURCE_QUERY_REFERENCES ? "referencesProvider"
                                  : policy->kind == SOURCE_QUERY_HOVER      ? "hoverProvider"
                                  : policy->kind == SOURCE_QUERY_FORMATTING ? "documentFormattingProvider"
                                                                            : "completionProvider",
                                  &completion);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int supported = 0;
        if ((policy->kind == SOURCE_QUERY_COMPLETION) ||
            UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* A signature provider must advertise options before the temporary connection receives source.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities,
                                  policy->kind == SOURCE_QUERY_SYMBOLS      ? "documentSymbolProvider"
                                  : policy->kind == SOURCE_QUERY_DEFINITION ? "definitionProvider"
                                  : policy->kind == SOURCE_QUERY_REFERENCES ? "referencesProvider"
                                  : policy->kind == SOURCE_QUERY_HOVER      ? "hoverProvider"
                                  : policy->kind == SOURCE_QUERY_FORMATTING ? "documentFormattingProvider"
                                                                            : "completionProvider",
                                  &completion);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int supported = 0;
        if ((policy->kind == SOURCE_QUERY_COMPLETION) ||
            UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Rename negotiation records preparation support before sending source; malformed options cannot silently disable required preparation.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy)
{
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities,
                                  policy->kind == SOURCE_QUERY_SIGNATURE    ? "signatureHelpProvider"
                                  : policy->kind == SOURCE_QUERY_SYMBOLS    ? "documentSymbolProvider"
                                  : policy->kind == SOURCE_QUERY_DEFINITION ? "definitionProvider"
                                  : policy->kind == SOURCE_QUERY_REFERENCES ? "referencesProvider"
                                  : policy->kind == SOURCE_QUERY_HOVER      ? "hoverProvider"
                                  : policy->kind == SOURCE_QUERY_FORMATTING ? "documentFormattingProvider"
                                                                            : "completionProvider",
                                  &completion);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int supported = 0;
        if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
            UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Require the code-action provider before sending a captured draft; existing source tools retain their own negotiation.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities,
                                  policy->kind == SOURCE_QUERY_RENAME       ? "renameProvider"
                                  : policy->kind == SOURCE_QUERY_SIGNATURE  ? "signatureHelpProvider"
                                  : policy->kind == SOURCE_QUERY_SYMBOLS    ? "documentSymbolProvider"
                                  : policy->kind == SOURCE_QUERY_DEFINITION ? "definitionProvider"
                                  : policy->kind == SOURCE_QUERY_REFERENCES ? "referencesProvider"
                                  : policy->kind == SOURCE_QUERY_HOVER      ? "hoverProvider"
                                  : policy->kind == SOURCE_QUERY_FORMATTING ? "documentFormattingProvider"
                                                                            : "completionProvider",
                                  &completion);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int supported = 0;
        if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
            UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Push diagnostics use document synchronization and UTF-16 negotiation; the protocol has no publishDiagnosticsProvider switch to require.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities,
                                  policy->kind == SOURCE_QUERY_CODE_ACTION  ? "codeActionProvider"
                                  : policy->kind == SOURCE_QUERY_RENAME     ? "renameProvider"
                                  : policy->kind == SOURCE_QUERY_SIGNATURE  ? "signatureHelpProvider"
                                  : policy->kind == SOURCE_QUERY_SYMBOLS    ? "documentSymbolProvider"
                                  : policy->kind == SOURCE_QUERY_DEFINITION ? "definitionProvider"
                                  : policy->kind == SOURCE_QUERY_REFERENCES ? "referencesProvider"
                                  : policy->kind == SOURCE_QUERY_HOVER      ? "hoverProvider"
                                  : policy->kind == SOURCE_QUERY_FORMATTING ? "documentFormattingProvider"
                                                                            : "completionProvider",
                                  &completion);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int supported = 0;
        if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
            UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Select the actual project or document symbol capability; a document outline provider does not imply workspace search support.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (status == UMI_STATUS_OK)
            status = CompletionMember(tree, capabilities,
                                      policy->kind == SOURCE_QUERY_CODE_ACTION  ? "codeActionProvider"
                                      : policy->kind == SOURCE_QUERY_RENAME     ? "renameProvider"
                                      : policy->kind == SOURCE_QUERY_SIGNATURE  ? "signatureHelpProvider"
                                      : policy->kind == SOURCE_QUERY_SYMBOLS    ? "documentSymbolProvider"
                                      : policy->kind == SOURCE_QUERY_DEFINITION ? "definitionProvider"
                                      : policy->kind == SOURCE_QUERY_REFERENCES ? "referencesProvider"
                                      : policy->kind == SOURCE_QUERY_HOVER      ? "hoverProvider"
                                      : policy->kind == SOURCE_QUERY_FORMATTING ? "documentFormattingProvider"
                                                                                : "completionProvider",
                                      &completion);
        if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int supported = 0;
            if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
                UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Require the exact navigation provider selected by the caller, preventing a supported definition provider from masking an unsupported type or implementation search.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (status == UMI_STATUS_OK)
            status =
                CompletionMember(tree, capabilities,
                                 policy->kind == SOURCE_QUERY_CODE_ACTION ? "codeActionProvider"
                                 : policy->kind == SOURCE_QUERY_RENAME    ? "renameProvider"
                                 : policy->kind == SOURCE_QUERY_SIGNATURE ? "signatureHelpProvider"
                                 : policy->kind == SOURCE_QUERY_SYMBOLS
                                     ? (policy->workspace_symbol_query != NULL ? "workspaceSymbolProvider"
                                                                               : "documentSymbolProvider")
                                 : policy->kind == SOURCE_QUERY_DEFINITION ? "definitionProvider"
                                 : policy->kind == SOURCE_QUERY_REFERENCES ? "referencesProvider"
                                 : policy->kind == SOURCE_QUERY_HOVER      ? "hoverProvider"
                                 : policy->kind == SOURCE_QUERY_FORMATTING ? "documentFormattingProvider"
                                                                           : "completionProvider",
                                 &completion);
        if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int supported = 0;
            if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
                UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Require the chosen formatting provider rather than silently widening a range request to the complete document.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (status == UMI_STATUS_OK)
            status =
                CompletionMember(tree, capabilities,
                                 policy->kind == SOURCE_QUERY_CODE_ACTION ? "codeActionProvider"
                                 : policy->kind == SOURCE_QUERY_RENAME    ? "renameProvider"
                                 : policy->kind == SOURCE_QUERY_SIGNATURE ? "signatureHelpProvider"
                                 : policy->kind == SOURCE_QUERY_SYMBOLS
                                     ? (policy->workspace_symbol_query != NULL ? "workspaceSymbolProvider"
                                                                               : "documentSymbolProvider")
                                 : policy->kind == SOURCE_QUERY_TYPE_DEFINITION ? "typeDefinitionProvider"
                                 : policy->kind == SOURCE_QUERY_IMPLEMENTATION  ? "implementationProvider"
                                 : policy->kind == SOURCE_QUERY_DEFINITION      ? "definitionProvider"
                                 : policy->kind == SOURCE_QUERY_REFERENCES      ? "referencesProvider"
                                 : policy->kind == SOURCE_QUERY_HOVER           ? "hoverProvider"
                                 : policy->kind == SOURCE_QUERY_FORMATTING      ? "documentFormattingProvider"
                                                                                : "completionProvider",
                                 &completion);
        if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int supported = 0;
            if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
                UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Require selection-range support independently of definitions, references and formatting.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (status == UMI_STATUS_OK)
            status =
                CompletionMember(tree, capabilities,
                                 policy->kind == SOURCE_QUERY_CODE_ACTION ? "codeActionProvider"
                                 : policy->kind == SOURCE_QUERY_RENAME    ? "renameProvider"
                                 : policy->kind == SOURCE_QUERY_SIGNATURE ? "signatureHelpProvider"
                                 : policy->kind == SOURCE_QUERY_SYMBOLS
                                     ? (policy->workspace_symbol_query != NULL ? "workspaceSymbolProvider"
                                                                               : "documentSymbolProvider")
                                 : policy->kind == SOURCE_QUERY_TYPE_DEFINITION ? "typeDefinitionProvider"
                                 : policy->kind == SOURCE_QUERY_IMPLEMENTATION  ? "implementationProvider"
                                 : policy->kind == SOURCE_QUERY_DEFINITION      ? "definitionProvider"
                                 : policy->kind == SOURCE_QUERY_REFERENCES      ? "referencesProvider"
                                 : policy->kind == SOURCE_QUERY_HOVER           ? "hoverProvider"
                                 : policy->kind == SOURCE_QUERY_FORMATTING
                                     ? (policy->range_formatting ? "documentRangeFormattingProvider"
                                                                 : "documentFormattingProvider")
                                     : "completionProvider",
                                 &completion);
        if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int supported = 0;
            if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
                UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Require explicit server support before opening drafts for deferred edit resolution.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (status == UMI_STATUS_OK)
            status =
                CompletionMember(tree, capabilities,
                                 policy->kind == SOURCE_QUERY_CODE_ACTION ? "codeActionProvider"
                                 : policy->kind == SOURCE_QUERY_RENAME    ? "renameProvider"
                                 : policy->kind == SOURCE_QUERY_SIGNATURE ? "signatureHelpProvider"
                                 : policy->kind == SOURCE_QUERY_SYMBOLS
                                     ? (policy->workspace_symbol_query != NULL ? "workspaceSymbolProvider"
                                                                               : "documentSymbolProvider")
                                 : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "typeDefinitionProvider"
                                 : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "selectionRangeProvider"
                                 : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "implementationProvider"
                                 : policy->kind == SOURCE_QUERY_DEFINITION       ? "definitionProvider"
                                 : policy->kind == SOURCE_QUERY_REFERENCES       ? "referencesProvider"
                                 : policy->kind == SOURCE_QUERY_HOVER            ? "hoverProvider"
                                 : policy->kind == SOURCE_QUERY_FORMATTING
                                     ? (policy->range_formatting ? "documentRangeFormattingProvider"
                                                                 : "documentFormattingProvider")
                                     : "completionProvider",
                                 &completion);
        if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int supported = 0;
            if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
                UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Require the call hierarchy provider before opening source for caller or callee inspection.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (status == UMI_STATUS_OK)
            status =
                CompletionMember(tree, capabilities,
                                 policy->kind == SOURCE_QUERY_CODE_ACTION ? "codeActionProvider"
                                 : policy->kind == SOURCE_QUERY_RENAME    ? "renameProvider"
                                 : policy->kind == SOURCE_QUERY_SIGNATURE ? "signatureHelpProvider"
                                 : policy->kind == SOURCE_QUERY_SYMBOLS
                                     ? (policy->workspace_symbol_query != NULL ? "workspaceSymbolProvider"
                                                                               : "documentSymbolProvider")
                                 : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "typeDefinitionProvider"
                                 : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "selectionRangeProvider"
                                 : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "implementationProvider"
                                 : policy->kind == SOURCE_QUERY_DEFINITION       ? "definitionProvider"
                                 : policy->kind == SOURCE_QUERY_REFERENCES       ? "referencesProvider"
                                 : policy->kind == SOURCE_QUERY_HOVER            ? "hoverProvider"
                                 : policy->kind == SOURCE_QUERY_FORMATTING
                                     ? (policy->range_formatting ? "documentRangeFormattingProvider"
                                                                 : "documentFormattingProvider")
                                     : "completionProvider",
                                 &completion);
        if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int supported = 0;
            if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
                UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK && policy->resolve_action_limit != 0U)
    {
        int resolver = -1, enabled = 0;
        if (UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        else
            status = CompletionMember(tree, completion, "resolveProvider", &resolver);
        if (status == UMI_STATUS_OK && resolver < 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeBoolean(tree, resolver, &enabled);
        if (status == UMI_STATUS_OK && !enabled)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
/* Require the folding provider capability independently of ordinary navigation support.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (status == UMI_STATUS_OK)
            status =
                CompletionMember(tree, capabilities,
                                 policy->kind == SOURCE_QUERY_CALL_HIERARCHY ? "callHierarchyProvider"
                                 : policy->kind == SOURCE_QUERY_CODE_ACTION  ? "codeActionProvider"
                                 : policy->kind == SOURCE_QUERY_RENAME       ? "renameProvider"
                                 : policy->kind == SOURCE_QUERY_SIGNATURE    ? "signatureHelpProvider"
                                 : policy->kind == SOURCE_QUERY_SYMBOLS
                                     ? (policy->workspace_symbol_query != NULL ? "workspaceSymbolProvider"
                                                                               : "documentSymbolProvider")
                                 : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "typeDefinitionProvider"
                                 : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "selectionRangeProvider"
                                 : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "implementationProvider"
                                 : policy->kind == SOURCE_QUERY_DEFINITION       ? "definitionProvider"
                                 : policy->kind == SOURCE_QUERY_REFERENCES       ? "referencesProvider"
                                 : policy->kind == SOURCE_QUERY_HOVER            ? "hoverProvider"
                                 : policy->kind == SOURCE_QUERY_FORMATTING
                                     ? (policy->range_formatting ? "documentRangeFormattingProvider"
                                                                 : "documentFormattingProvider")
                                     : "completionProvider",
                                 &completion);
        if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int supported = 0;
            if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
                UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK && policy->resolve_action_limit != 0U)
    {
        int resolver = -1, enabled = 0;
        if (UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        else
            status = CompletionMember(tree, completion, "resolveProvider", &resolver);
        if (status == UMI_STATUS_OK && resolver < 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeBoolean(tree, resolver, &enabled);
        if (status == UMI_STATUS_OK && !enabled)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
#endif
static UmiStatus QueryCapabilities(const char *json, uint64_t request_id, const SourceQueryPolicy *policy,
                                   int *prepare_rename)
{
    *prepare_rename = 0;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1, capabilities, encoding, completion, sync;
    char text[16];
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, strlen(json), request_id, &limits, NULL, &tree, &result);
    if (status != UMI_STATUS_OK)
        return status;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, result, "capabilities", &capabilities);
    if (status == UMI_STATUS_OK && capabilities < 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "positionEncoding", &encoding);
    if (status == UMI_STATUS_OK && encoding >= 0)
    {
        status = CompletionText(tree, encoding, text, sizeof(text));
        if (status == UMI_STATUS_OK && strcmp(text, "utf-16") != 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (status == UMI_STATUS_OK)
            status =
                CompletionMember(tree, capabilities,
                                 policy->kind == SOURCE_QUERY_CALL_HIERARCHY ? "callHierarchyProvider"
                                 : policy->kind == SOURCE_QUERY_CODE_ACTION  ? "codeActionProvider"
                                 : policy->kind == SOURCE_QUERY_RENAME       ? "renameProvider"
                                 : policy->kind == SOURCE_QUERY_SIGNATURE    ? "signatureHelpProvider"
                                 : policy->kind == SOURCE_QUERY_SYMBOLS
                                     ? (policy->workspace_symbol_query != NULL ? "workspaceSymbolProvider"
                                                                               : "documentSymbolProvider")
                                 : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "typeDefinitionProvider"
                                 : policy->kind == SOURCE_QUERY_FOLDING_RANGES ? "foldingRangeProvider"
 : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "selectionRangeProvider"
                                 : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "implementationProvider"
                                 : policy->kind == SOURCE_QUERY_DEFINITION       ? "definitionProvider"
                                 : policy->kind == SOURCE_QUERY_REFERENCES       ? "referencesProvider"
                                 : policy->kind == SOURCE_QUERY_HOVER            ? "hoverProvider"
                                 : policy->kind == SOURCE_QUERY_FORMATTING
                                     ? (policy->range_formatting ? "documentRangeFormattingProvider"
                                                                 : "documentFormattingProvider")
                                     : "completionProvider",
                                 &completion);
        if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int supported = 0;
            if ((policy->kind == SOURCE_QUERY_COMPLETION || policy->kind == SOURCE_QUERY_SIGNATURE) ||
                UmiJsonTreeBoolean(tree, completion, &supported) != UMI_STATUS_OK || !supported)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME &&
        UmiJsonTreeKind(tree, completion) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    {
        int prepare = -1;
        status = CompletionMember(tree, completion, "prepareProvider", &prepare);
        if (status == UMI_STATUS_OK && prepare >= 0)
            status = UmiJsonTreeBoolean(tree, prepare, prepare_rename);
    }
    if (status == UMI_STATUS_OK && policy->resolve_action_limit != 0U)
    {
        int resolver = -1, enabled = 0;
        if (UmiJsonTreeKind(tree, completion) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        else
            status = CompletionMember(tree, completion, "resolveProvider", &resolver);
        if (status == UMI_STATUS_OK && resolver < 0)
            status = UMI_STATUS_NOT_IMPLEMENTED;
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeBoolean(tree, resolver, &enabled);
        if (status == UMI_STATUS_OK && !enabled)
            status = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, capabilities, "textDocumentSync", &sync);
    if (status == UMI_STATUS_OK)
    {
        if (UmiJsonTreeKind(tree, sync) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        {
            int open_close, enabled = 0;
            status = CompletionMember(tree, sync, "openClose", &open_close);
            if (status == UMI_STATUS_OK && open_close < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (status == UMI_STATUS_OK)
                status = UmiJsonTreeBoolean(tree, open_close, &enabled);
            if (status == UMI_STATUS_OK && !enabled)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else
        {
            int64_t kind = 0;
            status = UmiJsonTreeInteger(tree, sync, &kind);
            if (status == UMI_STATUS_OK && kind != 1 && kind != 2)
                status = UMI_STATUS_NOT_IMPLEMENTED;
            if (sync < 0)
                status = UMI_STATUS_NOT_IMPLEMENTED;
        }
    }
    UmiJsonTreeDestroy(tree);
    return status;
}
/* Completion and formatting now share initialization, request lifetime and cleanup ownership.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report,
                          UmiLanguageCompletionCatalogue **out_catalogue)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiLanguageCompletionCatalogue *catalogue = NULL;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = umi_language_runtime_server_send_request(server, "textDocument/completion",
                                                              wire->position, "completion-source", &id);
    }
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                            &catalogue);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(catalogue);
        return status;
    }
    report->choices = UmiLanguageCompletionCatalogueCount(catalogue);
    *out_catalogue = catalogue;
    return UMI_STATUS_OK;
}
#endif
/* Read-only source information shares transport cleanup but never constructs an edit proposal.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = umi_language_runtime_server_send_request(
                server, policy->formatting ? "textDocument/formatting" : "textDocument/completion",
                wire->position, policy->formatting ? "formatting-source" : "completion-source", &id);
    }
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
    {
        if (policy->formatting)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        return status;
    }
    report->choices = policy->formatting ? UmiLanguageTextEditPreviewCount(result.formatting)
                                         : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif

/* Location parsing never opens a URI. Only ranges naming this captured source
 * or its origin can be checked here; a destination host checks other drafts. */
static UmiStatus QueryLocationRange(const UmiEditorTextPositionIndex *index, UmiLanguageSourceRange range)
{
    size_t offset = 0U;
    UmiStatus status = UmiEditorTextPositionIndexResolve(index, range.start, &offset);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextPositionIndexResolve(index, range.end, &offset);
    return status;
}
/* Decode complete enclosing selection chains before reusing exact captured-source endpoint validation for all navigation results.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryLocations(const char *json, uint64_t id, const UmiLanguageCompletionRequest *request,
                                const SourceQueryPolicy *policy, const UmiCancellationToken *cancel,
                                UmiLanguageLocationCatalogue **out)
{
    *out = NULL;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
    int result = -1;
    UmiStatus status = UmiLanguageResponseTreeRead(json, strlen(json), id, &limits, cancel, &tree, &result);
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_REFERENCES &&
        !UmiJsonTreeIsNull(tree, result) && UmiJsonTreeKind(tree, result) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    const char *value = NULL;
    size_t bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &value, &bytes);
    UmiLanguageLocationCatalogue *locations = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageLocationCatalogueCreate(value, bytes, cancel, &locations);
    UmiJsonTreeDestroy(tree);
    UmiEditorTextPositionIndex *index = NULL;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiLanguageLocationCatalogueCount(locations); ++i)
    {
        UmiLanguageSourceLocation location;
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = UmiLanguageLocationCatalogueAt(locations, i, &location);
        if (status != UMI_STATUS_OK)
            break;
        if (policy->kind == SOURCE_QUERY_REFERENCES && location.is_link)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        int same = strcmp(location.uri, request->document_uri) == 0;
        if ((same || location.has_origin) && index == NULL)
        {
            UmiEditorTextBufferView view = {0};
            view.struct_size = (uint32_t)sizeof(view);
            view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
            view.bytes = request->source;
            view.byte_count = view.capacity = request->source_bytes;
            status = UmiEditorTextPositionIndexCreate(&view, cancel, &index);
        }
        if (status == UMI_STATUS_OK && location.has_origin)
            status = QueryLocationRange(index, location.origin);
        if (status == UMI_STATUS_OK && same)
            status = QueryLocationRange(index, location.target);
        if (status == UMI_STATUS_OK && same)
            status = QueryLocationRange(index, location.selection);
    }
    UmiEditorTextPositionIndexDestroy(index);
    if (status == UMI_STATUS_OK)
        *out = locations;
    else
        UmiLanguageLocationCatalogueDestroy(locations);
    return status;
}
#endif
/* Reject incomplete folding responses and out-of-source lines before reusing exact navigation endpoint checks.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryLocations(const char *json, uint64_t id, const UmiLanguageCompletionRequest *request,
                                const SourceQueryPolicy *policy, const UmiCancellationToken *cancel,
                                UmiLanguageLocationCatalogue **out)
{
    *out = NULL;
    UmiLanguageLocationCatalogue *locations = NULL;
    UmiStatus status = UMI_STATUS_OK;
    if (policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        UmiEditorTextBufferView view = {0};
        view.struct_size = (uint32_t)sizeof(view);
        view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
        view.bytes = request->source;
        view.byte_count = view.capacity = request->source_bytes;
        UmiEditorTextPosition position;
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &position);
        if (status == UMI_STATUS_OK)
            status = UmiLanguageLocationCatalogueReadSelectionResponse(
                json, strlen(json), id, request->document_uri, position, cancel, &locations);
    }
    else
    {
        UmiJsonTree *tree = NULL;
        UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
        int result = -1;
        status = UmiLanguageResponseTreeRead(json, strlen(json), id, &limits, cancel, &tree, &result);
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_REFERENCES &&
            !UmiJsonTreeIsNull(tree, result) &&
            UmiJsonTreeKind(tree, result) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        const char *value = NULL;
        size_t bytes = 0U;
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeSourceSpan(tree, result, &value, &bytes);
        if (status == UMI_STATUS_OK)
            status = UmiLanguageLocationCatalogueCreate(value, bytes, cancel, &locations);
        UmiJsonTreeDestroy(tree);
    }
    UmiEditorTextPositionIndex *index = NULL;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiLanguageLocationCatalogueCount(locations); ++i)
    {
        UmiLanguageSourceLocation location;
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = UmiLanguageLocationCatalogueAt(locations, i, &location);
        if (status != UMI_STATUS_OK)
            break;
        if (policy->kind == SOURCE_QUERY_REFERENCES && location.is_link)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        int same = strcmp(location.uri, request->document_uri) == 0;
        if ((same || location.has_origin) && index == NULL)
        {
            UmiEditorTextBufferView view = {0};
            view.struct_size = (uint32_t)sizeof(view);
            view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
            view.bytes = request->source;
            view.byte_count = view.capacity = request->source_bytes;
            status = UmiEditorTextPositionIndexCreate(&view, cancel, &index);
        }
        if (status == UMI_STATUS_OK && location.has_origin)
            status = QueryLocationRange(index, location.origin);
        if (status == UMI_STATUS_OK && same)
            status = QueryLocationRange(index, location.target);
        if (status == UMI_STATUS_OK && same)
            status = QueryLocationRange(index, location.selection);
    }
    UmiEditorTextPositionIndexDestroy(index);
    if (status == UMI_STATUS_OK)
        *out = locations;
    else
        UmiLanguageLocationCatalogueDestroy(locations);
    return status;
}
#endif
static UmiStatus QueryLocations(const char *json, uint64_t id, const UmiLanguageCompletionRequest *request,
                                const SourceQueryPolicy *policy, const UmiCancellationToken *cancel,
                                UmiLanguageLocationCatalogue **out)
{
    *out = NULL;
    UmiLanguageLocationCatalogue *locations = NULL;
    UmiStatus status = UMI_STATUS_OK;
    if(policy->kind==SOURCE_QUERY_FOLDING_RANGES) {
        status=UmiLanguageLocationCatalogueReadFoldingResponse(json,strlen(json),id,request->document_uri,
            request->source,request->source_bytes,cancel,&locations);
    }
    else if (policy->kind == SOURCE_QUERY_SELECTION_RANGES)
    {
        UmiEditorTextBufferView view = {0};
        view.struct_size = (uint32_t)sizeof(view);
        view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
        view.bytes = request->source;
        view.byte_count = view.capacity = request->source_bytes;
        UmiEditorTextPosition position;
        status = UmiEditorTextViewPositionAt(&view, request->cursor_offset, &position);
        if (status == UMI_STATUS_OK)
            status = UmiLanguageLocationCatalogueReadSelectionResponse(
                json, strlen(json), id, request->document_uri, position, cancel, &locations);
    }
    else
    {
        UmiJsonTree *tree = NULL;
        UmiJsonTreeLimits limits = {UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, 16384U, 64U};
        int result = -1;
        status = UmiLanguageResponseTreeRead(json, strlen(json), id, &limits, cancel, &tree, &result);
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_REFERENCES &&
            !UmiJsonTreeIsNull(tree, result) &&
            UmiJsonTreeKind(tree, result) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        const char *value = NULL;
        size_t bytes = 0U;
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeSourceSpan(tree, result, &value, &bytes);
        if (status == UMI_STATUS_OK)
            status = UmiLanguageLocationCatalogueCreate(value, bytes, cancel, &locations);
        UmiJsonTreeDestroy(tree);
    }
    UmiEditorTextPositionIndex *index = NULL;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiLanguageLocationCatalogueCount(locations); ++i)
    {
        UmiLanguageSourceLocation location;
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = UmiLanguageLocationCatalogueAt(locations, i, &location);
        if (status != UMI_STATUS_OK)
            break;
        if (policy->kind == SOURCE_QUERY_REFERENCES && location.is_link)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        int same = strcmp(location.uri, request->document_uri) == 0;
        if ((same || location.has_origin) && index == NULL)
        {
            UmiEditorTextBufferView view = {0};
            view.struct_size = (uint32_t)sizeof(view);
            view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
            view.bytes = request->source;
            view.byte_count = view.capacity = request->source_bytes;
            status = UmiEditorTextPositionIndexCreate(&view, cancel, &index);
        }
        if (status == UMI_STATUS_OK && location.has_origin)
            status = QueryLocationRange(index, location.origin);
        if (status == UMI_STATUS_OK && same)
            status = QueryLocationRange(index, location.target);
        if (status == UMI_STATUS_OK && same)
            status = QueryLocationRange(index, location.selection);
    }
    UmiEditorTextPositionIndexDestroy(index);
    if (status == UMI_STATUS_OK)
        *out = locations;
    else
        UmiLanguageLocationCatalogueDestroy(locations);
    return status;
}
/* Source navigation decodes a complete owned catalogue and validates captured ranges before shared cleanup.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status =
                umi_language_runtime_server_send_request(server,
                                                         policy->hover        ? "textDocument/hover"
                                                         : policy->formatting ? "textDocument/formatting"
                                                                              : "textDocument/completion",
                                                         wire->position,
                                                         policy->hover        ? "source-information"
                                                         : policy->formatting ? "formatting-source"
                                                                              : "completion-source",
                                                         &id);
    }
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
    {
        if (policy->hover)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->formatting)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        return status;
    }
    report->choices = policy->hover        ? UmiLanguageHoverDocumentCount(result.hover)
                      : policy->formatting ? UmiLanguageTextEditPreviewCount(result.formatting)
                                           : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif

/* Resolve all same-document symbol spans against one immutable coordinate
 * index. A different URI stays unvisited until an explicit host navigation. */
/* Project symbol results reuse owned flat locations and exact primary-source coordinates, while refusing hierarchical replies or incomplete locations.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QuerySymbols(const char *json, uint64_t id, const UmiLanguageCompletionRequest *request,
                              const UmiCancellationToken *cancel, UmiLanguageSymbolCatalogue **out)
{
    *out = NULL;
    UmiLanguageSymbolCatalogue *symbols = NULL;
    UmiStatus status = UmiLanguageSymbolCatalogueReadResponse(json, strlen(json), id, request->document_uri,
                                                              cancel, &symbols);
    UmiEditorTextPositionIndex *index = NULL;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiLanguageSymbolCatalogueCount(symbols); ++i)
    {
        UmiLanguageSymbol symbol;
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = UmiLanguageSymbolCatalogueAt(symbols, i, &symbol);
        if (status != UMI_STATUS_OK)
            break;
        if (strcmp(symbol.location.uri, request->document_uri) != 0)
            continue;
        if (index == NULL)
        {
            UmiEditorTextBufferView view = {0};
            view.struct_size = (uint32_t)sizeof(view);
            view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
            view.bytes = request->source;
            view.byte_count = view.capacity = request->source_bytes;
            status = UmiEditorTextPositionIndexCreate(&view, cancel, &index);
        }
        if (status == UMI_STATUS_OK)
            status = QueryLocationRange(index, symbol.location.target);
        if (status == UMI_STATUS_OK)
            status = QueryLocationRange(index, symbol.location.selection);
    }
    UmiEditorTextPositionIndexDestroy(index);
    if (status == UMI_STATUS_OK)
        *out = symbols;
    else
        UmiLanguageSymbolCatalogueDestroy(symbols);
    return status;
}
#endif
static UmiStatus QuerySymbols(const char *json, uint64_t id, const UmiLanguageCompletionRequest *request,
                              const UmiCancellationToken *cancel, int workspace,
                              UmiLanguageSymbolCatalogue **out)
{
    *out = NULL;
    UmiLanguageSymbolCatalogue *symbols = NULL;
    UmiStatus status = UmiLanguageSymbolCatalogueReadResponse(json, strlen(json), id, request->document_uri,
                                                              cancel, &symbols);
    UmiEditorTextPositionIndex *index = NULL;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < UmiLanguageSymbolCatalogueCount(symbols); ++i)
    {
        UmiLanguageSymbol symbol;
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        status = UmiLanguageSymbolCatalogueAt(symbols, i, &symbol);
        if (status != UMI_STATUS_OK)
            break;
        /* Workspace results must provide explicit flat locations. A hierarchy
         * cannot borrow the primary URI and pretend it is a project result. */
        if (workspace && symbol.hierarchical)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        if (strcmp(symbol.location.uri, request->document_uri) != 0)
            continue;
        if (index == NULL)
        {
            UmiEditorTextBufferView view = {0};
            view.struct_size = (uint32_t)sizeof(view);
            view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
            view.bytes = request->source;
            view.byte_count = view.capacity = request->source_bytes;
            status = UmiEditorTextPositionIndexCreate(&view, cancel, &index);
        }
        if (status == UMI_STATUS_OK)
            status = QueryLocationRange(index, symbol.location.target);
        if (status == UMI_STATUS_OK)
            status = QueryLocationRange(index, symbol.location.selection);
    }
    UmiEditorTextPositionIndexDestroy(index);
    if (status == UMI_STATUS_OK)
        *out = symbols;
    else
        UmiLanguageSymbolCatalogueDestroy(symbols);
    return status;
}
/* Symbol outlines validate complete ranges and retain owned hierarchy through the shared close/shutdown boundary.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_DEFINITION   ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                wire->position,
                policy->kind == SOURCE_QUERY_DEFINITION   ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
    {
        if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        return status;
    }
    report->choices = result.locations != NULL ? UmiLanguageLocationCatalogueCount(result.locations)
                      : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
                      : policy->kind == SOURCE_QUERY_FORMATTING
                          ? UmiLanguageTextEditPreviewCount(result.formatting)
                          : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Complete overload results use the shared cleanup boundary, so failed closure never publishes a usable result.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_SYMBOLS      ? "textDocument/documentSymbol"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                wire->position,
                policy->kind == SOURCE_QUERY_SYMBOLS      ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
    {
        if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        return status;
    }
    report->choices = result.symbols != NULL     ? UmiLanguageSymbolCatalogueCount(result.symbols)
                      : result.locations != NULL ? UmiLanguageLocationCatalogueCount(result.locations)
                      : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
                      : policy->kind == SOURCE_QUERY_FORMATTING
                          ? UmiLanguageTextEditPreviewCount(result.formatting)
                          : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Rename preparation and proposals share one elapsed read budget; source checks and connection cleanup precede publication.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_SIGNATURE    ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "textDocument/documentSymbol"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                wire->position,
                policy->kind == SOURCE_QUERY_SIGNATURE    ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
    {
        if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        return status;
    }
    report->choices = result.signatures != NULL  ? UmiLanguageSignatureCatalogueCount(result.signatures)
                      : result.symbols != NULL   ? UmiLanguageSymbolCatalogueCount(result.symbols)
                      : result.locations != NULL ? UmiLanguageLocationCatalogueCount(result.locations)
                      : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
                      : policy->kind == SOURCE_QUERY_FORMATTING
                          ? UmiLanguageTextEditPreviewCount(result.formatting)
                          : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Keep action proposals intact through connection cleanup; an inspected action is not an executable command or an approved edit.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    int prepare_rename = 0;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_RENAME       ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "textDocument/documentSymbol"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                wire->position,
                policy->kind == SOURCE_QUERY_RENAME       ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
    {
        if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        return status;
    }
    report->choices =
        result.workspace_edits != NULL       ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
#include "diagnostic_query_support.inc"
#include "diagnostic_pull_query.inc"
#include "action_diagnostics_query.inc"
#include "action_resolution_query.inc"
#include "call_query_support.inc"

/* Wait for a source-matched diagnostic notification after didOpen, then use the same close/shutdown ownership gate as other source tools.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    int prepare_rename = 0;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME     ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "textDocument/documentSymbol"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                wire->position,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME     ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        return status;
    }
    report->choices =
        result.actions != NULL               ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Diagnostic-backed actions wait for current source analysis before requesting fixes, while retaining one total read budget and one cleanup owner.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    int prepare_rename = 0;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME     ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "textDocument/documentSymbol"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                wire->position,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME     ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = QueryAwaitDiagnostics(server, request, &clock, started, cancel, reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Synchronize all supplied drafts before rename and balance every successful open before publishing a workspace proposal.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        if (policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME     ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "textDocument/documentSymbol"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME     ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = QueryAwaitDiagnostics(server, request, &clock, started, cancel, reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Pull and notification diagnostics share captured-source and shutdown ownership, while explicit provider negotiation selects the correct wire request.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME     ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "textDocument/documentSymbol"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME     ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = QueryAwaitDiagnostics(server, request, &clock, started, cancel, reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Negotiate pull diagnostic identity before opening sources and retain that same connection through diagnostic context and action response cleanup.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME     ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "textDocument/documentSymbol"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME     ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Send project symbol searches through the same correlated response and close/shutdown ownership boundary as document outlines.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, wire, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME     ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "textDocument/documentSymbol"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME     ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Route each navigation intent to its own server method and validate complete owned results before closing the source connection.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, wire, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME    ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS
                    ? (policy->workspace_symbol_query != NULL ? "workspace/symbol"
                                                              : "textDocument/documentSymbol")
                : policy->kind == SOURCE_QUERY_DEFINITION ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "textDocument/formatting"
                                                          : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION  ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME     ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING ? "formatting-source"
                                                          : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, policy->workspace_symbol_query != NULL,
                                  &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Range formatting reuses complete edit validation, preview ownership and close/shutdown gates; the method reflects the captured scope.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, wire, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME    ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS
                    ? (policy->workspace_symbol_query != NULL ? "workspace/symbol"
                                                              : "textDocument/documentSymbol")
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION ? "textDocument/typeDefinition"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION  ? "textDocument/implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION      ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES      ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER           ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING      ? "textDocument/formatting"
                                                               : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION       ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME          ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE       ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS         ? "source-symbols"
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION ? "source-type-definition"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION  ? "source-implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION      ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES      ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER           ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING      ? "formatting-source"
                                                               : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, policy->workspace_symbol_query != NULL,
                                  &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
                 policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Apply exact action-family filtering after complete response validation and before publishing the cleanup-gated catalogue.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, wire, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME    ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS
                    ? (policy->workspace_symbol_query != NULL ? "workspace/symbol"
                                                              : "textDocument/documentSymbol")
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION ? "textDocument/typeDefinition"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION  ? "textDocument/implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION      ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES      ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER           ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING
                    ? (policy->range_formatting ? "textDocument/rangeFormatting" : "textDocument/formatting")
                    : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION       ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME          ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE       ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS         ? "source-symbols"
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION ? "source-type-definition"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION  ? "source-implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION      ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES      ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER           ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING      ? "formatting-source"
                                                               : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, policy->workspace_symbol_query != NULL,
                                  &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
                 policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Route syntax-selection requests to their own method and withhold the complete chain until source closure and shutdown succeed.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, wire, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME    ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS
                    ? (policy->workspace_symbol_query != NULL ? "workspace/symbol"
                                                              : "textDocument/documentSymbol")
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION ? "textDocument/typeDefinition"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION  ? "textDocument/implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION      ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES      ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER           ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING
                    ? (policy->range_formatting ? "textDocument/rangeFormatting" : "textDocument/formatting")
                    : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION       ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME          ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE       ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS         ? "source-symbols"
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION ? "source-type-definition"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION  ? "source-implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION      ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES      ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER           ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING      ? "formatting-source"
                                                               : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, policy->workspace_symbol_query != NULL,
                                  &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
                 policy->kind == SOURCE_QUERY_TYPE_DEFINITION || policy->kind == SOURCE_QUERY_IMPLEMENTATION)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION &&
        policy->action_filter != UMI_LANGUAGE_ACTION_FILTER_ALL)
    {
        /* Parse every action first, then keep complete matching proposals. A
         * malformed unrelated row is not hidden by the presentation filter. */
        UmiLanguageCodeActionCatalogue *filtered = NULL;
        status =
            UmiLanguageCodeActionCatalogueFilter(result.actions, policy->action_filter, cancel, &filtered);
        if (status == UMI_STATUS_OK)
        {
            UmiLanguageCodeActionCatalogueDestroy(result.actions);
            result.actions = filtered;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Resolve filtered deferred actions inside their original server session and withhold all results until resolution and cleanup succeed.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, wire, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME    ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS
                    ? (policy->workspace_symbol_query != NULL ? "workspace/symbol"
                                                              : "textDocument/documentSymbol")
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "textDocument/typeDefinition"
                : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "textDocument/selectionRange"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "textDocument/implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION       ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES       ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER            ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING
                    ? (policy->range_formatting ? "textDocument/rangeFormatting" : "textDocument/formatting")
                    : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION        ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME           ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE        ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS          ? "source-symbols"
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "source-type-definition"
                : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "source-selection-ranges"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "source-implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION       ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES       ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER            ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING       ? "formatting-source"
                                                                : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, policy->workspace_symbol_query != NULL,
                                  &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
                 policy->kind == SOURCE_QUERY_TYPE_DEFINITION ||
                 policy->kind == SOURCE_QUERY_IMPLEMENTATION || policy->kind == SOURCE_QUERY_SELECTION_RANGES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION &&
        policy->action_filter != UMI_LANGUAGE_ACTION_FILTER_ALL)
    {
        /* Parse every action first, then keep complete matching proposals. A
         * malformed unrelated row is not hidden by the presentation filter. */
        UmiLanguageCodeActionCatalogue *filtered = NULL;
        status =
            UmiLanguageCodeActionCatalogueFilter(result.actions, policy->action_filter, cancel, &filtered);
        if (status == UMI_STATUS_OK)
        {
            UmiLanguageCodeActionCatalogueDestroy(result.actions);
            result.actions = filtered;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Prepare and expand direct call relationships within one connection, withholding the entire result until shared cleanup succeeds.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, wire, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CODE_ACTION ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME    ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS
                    ? (policy->workspace_symbol_query != NULL ? "workspace/symbol"
                                                              : "textDocument/documentSymbol")
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "textDocument/typeDefinition"
                : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "textDocument/selectionRange"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "textDocument/implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION       ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES       ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER            ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING
                    ? (policy->range_formatting ? "textDocument/rangeFormatting" : "textDocument/formatting")
                    : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CODE_ACTION        ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME           ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE        ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS          ? "source-symbols"
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "source-type-definition"
                : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "source-selection-ranges"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "source-implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION       ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES       ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER            ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING       ? "formatting-source"
                                                                : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, policy->workspace_symbol_query != NULL,
                                  &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
                 policy->kind == SOURCE_QUERY_TYPE_DEFINITION ||
                 policy->kind == SOURCE_QUERY_IMPLEMENTATION || policy->kind == SOURCE_QUERY_SELECTION_RANGES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION &&
        policy->action_filter != UMI_LANGUAGE_ACTION_FILTER_ALL)
    {
        /* Parse every action first, then keep complete matching proposals. A
         * malformed unrelated row is not hidden by the presentation filter. */
        UmiLanguageCodeActionCatalogue *filtered = NULL;
        status =
            UmiLanguageCodeActionCatalogueFilter(result.actions, policy->action_filter, cancel, &filtered);
        if (status == UMI_STATUS_OK)
        {
            UmiLanguageCodeActionCatalogueDestroy(result.actions);
            result.actions = filtered;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION &&
        policy->resolve_action_limit != 0U)
        status =
            QueryResolveActions(server, request, policy, &clock, started, cancel, reply, &result.actions);
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.diagnostics != NULL           ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
/* Publish folding regions only after the captured document has closed and the owned language process has stopped.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, wire, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CALL_HIERARCHY ? "textDocument/prepareCallHierarchy"
                : policy->kind == SOURCE_QUERY_CODE_ACTION  ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME       ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE    ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS
                    ? (policy->workspace_symbol_query != NULL ? "workspace/symbol"
                                                              : "textDocument/documentSymbol")
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "textDocument/typeDefinition"
                : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "textDocument/selectionRange"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "textDocument/implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION       ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES       ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER            ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING
                    ? (policy->range_formatting ? "textDocument/rangeFormatting" : "textDocument/formatting")
                    : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CALL_HIERARCHY     ? "source-call-prepare"
                : policy->kind == SOURCE_QUERY_CODE_ACTION      ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME           ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE        ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS          ? "source-symbols"
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "source-type-definition"
                : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "source-selection-ranges"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "source-implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION       ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES       ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER            ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING       ? "formatting-source"
                                                                : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CALL_HIERARCHY)
            status = QueryCallHierarchy(server, request, policy, &clock, started, id, cancel, reply,
                                        &result.calls);
        else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, policy->workspace_symbol_query != NULL,
                                  &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
                 policy->kind == SOURCE_QUERY_TYPE_DEFINITION ||
                 policy->kind == SOURCE_QUERY_IMPLEMENTATION || policy->kind == SOURCE_QUERY_SELECTION_RANGES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION &&
        policy->action_filter != UMI_LANGUAGE_ACTION_FILTER_ALL)
    {
        /* Parse every action first, then keep complete matching proposals. A
         * malformed unrelated row is not hidden by the presentation filter. */
        UmiLanguageCodeActionCatalogue *filtered = NULL;
        status =
            UmiLanguageCodeActionCatalogueFilter(result.actions, policy->action_filter, cancel, &filtered);
        if (status == UMI_STATUS_OK)
        {
            UmiLanguageCodeActionCatalogueDestroy(result.actions);
            result.actions = filtered;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION &&
        policy->resolve_action_limit != 0U)
        status =
            QueryResolveActions(server, request, policy, &clock, started, cancel, reply, &result.actions);
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageCallResultDestroy(result.calls);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.calls != NULL                 ? UmiLanguageCallResultCount(result.calls)
        : result.diagnostics != NULL         ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus QueryRun(UmiLanguageRuntimeServer *server, const UmiLanguageCompletionRequest *request,
                          CompletionQueryWire *wire, const UmiCancellationToken *cancel,
                          UmiLanguageCompletionQueryReport *report, const SourceQueryPolicy *policy,
                          SourceQueryOutput *out)
{
    UmiLanguageRuntimeEnvelope *reply = malloc(sizeof(*reply));
    if (reply == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    SourceQueryOutput result = {0};
    char *action_parameters = NULL;
    int prepare_rename = 0;
    size_t additional_opened = 0U;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock), id = 0U;
    report->started = 1;
    UmiStatus status =
        umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_send_request(server, "initialize", wire->initialize, "", &id);
    if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK)
        status = QueryCapabilities(reply->json, id, policy, &prepare_rename);
    if (status == UMI_STATUS_OK && policy->pull_diagnostics)
        status = QueryPullDiagnosticPrepare(reply->json, id, request, wire);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_request_initialized(server);
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_transition(server, UMI_LANGUAGE_RUNTIME_SERVER_READY);
    if (status == UMI_STATUS_OK)
    {
        report->initialized = 1;
        status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen", wire->open);
    }
    if (status == UMI_STATUS_OK)
    {
        report->document_opened = 1;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < wire->additional_count; ++i)
        {
            if (CompletionCancelled(cancel))
            {
                status = UMI_STATUS_CANCELLED;
                break;
            }
            status = umi_language_runtime_server_send_notification(server, "textDocument/didOpen",
                                                                   wire->additional[i].open);
            if (status == UMI_STATUS_OK)
                ++additional_opened;
        }
        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME && prepare_rename)
        {
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            if (status == UMI_STATUS_OK)
                status = umi_language_runtime_server_send_request(
                    server, "textDocument/prepareRename", wire->prepare, "source-rename-prepare", &id);
            if (status == UMI_STATUS_OK)
                status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
            if (status == UMI_STATUS_OK)
                status = QueryRenamePreparation(reply->json, id, request, cancel);
        }

        if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION && policy->diagnostic_context)
            status = QueryActionDiagnostics(server, request, policy, wire, &clock, started, cancel, reply,
                                            &action_parameters);
        if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
            status = UMI_STATUS_CANCELLED;
        else if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
            status = umi_language_runtime_server_send_request(
                server,
                policy->kind == SOURCE_QUERY_CALL_HIERARCHY ? "textDocument/prepareCallHierarchy"
                : policy->kind == SOURCE_QUERY_CODE_ACTION  ? "textDocument/codeAction"
                : policy->kind == SOURCE_QUERY_RENAME       ? "textDocument/rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE    ? "textDocument/signatureHelp"
                : policy->kind == SOURCE_QUERY_SYMBOLS
                    ? (policy->workspace_symbol_query != NULL ? "workspace/symbol"
                                                              : "textDocument/documentSymbol")
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "textDocument/typeDefinition"
                : policy->kind == SOURCE_QUERY_FOLDING_RANGES ? "textDocument/foldingRange"
 : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "textDocument/selectionRange"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "textDocument/implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION       ? "textDocument/definition"
                : policy->kind == SOURCE_QUERY_REFERENCES       ? "textDocument/references"
                : policy->kind == SOURCE_QUERY_HOVER            ? "textDocument/hover"
                : policy->kind == SOURCE_QUERY_FORMATTING
                    ? (policy->range_formatting ? "textDocument/rangeFormatting" : "textDocument/formatting")
                    : "textDocument/completion",
                action_parameters == NULL ? wire->position : action_parameters,
                policy->kind == SOURCE_QUERY_CALL_HIERARCHY     ? "source-call-prepare"
                : policy->kind == SOURCE_QUERY_CODE_ACTION      ? "source-actions"
                : policy->kind == SOURCE_QUERY_RENAME           ? "source-rename"
                : policy->kind == SOURCE_QUERY_SIGNATURE        ? "source-signature"
                : policy->kind == SOURCE_QUERY_SYMBOLS          ? "source-symbols"
                : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "source-type-definition"
                : policy->kind == SOURCE_QUERY_FOLDING_RANGES ? "source-folding-ranges"
 : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "source-selection-ranges"
                : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "source-implementation"
                : policy->kind == SOURCE_QUERY_DEFINITION       ? "source-definition"
                : policy->kind == SOURCE_QUERY_REFERENCES       ? "source-references"
                : policy->kind == SOURCE_QUERY_HOVER            ? "source-information"
                : policy->kind == SOURCE_QUERY_FORMATTING       ? "formatting-source"
                                                                : "completion-source",
                &id);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_DIAGNOSTICS)
        status = policy->pull_diagnostics ? QueryPullDiagnostics(server, request, wire, &clock, started,
                                                                 cancel, reply, &result.diagnostics)
                                          : QueryAwaitDiagnostics(server, request, &clock, started, cancel,
                                                                  reply, &result.diagnostics);
    else if (status == UMI_STATUS_OK)
        status = QueryAwait(server, id, &clock, started, request->timeout_ms, cancel, reply);
    if (status == UMI_STATUS_OK && policy->kind != SOURCE_QUERY_DIAGNOSTICS)
    {
        if (policy->kind == SOURCE_QUERY_CALL_HIERARCHY)
            status = QueryCallHierarchy(server, request, policy, &clock, started, id, cancel, reply,
                                        &result.calls);
        else if (policy->kind == SOURCE_QUERY_CODE_ACTION)
            status = UmiLanguageCodeActionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.actions);
        else if (policy->kind == SOURCE_QUERY_RENAME)
            status = QueryRenameResult(reply->json, id, request, cancel, &result.workspace_edits);
        else if (policy->kind == SOURCE_QUERY_SIGNATURE)
            status = UmiLanguageSignatureCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                               &result.signatures);
        else if (policy->kind == SOURCE_QUERY_SYMBOLS)
            status = QuerySymbols(reply->json, id, request, cancel, policy->workspace_symbol_query != NULL,
                                  &result.symbols);
        else if (policy->kind == SOURCE_QUERY_DEFINITION || policy->kind == SOURCE_QUERY_REFERENCES ||
                 policy->kind == SOURCE_QUERY_TYPE_DEFINITION ||
                 policy->kind == SOURCE_QUERY_IMPLEMENTATION || policy->kind == SOURCE_QUERY_SELECTION_RANGES || policy->kind == SOURCE_QUERY_FOLDING_RANGES)
            status = QueryLocations(reply->json, id, request, policy, cancel, &result.locations);
        else if (policy->kind == SOURCE_QUERY_HOVER)
        {
            status = UmiLanguageHoverDocumentReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                          &result.hover);
            if (status == UMI_STATUS_OK)
            {
                UmiEditorTextPosition begin, end;
                UmiStatus range = UmiLanguageHoverDocumentRange(result.hover, &begin, &end);
                if (range == UMI_STATUS_OK)
                {
                    UmiEditorTextBufferView view = {0};
                    view.struct_size = (uint32_t)sizeof(view);
                    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
                    view.bytes = request->source;
                    view.byte_count = request->source_bytes;
                    view.capacity = request->source_bytes;
                    size_t first, last;
                    status = UmiEditorTextViewResolvePosition(&view, begin, &first);
                    if (status == UMI_STATUS_OK)
                        status = UmiEditorTextViewResolvePosition(&view, end, &last);
                }
                else if (range != UMI_STATUS_NOT_FOUND)
                    status = range;
            }
        }
        else if (policy->kind == SOURCE_QUERY_FORMATTING)
            status = UmiLanguageTextEditPreviewReadResponse(
                reply->json, strlen(reply->json), id, request->source, request->source_bytes,
                request->cursor_offset, cancel, &result.formatting);
        else
            status = UmiLanguageCompletionCatalogueReadResponse(reply->json, strlen(reply->json), id, cancel,
                                                                &result.completion);
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION &&
        policy->action_filter != UMI_LANGUAGE_ACTION_FILTER_ALL)
    {
        /* Parse every action first, then keep complete matching proposals. A
         * malformed unrelated row is not hidden by the presentation filter. */
        UmiLanguageCodeActionCatalogue *filtered = NULL;
        status =
            UmiLanguageCodeActionCatalogueFilter(result.actions, policy->action_filter, cancel, &filtered);
        if (status == UMI_STATUS_OK)
        {
            UmiLanguageCodeActionCatalogueDestroy(result.actions);
            result.actions = filtered;
        }
    }
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_CODE_ACTION &&
        policy->resolve_action_limit != 0U)
        status =
            QueryResolveActions(server, request, policy, &clock, started, cancel, reply, &result.actions);
    if (status == UMI_STATUS_OK && policy->kind == SOURCE_QUERY_RENAME)
        status = QueryCheckSourceEdits(result.workspace_edits, policy, cancel);
    report->query_status = status;
    /* A reply is not published until cleanup finishes. Close uses only the
     * captured URI, so changing a UI selection cannot close another document. */
    if (report->document_opened)
        report->close_status =
            umi_language_runtime_server_send_notification(server, "textDocument/didClose", wire->close);
    /* Attempt every matching close even when an earlier close fails. The
     * first cleanup error prevents result publication; shutdown still follows. */
    for (size_t i = additional_opened; i > 0U; --i)
    {
        UmiStatus closed = umi_language_runtime_server_send_notification(server, "textDocument/didClose",
                                                                         wire->additional[i - 1U].close);
        if (report->close_status == UMI_STATUS_OK)
            report->close_status = closed;
    }
    report->shutdown_status = report->initialized ? UmiLanguageRuntimeServerShutdown(server, 250U)
                                                  : umi_language_runtime_server_stop(server, 0U);
    if (status == UMI_STATUS_OK)
        status = report->close_status;
    if (status == UMI_STATUS_OK)
        status = report->shutdown_status;
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    free(action_parameters);
    free(reply);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionCatalogueDestroy(result.completion);
        UmiLanguageTextEditPreviewDestroy(result.formatting);
        UmiLanguageHoverDocumentDestroy(result.hover);
        UmiLanguageLocationCatalogueDestroy(result.locations);
        UmiLanguageSymbolCatalogueDestroy(result.symbols);
        UmiLanguageCallResultDestroy(result.calls);
        UmiLanguageSignatureCatalogueDestroy(result.signatures);
        UmiLanguageWorkspaceEditCatalogueDestroy(result.workspace_edits);
        UmiLanguageCodeActionCatalogueDestroy(result.actions);
        UmiLanguageDiagnosticCatalogueDestroy(result.diagnostics);
        return status;
    }
    report->choices =
        result.calls != NULL                 ? UmiLanguageCallResultCount(result.calls)
        : result.diagnostics != NULL         ? UmiLanguageDiagnosticCatalogueCount(result.diagnostics)
        : result.actions != NULL             ? UmiLanguageCodeActionCatalogueCount(result.actions)
        : result.workspace_edits != NULL     ? UmiLanguageWorkspaceEditCatalogueCount(result.workspace_edits)
        : result.signatures != NULL          ? UmiLanguageSignatureCatalogueCount(result.signatures)
        : result.symbols != NULL             ? UmiLanguageSymbolCatalogueCount(result.symbols)
        : result.locations != NULL           ? UmiLanguageLocationCatalogueCount(result.locations)
        : policy->kind == SOURCE_QUERY_HOVER ? UmiLanguageHoverDocumentCount(result.hover)
        : policy->kind == SOURCE_QUERY_FORMATTING ? UmiLanguageTextEditPreviewCount(result.formatting)
                                                  : UmiLanguageCompletionCatalogueCount(result.completion);
    *out = result;
    return UMI_STATUS_OK;
}
static UmiStatus QueryOutputs(UmiLanguageCompletionQueryReport *report,
                              UmiLanguageCompletionCatalogue **catalogue)
{
    if (catalogue != NULL)
        *catalogue = NULL;
    if (report != NULL)
    {
        memset(report, 0, sizeof(*report));
        report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    return report == NULL || catalogue == NULL ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
}
/* Private source-query ownership now serves both public completion and formatting entry points.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageCompletionQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageCompletionRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageCompletionQueryReport *out_report,
                                             UmiLanguageCompletionCatalogue **out_catalogue)
{
    UmiStatus status = QueryOutputs(out_report, out_catalogue);
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    status = QueryPrepare(request, cancel, &wire);
    UmiLanguageRuntimeServerSnapshot snapshot;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_snapshot(server, &snapshot);
    if (status == UMI_STATUS_OK &&
        (snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_STARTING ||
         strcmp(snapshot.root_uri, request->root_uri) != 0 || snapshot.pending_requests != 0U))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = QueryRun(server, request, wire, cancel, out_report, out_catalogue);
    if (!out_report->started)
        out_report->query_status = status;
    free(wire);
    return status;
}
#endif
/* Release the prepared additional-source notifications with the shared query wire owner.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryOnServer(UmiLanguageRuntimeServer *server,
                                     const UmiLanguageCompletionRequest *request,
                                     const UmiCancellationToken *cancel,
                                     UmiLanguageCompletionQueryReport *out_report,
                                     const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    UmiLanguageRuntimeServerSnapshot snapshot;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_snapshot(server, &snapshot);
    if (status == UMI_STATUS_OK &&
        (snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_STARTING ||
         strcmp(snapshot.root_uri, request->root_uri) != 0 || snapshot.pending_requests != 0U))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
    if (!out_report->started)
        out_report->query_status = status;
    free(wire);
    return status;
}
#endif
static UmiStatus SourceQueryOnServer(UmiLanguageRuntimeServer *server,
                                     const UmiLanguageCompletionRequest *request,
                                     const UmiCancellationToken *cancel,
                                     UmiLanguageCompletionQueryReport *out_report,
                                     const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    UmiLanguageRuntimeServerSnapshot snapshot;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_snapshot(server, &snapshot);
    if (status == UMI_STATUS_OK &&
        (snapshot.state != UMI_LANGUAGE_RUNTIME_SERVER_STARTING ||
         strcmp(snapshot.root_uri, request->root_uri) != 0 || snapshot.pending_requests != 0U))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
    if (!out_report->started)
        out_report->query_status = status;
    QueryWireDestroy(wire);
    return status;
}
/* Private source-query ownership now serves both public completion and formatting entry points.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageCompletionQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageCompletionRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageCompletionQueryReport *out_report,
                                           UmiLanguageCompletionCatalogue **out_catalogue)
{
    UmiStatus status = QueryOutputs(out_report, out_catalogue);
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_start("source-completion", profile, request->root_uri,
                                                   working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, out_catalogue);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    free(wire);
    return status;
}
#endif
/* Native connections carry a meaningful operation identifier for definition and reference requests.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_start(policy->hover        ? "source-information"
                                                   : policy->formatting ? "source-formatting"
                                                                        : "source-completion",
                                                   profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    free(wire);
    return status;
}
#endif
/* Temporary outline connections identify their source-symbol operation explicitly.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status =
            umi_language_runtime_server_start(policy->kind == SOURCE_QUERY_DEFINITION   ? "source-definition"
                                              : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                                              : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                                              : policy->kind == SOURCE_QUERY_FORMATTING ? "source-formatting"
                                                                                        : "source-completion",
                                              profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    free(wire);
    return status;
}
#endif
/* Give explicit parameter-help connections their own operation identity while keeping process ownership in Framework.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status =
            umi_language_runtime_server_start(policy->kind == SOURCE_QUERY_SYMBOLS      ? "source-symbols"
                                              : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                                              : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                                              : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                                              : policy->kind == SOURCE_QUERY_FORMATTING ? "source-formatting"
                                                                                        : "source-completion",
                                              profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    free(wire);
    return status;
}
#endif
/* Identify temporary rename connections while keeping process ownership in the common Framework lifecycle.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status =
            umi_language_runtime_server_start(policy->kind == SOURCE_QUERY_SIGNATURE    ? "source-signature"
                                              : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                                              : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                                              : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                                              : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                                              : policy->kind == SOURCE_QUERY_FORMATTING ? "source-formatting"
                                                                                        : "source-completion",
                                              profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    free(wire);
    return status;
}
#endif
/* Use a distinct operation identity for explicit code-action queries with the same native process owner.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status =
            umi_language_runtime_server_start(policy->kind == SOURCE_QUERY_RENAME       ? "source-rename"
                                              : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                                              : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                                              : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                                              : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                                              : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                                              : policy->kind == SOURCE_QUERY_FORMATTING ? "source-formatting"
                                                                                        : "source-completion",
                                              profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    free(wire);
    return status;
}
#endif
/* Identify diagnostic capture independently while retaining the shared native process lifetime.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status =
            umi_language_runtime_server_start(policy->kind == SOURCE_QUERY_CODE_ACTION  ? "source-actions"
                                              : policy->kind == SOURCE_QUERY_RENAME     ? "source-rename"
                                              : policy->kind == SOURCE_QUERY_SIGNATURE  ? "source-signature"
                                              : policy->kind == SOURCE_QUERY_SYMBOLS    ? "source-symbols"
                                              : policy->kind == SOURCE_QUERY_DEFINITION ? "source-definition"
                                              : policy->kind == SOURCE_QUERY_REFERENCES ? "source-references"
                                              : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                                              : policy->kind == SOURCE_QUERY_FORMATTING ? "source-formatting"
                                                                                        : "source-completion",
                                              profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    free(wire);
    return status;
}
#endif
/* Release the prepared additional-source notifications with the shared query wire owner.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status =
            umi_language_runtime_server_start(policy->kind == SOURCE_QUERY_DIAGNOSTICS ? "source-diagnostics"
                                              : policy->kind == SOURCE_QUERY_CODE_ACTION ? "source-actions"
                                              : policy->kind == SOURCE_QUERY_RENAME      ? "source-rename"
                                              : policy->kind == SOURCE_QUERY_SIGNATURE   ? "source-signature"
                                              : policy->kind == SOURCE_QUERY_SYMBOLS     ? "source-symbols"
                                              : policy->kind == SOURCE_QUERY_DEFINITION  ? "source-definition"
                                              : policy->kind == SOURCE_QUERY_REFERENCES  ? "source-references"
                                              : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                                              : policy->kind == SOURCE_QUERY_FORMATTING ? "source-formatting"
                                                                                        : "source-completion",
                                              profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    free(wire);
    return status;
}
#endif
/* Give native type and implementation searches descriptive connection identities while preserving the shared launch and cleanup policy.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status =
            umi_language_runtime_server_start(policy->kind == SOURCE_QUERY_DIAGNOSTICS ? "source-diagnostics"
                                              : policy->kind == SOURCE_QUERY_CODE_ACTION ? "source-actions"
                                              : policy->kind == SOURCE_QUERY_RENAME      ? "source-rename"
                                              : policy->kind == SOURCE_QUERY_SIGNATURE   ? "source-signature"
                                              : policy->kind == SOURCE_QUERY_SYMBOLS     ? "source-symbols"
                                              : policy->kind == SOURCE_QUERY_DEFINITION  ? "source-definition"
                                              : policy->kind == SOURCE_QUERY_REFERENCES  ? "source-references"
                                              : policy->kind == SOURCE_QUERY_HOVER      ? "source-information"
                                              : policy->kind == SOURCE_QUERY_FORMATTING ? "source-formatting"
                                                                                        : "source-completion",
                                              profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    QueryWireDestroy(wire);
    return status;
}
#endif
/* Give syntax-selection sessions a descriptive identity while retaining the shared native process lifecycle.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_start(
            policy->kind == SOURCE_QUERY_DIAGNOSTICS       ? "source-diagnostics"
            : policy->kind == SOURCE_QUERY_CODE_ACTION     ? "source-actions"
            : policy->kind == SOURCE_QUERY_RENAME          ? "source-rename"
            : policy->kind == SOURCE_QUERY_SIGNATURE       ? "source-signature"
            : policy->kind == SOURCE_QUERY_SYMBOLS         ? "source-symbols"
            : policy->kind == SOURCE_QUERY_TYPE_DEFINITION ? "source-type-definition"
            : policy->kind == SOURCE_QUERY_IMPLEMENTATION  ? "source-implementation"
            : policy->kind == SOURCE_QUERY_DEFINITION      ? "source-definition"
            : policy->kind == SOURCE_QUERY_REFERENCES      ? "source-references"
            : policy->kind == SOURCE_QUERY_HOVER           ? "source-information"
            : policy->kind == SOURCE_QUERY_FORMATTING      ? "source-formatting"
                                                           : "source-completion",
            profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    QueryWireDestroy(wire);
    return status;
}
#endif
/* Identify call inspection as an explicit temporary service with the same child-process ownership as other source tools.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_start(
            policy->kind == SOURCE_QUERY_DIAGNOSTICS        ? "source-diagnostics"
            : policy->kind == SOURCE_QUERY_CODE_ACTION      ? "source-actions"
            : policy->kind == SOURCE_QUERY_RENAME           ? "source-rename"
            : policy->kind == SOURCE_QUERY_SIGNATURE        ? "source-signature"
            : policy->kind == SOURCE_QUERY_SYMBOLS          ? "source-symbols"
            : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "source-type-definition"
            : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "source-selection-ranges"
            : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "source-implementation"
            : policy->kind == SOURCE_QUERY_DEFINITION       ? "source-definition"
            : policy->kind == SOURCE_QUERY_REFERENCES       ? "source-references"
            : policy->kind == SOURCE_QUERY_HOVER            ? "source-information"
            : policy->kind == SOURCE_QUERY_FORMATTING       ? "source-formatting"
                                                            : "source-completion",
            profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    QueryWireDestroy(wire);
    return status;
}
#endif
/* Identify folding sessions explicitly while preserving the common native transport lifecycle.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_start(
            policy->kind == SOURCE_QUERY_CALL_HIERARCHY     ? "source-call-hierarchy"
            : policy->kind == SOURCE_QUERY_DIAGNOSTICS      ? "source-diagnostics"
            : policy->kind == SOURCE_QUERY_CODE_ACTION      ? "source-actions"
            : policy->kind == SOURCE_QUERY_RENAME           ? "source-rename"
            : policy->kind == SOURCE_QUERY_SIGNATURE        ? "source-signature"
            : policy->kind == SOURCE_QUERY_SYMBOLS          ? "source-symbols"
            : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "source-type-definition"
            : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "source-selection-ranges"
            : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "source-implementation"
            : policy->kind == SOURCE_QUERY_DEFINITION       ? "source-definition"
            : policy->kind == SOURCE_QUERY_REFERENCES       ? "source-references"
            : policy->kind == SOURCE_QUERY_HOVER            ? "source-information"
            : policy->kind == SOURCE_QUERY_FORMATTING       ? "source-formatting"
                                                            : "source-completion",
            profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    QueryWireDestroy(wire);
    return status;
}
#endif
static UmiStatus SourceQueryNative(const UmiLanguageServerProfile *profile, const char *working_directory,
                                   const UmiLanguageCompletionRequest *request,
                                   const UmiCancellationToken *cancel,
                                   UmiLanguageCompletionQueryReport *out_report,
                                   const SourceQueryPolicy *policy, SourceQueryOutput *out)
{
    memset(out, 0, sizeof(*out));
    memset(out_report, 0, sizeof(*out_report));
    out_report->query_status = UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    CompletionQueryWire *wire = NULL;
    UmiLanguageRuntimeServer *server = NULL;
    status = QueryPrepare(request, cancel, policy, &wire);
    if (status == UMI_STATUS_OK)
        status = LanguageProfileText(profile);
    if (status == UMI_STATUS_OK && !profile->enabled)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = umi_language_runtime_server_start(
            policy->kind == SOURCE_QUERY_CALL_HIERARCHY     ? "source-call-hierarchy"
            : policy->kind == SOURCE_QUERY_DIAGNOSTICS      ? "source-diagnostics"
            : policy->kind == SOURCE_QUERY_CODE_ACTION      ? "source-actions"
            : policy->kind == SOURCE_QUERY_RENAME           ? "source-rename"
            : policy->kind == SOURCE_QUERY_SIGNATURE        ? "source-signature"
            : policy->kind == SOURCE_QUERY_SYMBOLS          ? "source-symbols"
            : policy->kind == SOURCE_QUERY_TYPE_DEFINITION  ? "source-type-definition"
            : policy->kind == SOURCE_QUERY_FOLDING_RANGES ? "source-folding-ranges"
 : policy->kind == SOURCE_QUERY_SELECTION_RANGES ? "source-selection-ranges"
            : policy->kind == SOURCE_QUERY_IMPLEMENTATION   ? "source-implementation"
            : policy->kind == SOURCE_QUERY_DEFINITION       ? "source-definition"
            : policy->kind == SOURCE_QUERY_REFERENCES       ? "source-references"
            : policy->kind == SOURCE_QUERY_HOVER            ? "source-information"
            : policy->kind == SOURCE_QUERY_FORMATTING       ? "source-formatting"
                                                            : "source-completion",
            profile, request->root_uri, working_directory, &server);
    if (status == UMI_STATUS_OK)
    {
        status = QueryRun(server, request, wire, cancel, out_report, policy, out);
        /* A pre-initialization allocation failure still owns a launched child. */
        if (!out_report->started)
            out_report->shutdown_status = umi_language_runtime_server_stop(server, 0U);
    }
    if (!out_report->started)
        out_report->query_status = status;
    umi_language_runtime_server_destroy(server);
    QueryWireDestroy(wire);
    return status;
}

/* Keep completion callers source-compatible while the private transport owner
 * also supports formatting. A result is handed out only after cleanup succeeds. */
UmiStatus UmiLanguageCompletionQueryOnServer(UmiLanguageRuntimeServer *server,
                                             const UmiLanguageCompletionRequest *request,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageCompletionQueryReport *out_report,
                                             UmiLanguageCompletionCatalogue **out_catalogue)
{
    UmiStatus status = QueryOutputs(out_report, out_catalogue);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {0};
    SourceQueryOutput result = {0};
    status = SourceQueryOnServer(server, request, cancel, out_report, &policy, &result);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.completion;
    return status;
}
UmiStatus UmiLanguageCompletionQueryNative(const UmiLanguageServerProfile *profile,
                                           const char *working_directory,
                                           const UmiLanguageCompletionRequest *request,
                                           const UmiCancellationToken *cancel,
                                           UmiLanguageCompletionQueryReport *out_report,
                                           UmiLanguageCompletionCatalogue **out_catalogue)
{
    UmiStatus status = QueryOutputs(out_report, out_catalogue);
    if (status != UMI_STATUS_OK)
        return status;
    const SourceQueryPolicy policy = {0};
    SourceQueryOutput result = {0};
    status = SourceQueryNative(profile, working_directory, request, cancel, out_report, &policy, &result);
    if (status == UMI_STATUS_OK)
        *out_catalogue = result.completion;
    return status;
}
#include "formatting_query.inc"
#include "range_formatting_query.inc"

#include "hover_query.inc"

#include "navigation_query.inc"

#include "symbol_query.inc"

#include "signature_query.inc"

#include "rename_query.inc"

#include "code_action_query.inc"

#include "diagnostic_query.inc"

#include "workspace_symbol_query.inc"

#include "call_query.inc"
