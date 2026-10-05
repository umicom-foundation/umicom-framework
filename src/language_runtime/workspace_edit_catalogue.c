/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/workspace_edit_catalogue.c
 * PURPOSE: Retain protocol edit groups, document versions and annotation requirements before any source is changed.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/workspace_edit_catalogue.h"
#include "umicom/language_runtime/response_tree.h"
#include "source_location_internal.h"
#include "umicom/language_runtime/json_writer.h"
typedef struct WorkspaceTextChange
{
    UmiLanguageWorkspaceTextChange value;
    char *text;
} WorkspaceTextChange;
typedef struct WorkspaceDocumentChange
{
    UmiLanguageWorkspaceDocumentChange value;
    char *uri;
    WorkspaceTextChange *edits;
} WorkspaceDocumentChange;
typedef struct WorkspaceAnnotation
{
    UmiLanguageWorkspaceChangeAnnotation value;
    char *id, *label, *description;
} WorkspaceAnnotation;
struct UmiLanguageWorkspaceEditCatalogue
{
    WorkspaceDocumentChange *documents;
    WorkspaceAnnotation *annotations;
    size_t count, annotation_count, edit_count;
};
static UmiStatus WorkspaceText(const UmiJsonTree *tree, int node, size_t limit, char **out)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    const char *raw;
    size_t bytes;
    UmiStatus status = UmiJsonTreeSourceSpan(tree, node, &raw, &bytes);
    (void)raw;
    if (status != UMI_STATUS_OK)
        return status;
    char *text = malloc(bytes + 1U);
    if (text == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiJsonTreeText(tree, node, text, bytes + 1U);
    if (status == UMI_STATUS_OK && strlen(text) > limit)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        *out = text;
    else
        free(text);
    return status;
}
/* Allocate all annotation rows before decoding. This gives the destructor one
 * uniform path for a failure at any key, description or confirmation flag. */
static UmiStatus WorkspaceAnnotations(const UmiJsonTree *tree, int node, const UmiCancellationToken *cancel,
                                      UmiLanguageWorkspaceEditCatalogue *catalogue)
{
    if (node < 0)
        return UMI_STATUS_OK;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    catalogue->annotation_count = UmiJsonTreeCount(tree, node);
    if (catalogue->annotation_count > 256U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (catalogue->annotation_count == 0U)
        return UMI_STATUS_OK;
    catalogue->annotations = calloc(catalogue->annotation_count, sizeof(*catalogue->annotations));
    if (catalogue->annotations == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    int key = UmiJsonTreeFirst(tree, node);
    for (size_t i = 0U; i < catalogue->annotation_count; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        WorkspaceAnnotation *entry = &catalogue->annotations[i];
        int value = UmiJsonTreeNext(tree, key), label, description, confirmation;
        UmiStatus status = WorkspaceText(tree, key, 4096U, &entry->id);
        if (status == UMI_STATUS_OK)
            status = LocationMember(tree, value, "label", 1, &label);
        if (status == UMI_STATUS_OK)
            status = WorkspaceText(tree, label, 65536U, &entry->label);
        if (status == UMI_STATUS_OK)
            status = LocationMember(tree, value, "description", 0, &description);
        if (status == UMI_STATUS_OK && description >= 0)
            status = WorkspaceText(tree, description, 65536U, &entry->description);
        if (status == UMI_STATUS_OK)
            status = LocationMember(tree, value, "needsConfirmation", 0, &confirmation);
        if (status == UMI_STATUS_OK && confirmation >= 0)
            status = UmiJsonTreeBoolean(tree, confirmation, &entry->value.needs_confirmation);
        if (status != UMI_STATUS_OK)
            return status;
        for (size_t previous = 0U; previous < i; ++previous)
            if (strcmp(catalogue->annotations[previous].id, entry->id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        entry->value.id = entry->id;
        entry->value.label = entry->label;
        entry->value.description = entry->description == NULL ? "" : entry->description;
        key = UmiJsonTreeNext(tree, value);
    }
    return UMI_STATUS_OK;
}
static UmiStatus WorkspaceEditRead(const UmiJsonTree *tree, int node, int annotated,
                                   const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                   WorkspaceTextChange *entry)
{
    int range, text, annotation;
    entry->value.annotation = SIZE_MAX;
    UmiStatus status = LocationMember(tree, node, "range", 1, &range);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "newText", 1, &text);
    if (status == UMI_STATUS_OK)
        status = LocationMember(tree, node, "annotationId", 0, &annotation);
    if (status != UMI_STATUS_OK)
        return status;
    /* Refuse alternate edit shapes or unadvertised semantics. A later protocol
     * extension should add an explicit reader rather than silently omit data. */
    if (UmiJsonTreeCount(tree, node) != (annotation < 0 ? 2U : 3U))
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (annotation >= 0 && !annotated)
        return UMI_STATUS_NOT_IMPLEMENTED;
    status = LocationRange(tree, range, &entry->value.range);
    if (status == UMI_STATUS_OK)
        status = WorkspaceText(tree, text, 65536U, &entry->text);
    if (status == UMI_STATUS_OK && annotation >= 0)
    {
        char *id = NULL;
        status = WorkspaceText(tree, annotation, 4096U, &id);
        if (status == UMI_STATUS_OK)
        {
            status = UMI_STATUS_NOT_FOUND;
            for (size_t i = 0U; i < catalogue->annotation_count; ++i)
                if (strcmp(id, catalogue->annotations[i].id) == 0)
                {
                    entry->value.annotation = i;
                    status = UMI_STATUS_OK;
                    break;
                }
        }
        free(id);
    }
    if (status == UMI_STATUS_OK)
    {
        entry->value.text = entry->text;
        entry->value.text_bytes = strlen(entry->text);
    }
    return status;
}
static UmiStatus WorkspaceDocumentRead(const UmiJsonTree *tree, int uri, int edits, int version,
                                       int annotated, const UmiCancellationToken *cancel,
                                       UmiLanguageWorkspaceEditCatalogue *catalogue, size_t index)
{
    WorkspaceDocumentChange *entry = &catalogue->documents[index];
    UmiStatus status = LocationUri(tree, uri, &entry->uri);
    if (status == UMI_STATUS_OK && version >= 0 && !UmiJsonTreeIsNull(tree, version))
    {
        int64_t value;
        status = UmiJsonTreeInteger(tree, version, &value);
        if (status == UMI_STATUS_OK && (value < INT32_MIN || value > INT32_MAX))
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
        {
            entry->value.version = (int32_t)value;
            entry->value.has_version = 1;
        }
    }
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t previous = 0U; previous < index; ++previous)
        if (strcmp(catalogue->documents[previous].uri, entry->uri) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (UmiJsonTreeKind(tree, edits) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_PARSE_ERROR;
    entry->value.edit_count = UmiJsonTreeCount(tree, edits);
    if (entry->value.edit_count > 4096U - catalogue->edit_count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->edit_count += entry->value.edit_count;
    if (entry->value.edit_count != 0U)
    {
        entry->edits = calloc(entry->value.edit_count, sizeof(*entry->edits));
        if (entry->edits == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
    }
    int item = UmiJsonTreeFirst(tree, edits);
    for (size_t i = 0U; i < entry->value.edit_count; ++i, item = UmiJsonTreeNext(tree, item))
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        status = WorkspaceEditRead(tree, item, annotated, catalogue, &entry->edits[i]);
        if (status != UMI_STATUS_OK)
            return status;
    }
    entry->value.uri = entry->uri;
    return UMI_STATUS_OK;
}
static UmiStatus WorkspaceDocuments(const UmiJsonTree *tree, int node, int versioned,
                                    const UmiCancellationToken *cancel,
                                    UmiLanguageWorkspaceEditCatalogue *catalogue)
{
    if (node < 0)
        return UMI_STATUS_OK;
    if (UmiJsonTreeKind(tree, node) !=
        (versioned ? UMI_LANGUAGE_RUNTIME_JSON_ARRAY : UMI_LANGUAGE_RUNTIME_JSON_OBJECT))
        return UMI_STATUS_PARSE_ERROR;
    catalogue->count = UmiJsonTreeCount(tree, node);
    if (catalogue->count > 256U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (catalogue->count == 0U)
        return UMI_STATUS_OK;
    catalogue->documents = calloc(catalogue->count, sizeof(*catalogue->documents));
    if (catalogue->documents == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    int item = UmiJsonTreeFirst(tree, node);
    for (size_t i = 0U; i < catalogue->count; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        UmiStatus status;
        int uri, edits, version = -1;
        if (versioned)
        {
            int kind, document;
            status = LocationMember(tree, item, "kind", 0, &kind);
            if (status != UMI_STATUS_OK)
                return status;
            if (kind >= 0)
                return UMI_STATUS_NOT_IMPLEMENTED;
            status = LocationMember(tree, item, "textDocument", 1, &document);
            if (status == UMI_STATUS_OK)
                status = LocationMember(tree, item, "edits", 1, &edits);
            if (status == UMI_STATUS_OK)
                status = LocationMember(tree, document, "uri", 1, &uri);
            if (status == UMI_STATUS_OK)
                status = LocationMember(tree, document, "version", 1, &version);
            if (status != UMI_STATUS_OK)
                return status;
            item = UmiJsonTreeNext(tree, item);
        }
        else
        {
            uri = item;
            edits = UmiJsonTreeNext(tree, item);
            item = UmiJsonTreeNext(tree, edits);
        }
        status = WorkspaceDocumentRead(tree, uri, edits, version, versioned, cancel, catalogue, i);
        if (status != UMI_STATUS_OK)
            return status;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageWorkspaceEditCatalogueCreate(const void *json, size_t bytes,
                                                  const UmiCancellationToken *cancel,
                                                  UmiLanguageWorkspaceEditCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    UmiStatus status = UmiJsonTreeCreate(json, bytes, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageWorkspaceEditCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
    {
        UmiJsonTreeDestroy(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    if (!UmiJsonTreeIsNull(tree, 0))
    {
        int changes, documents, annotations;
        status = LocationMember(tree, 0, "changes", 0, &changes);
        if (status == UMI_STATUS_OK)
            status = LocationMember(tree, 0, "documentChanges", 0, &documents);
        if (status == UMI_STATUS_OK)
            status = LocationMember(tree, 0, "changeAnnotations", 0, &annotations);
        if (status == UMI_STATUS_OK && changes >= 0 &&
            UmiJsonTreeKind(tree, changes) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK)
            status = WorkspaceAnnotations(tree, annotations, cancel, catalogue);
        if (status == UMI_STATUS_OK)
            status = WorkspaceDocuments(tree, documents >= 0 ? documents : changes, documents >= 0, cancel,
                                        catalogue);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK)
        *out_catalogue = catalogue;
    else
        UmiLanguageWorkspaceEditCatalogueDestroy(catalogue);
    return status;
}
UmiStatus UmiLanguageWorkspaceEditCatalogueReadResponse(const void *json, size_t bytes,
                                                        uint64_t expected_request_id,
                                                        const UmiCancellationToken *cancel,
                                                        UmiLanguageWorkspaceEditCatalogue **out_catalogue)
{
    if (out_catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_catalogue = NULL;
    UmiJsonTree *tree = NULL;
    int result = -1;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    UmiStatus status =
        UmiLanguageResponseTreeRead(json, bytes, expected_request_id, &limits, cancel, &tree, &result);
    const char *span = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &span, &length);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageWorkspaceEditCatalogueCreate(span, length, cancel, out_catalogue);
    UmiJsonTreeDestroy(tree);
    return status;
}
void UmiLanguageWorkspaceEditCatalogueDestroy(UmiLanguageWorkspaceEditCatalogue *catalogue)
{
    if (catalogue == NULL)
        return;
    if (catalogue->documents != NULL)
        for (size_t i = 0U; i < catalogue->count; ++i)
        {
            WorkspaceDocumentChange *entry = &catalogue->documents[i];
            if (entry->edits != NULL)
                for (size_t j = 0U; j < entry->value.edit_count; ++j)
                    free(entry->edits[j].text);
            free(entry->edits);
            free(entry->uri);
        }
    if (catalogue->annotations != NULL)
        for (size_t i = 0U; i < catalogue->annotation_count; ++i)
        {
            free(catalogue->annotations[i].id);
            free(catalogue->annotations[i].label);
            free(catalogue->annotations[i].description);
        }
    free(catalogue->documents);
    free(catalogue->annotations);
    free(catalogue);
}
size_t UmiLanguageWorkspaceEditCatalogueCount(const UmiLanguageWorkspaceEditCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
size_t UmiLanguageWorkspaceEditCatalogueAnnotationCount(const UmiLanguageWorkspaceEditCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->annotation_count;
}
UmiStatus UmiLanguageWorkspaceEditCatalogueDocument(const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                                    size_t index, UmiLanguageWorkspaceDocumentChange *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->documents[index].value;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageWorkspaceEditCatalogueEdit(const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                                size_t document_index, size_t edit_index,
                                                UmiLanguageWorkspaceTextChange *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (document_index >= catalogue->count ||
        edit_index >= catalogue->documents[document_index].value.edit_count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->documents[document_index].edits[edit_index].value;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageWorkspaceEditCatalogueAnnotation(const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                                      size_t index, UmiLanguageWorkspaceChangeAnnotation *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->annotation_count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->annotations[index].value;
    return UMI_STATUS_OK;
}

/* Keep edit ordering in the canonical array. The existing preview owner sorts
 * resolved ranges and preserves the server's order for inserts at one point. */
static void WorkspaceWritePosition(UmiLanguageRuntimeJsonWriter *writer, UmiEditorTextPosition position)
{
    umi_language_runtime_json_writer_raw(writer, "{\"line\":");
    umi_language_runtime_json_writer_uint64(writer, position.line);
    umi_language_runtime_json_writer_raw(writer, ",\"character\":");
    umi_language_runtime_json_writer_uint64(writer, position.utf16_column);
    umi_language_runtime_json_writer_raw(writer, "}");
}
UmiStatus UmiLanguageWorkspaceEditCataloguePreview(const UmiLanguageWorkspaceEditCatalogue *catalogue,
                                                   size_t document_index, const char *source,
                                                   size_t source_bytes, size_t caret,
                                                   const int32_t *source_version,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageTextEditPreview **out_preview)
{
    if (out_preview == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_preview = NULL;
    if (catalogue == NULL || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (document_index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    const WorkspaceDocumentChange *document = &catalogue->documents[document_index];
    if (document->value.has_version && (source_version == NULL || document->value.version != *source_version))
        return UMI_STATUS_INVALID_STATE;
    if (document->value.edit_count > 256U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *json = malloc(1024U * 1024U + 1U);
    if (json == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, json, 1024U * 1024U + 1U);
    umi_language_runtime_json_writer_raw(&writer, "[");
    for (size_t i = 0U; writer.status == UMI_STATUS_OK && i < document->value.edit_count; ++i)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            writer.status = UMI_STATUS_CANCELLED;
            break;
        }
        const UmiLanguageWorkspaceTextChange *edit = &document->edits[i].value;
        if (i != 0U)
            umi_language_runtime_json_writer_raw(&writer, ",");
        umi_language_runtime_json_writer_raw(&writer, "{\"range\":{\"start\":");
        WorkspaceWritePosition(&writer, edit->range.start);
        umi_language_runtime_json_writer_raw(&writer, ",\"end\":");
        WorkspaceWritePosition(&writer, edit->range.end);
        umi_language_runtime_json_writer_raw(&writer, "},\"newText\":");
        umi_language_runtime_json_writer_string(&writer, edit->text);
        umi_language_runtime_json_writer_raw(&writer, "}");
    }
    umi_language_runtime_json_writer_raw(&writer, "]");
    UmiStatus status = writer.status;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageTextEditPreviewCreate(json, writer.length, source, source_bytes, caret, cancel,
                                                  out_preview);
    free(json);
    return status;
}

/* A caller that owns one captured draft must reject an entire multi-document
 * proposal. Selecting only the matching group could leave references broken. */
UmiStatus UmiLanguageWorkspaceEditCataloguePreviewSingleDocument(
    const UmiLanguageWorkspaceEditCatalogue *catalogue, const char *document_uri, const char *source,
    size_t source_bytes, size_t caret, const int32_t *source_version, const UmiCancellationToken *cancel,
    UmiLanguageTextEditPreview **out_preview)
{
    if (out_preview == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_preview = NULL;
    if (catalogue == NULL || document_uri == NULL || document_uri[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    size_t count = UmiLanguageWorkspaceEditCatalogueCount(catalogue);
    if (count == 0U)
        return UMI_STATUS_NOT_FOUND;
    if (count != 1U)
        return UMI_STATUS_NOT_IMPLEMENTED;
    UmiLanguageWorkspaceDocumentChange document;
    UmiStatus status = UmiLanguageWorkspaceEditCatalogueDocument(catalogue, 0U, &document);
    if (status == UMI_STATUS_OK && strcmp(document.uri, document_uri) != 0)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageWorkspaceEditCataloguePreview(catalogue, 0U, source, source_bytes, caret,
                                                          source_version, cancel, out_preview);
    return status;
}
