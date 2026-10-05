/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/diagnostic_catalogue.c
 * PURPOSE: Retain full diagnostic publications without truncating messages or related locations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/diagnostic_catalogue.h"
#include "source_location_internal.h"
#include "completion_catalogue_internal.h"
#include <stdio.h>
#include "umicom/language_runtime/response_tree.h"
#include "umicom/editor/text_position_index.h"

typedef struct DiagnosticRelatedRow
{
    UmiLanguageDiagnosticRelated value;
    char *uri, *message;
} DiagnosticRelatedRow;
typedef struct DiagnosticRow
{
    UmiLanguageDiagnostic value;
    char *message, *source, *code, *description;
    DiagnosticRelatedRow *related;
    int item;
} DiagnosticRow;
struct UmiLanguageDiagnosticCatalogue
{
    UmiJsonTree *tree;
    char *uri;
    char *pull_result_id;
    int has_version;
    int32_t version;
    DiagnosticRow *rows;
    size_t count, related_count;
};
/* Size decoded strings from their actual JSON spelling. The document limit
 * bounds aggregate storage, and per-field limits give callers useful failures. */
static UmiStatus DiagnosticText(const UmiJsonTree *tree, int node, size_t limit, char **out)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    const char *span = NULL;
    size_t bytes = 0U;
    UmiStatus status = UmiJsonTreeSourceSpan(tree, node, &span, &bytes);
    (void)span;
    if (status != UMI_STATUS_OK)
        return status;
    size_t capacity = bytes < limit ? bytes + 1U : limit + 1U;
    char *text = malloc(capacity);
    if (text == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiJsonTreeText(tree, node, text, capacity);
    if (status == UMI_STATUS_OK)
        *out = text;
    else
        free(text);
    return status;
}
static UmiStatus DiagnosticRelatedRead(const UmiJsonTree *tree, int item, DiagnosticRelatedRow *row)
{
    int location = -1, uri = -1, range = -1, message = -1;
    UmiStatus status = LocationMember(tree, item, "location", 1, &location);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, location, "uri", 1, &uri);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, location, "range", 1, &range);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, item, "message", 1, &message);
    if (status == UMI_STATUS_OK)
        status = LocationUri(tree, uri, &row->uri);
    if (status == UMI_STATUS_OK)
        status = LocationRange(tree, range, &row->value.location.target);
    if (status == UMI_STATUS_OK)
        status = DiagnosticText(tree, message, 65536U, &row->message);
    row->value.location.uri = row->uri;
    row->value.location.selection = row->value.location.target;
    row->value.message = row->message;
    return status;
}
static UmiStatus DiagnosticRead(UmiLanguageDiagnosticCatalogue *catalogue, int item, DiagnosticRow *row,
                                const UmiCancellationToken *cancel)
{
    const UmiJsonTree *tree = catalogue->tree;
    int range = -1, message = -1, severity = -1, code = -1, source = -1, description = -1, tags = -1,
        related = -1, data = -1;
    row->item = item;
    row->value.severity = 1;
    UmiStatus status = LocationMember(tree, item, "range", 1, &range);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, item, "message", 1, &message);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, item, "severity", 0, &severity);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, item, "code", 0, &code);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, item, "source", 0, &source);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, item, "codeDescription", 0, &description);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, item, "tags", 0, &tags);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, item, "relatedInformation", 0, &related);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, item, "data", 0, &data);
    if (status == UMI_STATUS_OK)
        status = LocationRange(tree, range, &row->value.range);
    if (status == UMI_STATUS_OK)
        status = DiagnosticText(tree, message, 65536U, &row->message);
    if (status == UMI_STATUS_OK && severity >= 0)
    {
        int64_t value = 0;
        status = UmiJsonTreeInteger(tree, severity, &value);
        if (status == UMI_STATUS_OK && (value < 1 || value > 4))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
        {
            row->value.severity = (int)value;
            row->value.has_severity = 1;
        }
    }
    if (status == UMI_STATUS_OK && code >= 0)
    {
        row->value.has_code = 1;
        if (UmiJsonTreeKind(tree, code) == UMI_LANGUAGE_RUNTIME_JSON_STRING)
            status = DiagnosticText(tree, code, 4096U, &row->code);
        else
        {
            int64_t value = 0;
            status = UmiJsonTreeInteger(tree, code, &value);
            if (status == UMI_STATUS_OK && (value < INT32_MIN || value > INT32_MAX))
                status = UMI_STATUS_PARSE_ERROR;
            if (status == UMI_STATUS_OK)
            {
                row->code = malloc(16U);
                if (row->code == NULL)
                    status = UMI_STATUS_OUT_OF_MEMORY;
                else
                {
                    (void)snprintf(row->code, 16U, "%lld", (long long)value);
                    row->value.code_is_number = 1;
                }
            }
        }
    }
    if (status == UMI_STATUS_OK && source >= 0)
        status = DiagnosticText(tree, source, 4096U, &row->source);
    if (status == UMI_STATUS_OK && description >= 0)
    {
        int href = -1;
        status = LocationMember(tree, description, "href", 1, &href);
        if (status == UMI_STATUS_OK)
            status = LocationUri(tree, href, &row->description);
    }
    if (status == UMI_STATUS_OK && tags >= 0)
    {
        if (UmiJsonTreeKind(tree, tags) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        else if (UmiJsonTreeCount(tree, tags) > 256U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        for (int tag = UmiJsonTreeFirst(tree, tags); status == UMI_STATUS_OK && tag >= 0;
             tag = UmiJsonTreeNext(tree, tag))
        {
            int64_t value = 0;
            status = UmiJsonTreeInteger(tree, tag, &value);
            if (status == UMI_STATUS_OK && (value < 1 || value > INT32_MAX))
                status = UMI_STATUS_PARSE_ERROR;
            if (value == 1)
                row->value.unnecessary = 1;
            if (value == 2)
                row->value.deprecated = 1;
        }
    }
    if (status == UMI_STATUS_OK && related >= 0)
    {
        if (UmiJsonTreeKind(tree, related) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        else
        {
            row->value.related_count = UmiJsonTreeCount(tree, related);
            if (row->value.related_count > 128U ||
                row->value.related_count > 8192U - catalogue->related_count)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
            else
            {
                catalogue->related_count += row->value.related_count;
                row->related = calloc(row->value.related_count == 0U ? 1U : row->value.related_count,
                                      sizeof(*row->related));
                if (row->related == NULL)
                    status = UMI_STATUS_OUT_OF_MEMORY;
                int node = UmiJsonTreeFirst(tree, related);
                for (size_t i = 0U; status == UMI_STATUS_OK && i < row->value.related_count;
                     ++i, node = UmiJsonTreeNext(tree, node))
                    status = CompletionCancelled(cancel)
                                 ? UMI_STATUS_CANCELLED
                                 : DiagnosticRelatedRead(tree, node, &row->related[i]);
            }
        }
    }
    row->value.message = row->message;
    row->value.source = row->source == NULL ? "" : row->source;
    row->value.code = row->code == NULL ? "" : row->code;
    row->value.description_uri = row->description == NULL ? "" : row->description;
    row->value.has_data = data >= 0;
    return status;
}
/* Push publications and pull reports use the same complete row decoder. This
 * keeps message limits, related locations and cancellation behavior consistent. */
static UmiStatus DiagnosticReadRows(UmiLanguageDiagnosticCatalogue *catalogue, int diagnostics,
                                    const UmiCancellationToken *cancel)
{
    UmiStatus status = UMI_STATUS_OK;
    if (status == UMI_STATUS_OK &&
        UmiJsonTreeKind(catalogue->tree, diagnostics) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
    {
        catalogue->count = UmiJsonTreeCount(catalogue->tree, diagnostics);
        if (catalogue->count > 4096U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
        {
            catalogue->rows =
                calloc(catalogue->count == 0U ? 1U : catalogue->count, sizeof(*catalogue->rows));
            if (catalogue->rows == NULL)
                status = UMI_STATUS_OUT_OF_MEMORY;
            int node = UmiJsonTreeFirst(catalogue->tree, diagnostics);
            for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count;
                 ++i, node = UmiJsonTreeNext(catalogue->tree, node))
                status = CompletionCancelled(cancel)
                             ? UMI_STATUS_CANCELLED
                             : DiagnosticRead(catalogue, node, &catalogue->rows[i], cancel);
        }
    }
    return status;
}
/* Share complete diagnostic row ownership between notification and explicit-request responses, without changing the existing publication contract.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageDiagnosticCatalogueCreate(const void *json, size_t bytes,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageDiagnosticCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (json == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageDiagnosticCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    UmiStatus status = UmiJsonTreeCreate(json, bytes, &limits, cancel, &catalogue->tree);
    int uri = -1, version = -1, diagnostics = -1;
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, 0, "uri", 1, &uri);
    if (status == UMI_STATUS_OK)
        status = LocationUri(catalogue->tree, uri, &catalogue->uri);
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, 0, "version", 0, &version);
    if (status == UMI_STATUS_OK && version >= 0)
    {
        int64_t value = 0;
        status = UmiJsonTreeInteger(catalogue->tree, version, &value);
        if (status == UMI_STATUS_OK && (value < INT32_MIN || value > INT32_MAX))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
        {
            catalogue->has_version = 1;
            catalogue->version = (int32_t)value;
        }
    }
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, 0, "diagnostics", 1, &diagnostics);
    if (status == UMI_STATUS_OK &&
        UmiJsonTreeKind(catalogue->tree, diagnostics) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
    {
        catalogue->count = UmiJsonTreeCount(catalogue->tree, diagnostics);
        if (catalogue->count > 4096U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
        {
            catalogue->rows =
                calloc(catalogue->count == 0U ? 1U : catalogue->count, sizeof(*catalogue->rows));
            if (catalogue->rows == NULL)
                status = UMI_STATUS_OUT_OF_MEMORY;
            int node = UmiJsonTreeFirst(catalogue->tree, diagnostics);
            for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count;
                 ++i, node = UmiJsonTreeNext(catalogue->tree, node))
                status = CompletionCancelled(cancel)
                             ? UMI_STATUS_CANCELLED
                             : DiagnosticRead(catalogue, node, &catalogue->rows[i], cancel);
        }
    }
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    return status;
}
#endif
UmiStatus UmiLanguageDiagnosticCatalogueCreate(const void *json, size_t bytes,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageDiagnosticCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (json == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageDiagnosticCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    UmiStatus status = UmiJsonTreeCreate(json, bytes, &limits, cancel, &catalogue->tree);
    int uri = -1, version = -1, diagnostics = -1;
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, 0, "uri", 1, &uri);
    if (status == UMI_STATUS_OK)
        status = LocationUri(catalogue->tree, uri, &catalogue->uri);
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, 0, "version", 0, &version);
    if (status == UMI_STATUS_OK && version >= 0)
    {
        int64_t value = 0;
        status = UmiJsonTreeInteger(catalogue->tree, version, &value);
        if (status == UMI_STATUS_OK && (value < INT32_MIN || value > INT32_MAX))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
        {
            catalogue->has_version = 1;
            catalogue->version = (int32_t)value;
        }
    }
    if (status == UMI_STATUS_OK)
        status = LocationMember(catalogue->tree, 0, "diagnostics", 1, &diagnostics);
    if (status == UMI_STATUS_OK)
        status = DiagnosticReadRows(catalogue, diagnostics, cancel);
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    return status;
}
UmiStatus UmiLanguageDiagnosticCatalogueReadNotification(const void *json, size_t bytes,
                                                         const UmiCancellationToken *cancel,
                                                         UmiLanguageDiagnosticCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    UmiStatus status = UmiJsonTreeCreate(json, bytes, &limits, cancel, &tree);
    int protocol = -1, method = -1, params = -1, forbidden = -1;
    char rpc[8], name[128];
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, 0, "jsonrpc", 1, &protocol);
    if (status == UMI_STATUS_OK)
        status = CompletionText(tree, protocol, rpc, sizeof(rpc));
    if (status == UMI_STATUS_OK && strcmp(rpc, "2.0") != 0)
        status = UMI_STATUS_PARSE_ERROR;
    const char *names[] = {"id", "result", "error"};
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 3U; ++i)
    {
        status = LocationMember(tree, 0, names[i], 0, &forbidden);
        if (status == UMI_STATUS_OK && forbidden >= 0)
            status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, 0, "method", 1, &method);
    if (status == UMI_STATUS_OK)
        status = CompletionText(tree, method, name, sizeof(name));
    if (status == UMI_STATUS_OK && strcmp(name, "textDocument/publishDiagnostics") != 0)
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, 0, "params", 1, &params);
    const char *span = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, params, &span, &length);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticCatalogueCreate(span, length, cancel, out);
    UmiJsonTreeDestroy(tree);
    return status;
}
/* Release the optional pull report identifier with the same owner as its diagnostic rows and original JSON.
 * The former implementation is retained for engineering review. */
#if 0
void UmiLanguageDiagnosticCatalogueDestroy(UmiLanguageDiagnosticCatalogue *catalogue)
{
    if (catalogue == NULL)
        return;
    if (catalogue->rows != NULL)
        for (size_t i = 0U; i < catalogue->count; ++i)
        {
            DiagnosticRow *row = &catalogue->rows[i];
            free(row->message);
            free(row->source);
            free(row->code);
            free(row->description);
            if (row->related != NULL)
                for (size_t j = 0U; j < row->value.related_count; ++j)
                {
                    free(row->related[j].uri);
                    free(row->related[j].message);
                }
            free(row->related);
        }
    free(catalogue->rows);
    free(catalogue->uri);
    UmiJsonTreeDestroy(catalogue->tree);
    free(catalogue);
}
#endif
void UmiLanguageDiagnosticCatalogueDestroy(UmiLanguageDiagnosticCatalogue *catalogue)
{
    if (catalogue == NULL)
        return;
    if (catalogue->rows != NULL)
        for (size_t i = 0U; i < catalogue->count; ++i)
        {
            DiagnosticRow *row = &catalogue->rows[i];
            free(row->message);
            free(row->source);
            free(row->code);
            free(row->description);
            if (row->related != NULL)
                for (size_t j = 0U; j < row->value.related_count; ++j)
                {
                    free(row->related[j].uri);
                    free(row->related[j].message);
                }
            free(row->related);
        }
    free(catalogue->rows);
    free(catalogue->uri);
    free(catalogue->pull_result_id);
    UmiJsonTreeDestroy(catalogue->tree);
    free(catalogue);
}
size_t UmiLanguageDiagnosticCatalogueCount(const UmiLanguageDiagnosticCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
UmiStatus UmiLanguageDiagnosticCataloguePublication(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                    UmiLanguageDiagnosticPublication *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = (UmiLanguageDiagnosticPublication){catalogue->uri, catalogue->has_version, catalogue->version};
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageDiagnosticCatalogueAt(const UmiLanguageDiagnosticCatalogue *catalogue, size_t index,
                                           UmiLanguageDiagnostic *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->rows[index].value;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageDiagnosticCatalogueRelated(const UmiLanguageDiagnosticCatalogue *catalogue, size_t index,
                                                size_t related, UmiLanguageDiagnosticRelated *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count || related >= catalogue->rows[index].value.related_count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->rows[index].related[related].value;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageDiagnosticCatalogueItemJson(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                 size_t index, const char **out_json, size_t *out_bytes)
{
    if (out_json != NULL)
        *out_json = NULL;
    if (out_bytes != NULL)
        *out_bytes = 0U;
    if (catalogue == NULL || out_json == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    return UmiJsonTreeSourceSpan(catalogue->tree, catalogue->rows[index].item, out_json, out_bytes);
}
/* Repeated diagnostic ranges share one immutable coordinate index instead of scanning the entire source for each endpoint.
 * The former implementation is retained for engineering review. */
#if 0
static UmiStatus DiagnosticResolve(const UmiEditorTextBufferView *view,UmiLanguageSourceRange range)
{
    size_t first=0U,last=0U;
    UmiStatus status=UmiEditorTextViewResolvePosition(view,range.start,&first);
    if(status==UMI_STATUS_OK) status=UmiEditorTextViewResolvePosition(view,range.end,&last);
    return status;
}
#endif
static UmiStatus DiagnosticResolve(const UmiEditorTextPositionIndex *index, UmiLanguageSourceRange range)
{
    size_t first = 0U, last = 0U;
    UmiStatus status = UmiEditorTextPositionIndexResolve(index, range.start, &first);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextPositionIndexResolve(index, range.end, &last);
    return status;
}
/* Validate the entire captured text with the existing exact-position API, including empty diagnostic sets.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageDiagnosticCatalogueValidateSource(const UmiLanguageDiagnosticCatalogue *catalogue,
    const char *uri,const int32_t *version,const char *source,size_t bytes,const UmiCancellationToken *cancel)
{
    if(catalogue==NULL || uri==NULL || source==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(strcmp(uri,catalogue->uri)!=0) return UMI_STATUS_NOT_FOUND;
    if(catalogue->has_version && version==NULL) return UMI_STATUS_NOT_IMPLEMENTED;
    if(catalogue->has_version && catalogue->version!=*version) return UMI_STATUS_INVALID_STATE;
    UmiEditorTextBufferView view={0};view.struct_size=(uint32_t)sizeof(view);
    view.api_version=UMI_EDITOR_TEXT_BUFFER_API_VERSION;view.bytes=source;view.byte_count=bytes;view.capacity=bytes;
    UmiStatus status=UmiEditorTextViewValidate(&view);
    for(size_t i=0U;status==UMI_STATUS_OK && i<catalogue->count;++i) {
        if(CompletionCancelled(cancel)) { status=UMI_STATUS_CANCELLED;break; }
        const DiagnosticRow *row=&catalogue->rows[i];status=DiagnosticResolve(&view,row->value.range);
        for(size_t j=0U;status==UMI_STATUS_OK && j<row->value.related_count;++j) {
            const DiagnosticRelatedRow *related=&row->related[j];
            if(CompletionCancelled(cancel)) status=UMI_STATUS_CANCELLED;
            else if(strcmp(uri,related->uri)==0) status=DiagnosticResolve(&view,related->value.location.target);
        }
    }
    if(status==UMI_STATUS_OK && CompletionCancelled(cancel)) status=UMI_STATUS_CANCELLED;
    return status;
}
#endif
/* Own one validated position index for this source revision so large diagnostic sets resolve in bounded local scans.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiLanguageDiagnosticCatalogueValidateSource(const UmiLanguageDiagnosticCatalogue *catalogue,
    const char *uri,const int32_t *version,const char *source,size_t bytes,const UmiCancellationToken *cancel)
{
    if(catalogue==NULL || uri==NULL || source==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(strcmp(uri,catalogue->uri)!=0) return UMI_STATUS_NOT_FOUND;
    if(catalogue->has_version && version==NULL) return UMI_STATUS_NOT_IMPLEMENTED;
    if(catalogue->has_version && catalogue->version!=*version) return UMI_STATUS_INVALID_STATE;
    UmiEditorTextBufferView view={0};view.struct_size=(uint32_t)sizeof(view);
    view.api_version=UMI_EDITOR_TEXT_BUFFER_API_VERSION;view.bytes=source;view.byte_count=bytes;view.capacity=bytes;
    UmiEditorTextPosition end;
    UmiStatus status=UmiEditorTextViewPositionAt(&view,bytes,&end);
    for(size_t i=0U;status==UMI_STATUS_OK && i<catalogue->count;++i) {
        if(CompletionCancelled(cancel)) { status=UMI_STATUS_CANCELLED;break; }
        const DiagnosticRow *row=&catalogue->rows[i];status=DiagnosticResolve(&view,row->value.range);
        for(size_t j=0U;status==UMI_STATUS_OK && j<row->value.related_count;++j) {
            const DiagnosticRelatedRow *related=&row->related[j];
            if(CompletionCancelled(cancel)) status=UMI_STATUS_CANCELLED;
            else if(strcmp(uri,related->uri)==0) status=DiagnosticResolve(&view,related->value.location.target);
        }
    }
    if(status==UMI_STATUS_OK && CompletionCancelled(cancel)) status=UMI_STATUS_CANCELLED;
    return status;
}
#endif
UmiStatus UmiLanguageDiagnosticCatalogueValidateSource(const UmiLanguageDiagnosticCatalogue *catalogue,
                                                       const char *uri, const int32_t *version,
                                                       const char *source, size_t bytes,
                                                       const UmiCancellationToken *cancel)
{
    if (catalogue == NULL || uri == NULL || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(uri, catalogue->uri) != 0)
        return UMI_STATUS_NOT_FOUND;
    if (catalogue->has_version && version == NULL)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (catalogue->has_version && catalogue->version != *version)
        return UMI_STATUS_INVALID_STATE;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = source;
    view.byte_count = bytes;
    view.capacity = bytes;
    UmiEditorTextPositionIndex *index = NULL;
    UmiStatus status = UmiEditorTextPositionIndexCreate(&view, cancel, &index);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count; ++i)
    {
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        const DiagnosticRow *row = &catalogue->rows[i];
        status = DiagnosticResolve(index, row->value.range);
        for (size_t j = 0U; status == UMI_STATUS_OK && j < row->value.related_count; ++j)
        {
            const DiagnosticRelatedRow *related = &row->related[j];
            if (CompletionCancelled(cancel))
                status = UMI_STATUS_CANCELLED;
            else if (strcmp(uri, related->uri) == 0)
                status = DiagnosticResolve(index, related->value.location.target);
        }
    }
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiEditorTextPositionIndexDestroy(index);
    return status;
}

#include "diagnostic_pull_catalogue.inc"

#include "diagnostic_locations.inc"
