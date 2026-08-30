/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_search_index.c
 *
 * PURPOSE:
 *   Implement bounded, deterministic storage for workspace-search documents.
 *   The index is populated by adapters and deliberately performs no direct
 *   file-system access, keeping the editor library portable and testable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_search_index.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_MAXIMUM_DOCUMENTS 100000U
#define DEFAULT_MAXIMUM_DOCUMENT_BYTES (16U * 1024U * 1024U)
#define DEFAULT_MAXIMUM_TOTAL_BYTES (512U * 1024U * 1024U)

typedef struct StoredDocument {
    UmiEditorWorkspaceSearchDocumentView view;
    char *owned_content;
} StoredDocument;

struct UmiEditorWorkspaceSearchIndex {
    UmiEditorWorkspaceSearchIndexConfig config;
    StoredDocument *documents;
    size_t count;
    size_t capacity;
    size_t total_bytes;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int copy_text(char *destination,
                     size_t capacity,
                     const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    length = strlen(source);
    if (length >= capacity) return 0;
    (void)memcpy(destination, source, length + 1U);
    return 1;
}

static uint64_t fingerprint_content(const char *content, size_t length)
{
    uint64_t value = UINT64_C(1469598103934665603);
    size_t index;
    for (index = 0U; index < length; ++index) {
        value ^= (uint64_t)(unsigned char)content[index];
        value *= UINT64_C(1099511628211);
    }
    value ^= (uint64_t)length;
    value *= UINT64_C(1099511628211);
    return value;
}

static int content_looks_binary(const char *content, size_t length)
{
    size_t index;
    size_t limit = length < 4096U ? length : 4096U;
    for (index = 0U; index < limit; ++index) {
        if (content[index] == '\0') return 1;
    }
    return 0;
}

static const char *last_separator(const char *path)
{
    const char *last_forward;
    const char *last_backward;
    if (path == NULL) return NULL;
    last_forward = strrchr(path, '/');
    last_backward = strrchr(path, '\\');
    if (last_forward == NULL) return last_backward;
    if (last_backward == NULL) return last_forward;
    return last_forward > last_backward ? last_forward : last_backward;
}

static int derive_file_name(char *out_name,
                            size_t capacity,
                            const char *relative_path,
                            const char *explicit_name)
{
    const char *separator;
    const char *source;
    if (explicit_name != NULL && explicit_name[0] != '\0') {
        return copy_text(out_name, capacity, explicit_name);
    }
    separator = last_separator(relative_path);
    source = separator != NULL ? separator + 1 : relative_path;
    return source != NULL && source[0] != '\0' &&
           copy_text(out_name, capacity, source);
}

static int derive_extension(char *out_extension,
                            size_t capacity,
                            const char *file_name,
                            const char *explicit_extension)
{
    const char *dot;
    if (explicit_extension != NULL && explicit_extension[0] != '\0') {
        return copy_text(out_extension, capacity, explicit_extension);
    }
    dot = strrchr(file_name, '.');
    if (dot == NULL || dot == file_name || dot[1] == '\0') {
        out_extension[0] = '\0';
        return 1;
    }
    return copy_text(out_extension, capacity, dot + 1);
}

static UmiStatus validate_config(
    const UmiEditorWorkspaceSearchIndexConfig *config)
{
    if (config == NULL ||
        config->struct_size != (uint32_t)sizeof(*config) ||
        config->api_version != UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION ||
        config->maximum_documents == 0U ||
        config->maximum_document_bytes == 0U ||
        config->maximum_total_bytes == 0U ||
        config->maximum_document_bytes > config->maximum_total_bytes) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus validate_input(
    const UmiEditorWorkspaceSearchIndex *index,
    const UmiEditorWorkspaceSearchDocumentInput *input)
{
    if (index == NULL || input == NULL ||
        input->struct_size != (uint32_t)sizeof(*input) ||
        input->api_version != UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION ||
        input->uri == NULL || input->uri[0] == '\0' ||
        input->relative_path == NULL || input->relative_path[0] == '\0' ||
        input->language_id == NULL || input->content == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(input->uri) >= UMI_EDITOR_WORKSPACE_SEARCH_URI_CAPACITY ||
        strlen(input->relative_path) >=
            UMI_EDITOR_WORKSPACE_SEARCH_PATH_CAPACITY ||
        strlen(input->language_id) >=
            UMI_EDITOR_WORKSPACE_SEARCH_INDEX_LANGUAGE_CAPACITY ||
        (input->file_name != NULL &&
         strlen(input->file_name) >=
             UMI_EDITOR_WORKSPACE_SEARCH_FILE_NAME_CAPACITY) ||
        (input->extension != NULL &&
         strlen(input->extension) >=
             UMI_EDITOR_WORKSPACE_SEARCH_EXTENSION_CAPACITY)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (input->content_length == SIZE_MAX ||
        input->content_length > index->config.maximum_document_bytes) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

static size_t find_document(const UmiEditorWorkspaceSearchIndex *index,
                            const char *uri)
{
    size_t position;
    if (index == NULL || uri == NULL) return SIZE_MAX;
    for (position = 0U; position < index->count; ++position) {
        if (strcmp(index->documents[position].view.uri, uri) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

static UmiStatus reserve_documents(UmiEditorWorkspaceSearchIndex *index,
                                   size_t required)
{
    size_t capacity;
    StoredDocument *replacement;
    if (required <= index->capacity) return UMI_STATUS_OK;
    if (required > index->config.maximum_documents) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capacity = index->capacity > 0U ? index->capacity : 32U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > index->config.maximum_documents) {
        capacity = index->config.maximum_documents;
    }
    if (capacity < required || capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (StoredDocument *)realloc(
        index->documents, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index->documents = replacement;
    index->capacity = capacity;
    return UMI_STATUS_OK;
}

static int compare_documents(const void *left_pointer,
                             const void *right_pointer)
{
    const StoredDocument *left = (const StoredDocument *)left_pointer;
    const StoredDocument *right = (const StoredDocument *)right_pointer;
    int order = strcmp(left->view.relative_path, right->view.relative_path);
    if (order != 0) return order;
    return strcmp(left->view.uri, right->view.uri);
}

static void release_document(StoredDocument *document)
{
    if (document == NULL) return;
    free(document->owned_content);
    document->owned_content = NULL;
    document->view.content = NULL;
    document->view.content_length = 0U;
}

void umi_editor_workspace_search_index_config_init(
    UmiEditorWorkspaceSearchIndexConfig *config)
{
    if (config == NULL) return;
    (void)memset(config, 0, sizeof(*config));
    config->struct_size = (uint32_t)sizeof(*config);
    config->api_version = UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
    config->maximum_documents = DEFAULT_MAXIMUM_DOCUMENTS;
    config->maximum_document_bytes = DEFAULT_MAXIMUM_DOCUMENT_BYTES;
    config->maximum_total_bytes = DEFAULT_MAXIMUM_TOTAL_BYTES;
}

UmiStatus umi_editor_workspace_search_index_create(
    const UmiEditorWorkspaceSearchIndexConfig *config,
    UmiEditorWorkspaceSearchIndex **out_index)
{
    UmiEditorWorkspaceSearchIndexConfig effective;
    UmiEditorWorkspaceSearchIndex *index;
    if (out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = NULL;
    if (config == NULL) {
        umi_editor_workspace_search_index_config_init(&effective);
        config = &effective;
    }
    if (validate_config(config) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = (UmiEditorWorkspaceSearchIndex *)calloc(1U, sizeof(*index));
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index->config = *config;
    index->revision = 1U;
    *out_index = index;
    return UMI_STATUS_OK;
}

void umi_editor_workspace_search_index_destroy(
    UmiEditorWorkspaceSearchIndex *index)
{
    size_t position;
    if (index == NULL) return;
    for (position = 0U; position < index->count; ++position) {
        release_document(&index->documents[position]);
    }
    free(index->documents);
    index->documents = NULL;
    free(index);
}

UmiStatus umi_editor_workspace_search_index_clear(
    UmiEditorWorkspaceSearchIndex *index)
{
    size_t position;
    if (index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (position = 0U; position < index->count; ++position) {
        release_document(&index->documents[position]);
    }
    index->count = 0U;
    index->total_bytes = 0U;
    index->revision = next_revision(index->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_index_upsert(
    UmiEditorWorkspaceSearchIndex *index,
    const UmiEditorWorkspaceSearchDocumentInput *input)
{
    StoredDocument replacement;
    size_t position;
    size_t existing_length = 0U;
    size_t projected_total;
    UmiStatus status;

    status = validate_input(index, input);
    if (status != UMI_STATUS_OK) return status;
    position = find_document(index, input->uri);
    if (position != SIZE_MAX) {
        existing_length = index->documents[position].view.content_length;
    }
    if (input->content_length > SIZE_MAX - (index->total_bytes - existing_length)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    projected_total = index->total_bytes - existing_length +
                      input->content_length;
    if (projected_total > index->config.maximum_total_bytes) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(&replacement, 0, sizeof(replacement));
    replacement.owned_content = (char *)malloc(input->content_length + 1U);
    if (replacement.owned_content == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (input->content_length > 0U) {
        (void)memcpy(replacement.owned_content,
                     input->content,
                     input->content_length);
    }
    replacement.owned_content[input->content_length] = '\0';
    replacement.view.struct_size = (uint32_t)sizeof(replacement.view);
    replacement.view.api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
    if (!copy_text(replacement.view.uri,
                   sizeof(replacement.view.uri),
                   input->uri) ||
        !copy_text(replacement.view.relative_path,
                   sizeof(replacement.view.relative_path),
                   input->relative_path) ||
        !derive_file_name(replacement.view.file_name,
                          sizeof(replacement.view.file_name),
                          input->relative_path,
                          input->file_name) ||
        !derive_extension(replacement.view.extension,
                          sizeof(replacement.view.extension),
                          replacement.view.file_name,
                          input->extension) ||
        !copy_text(replacement.view.language_id,
                   sizeof(replacement.view.language_id),
                   input->language_id)) {
        release_document(&replacement);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement.view.content = replacement.owned_content;
    replacement.view.content_length = input->content_length;
    replacement.view.document_revision = input->document_revision;
    replacement.view.modified_nanoseconds = input->modified_nanoseconds;
    replacement.view.fingerprint = fingerprint_content(
        replacement.owned_content, input->content_length);
    replacement.view.hidden = input->hidden != 0;
    replacement.view.generated = input->generated != 0;
    replacement.view.binary = input->binary != 0 ||
        content_looks_binary(replacement.owned_content, input->content_length);
    replacement.view.read_only = input->read_only != 0;

    if (position == SIZE_MAX) {
        status = reserve_documents(index, index->count + 1U);
        if (status != UMI_STATUS_OK) {
            release_document(&replacement);
            return status;
        }
        position = index->count++;
    } else {
        release_document(&index->documents[position]);
    }
    index->documents[position] = replacement;
    index->total_bytes = projected_total;
    if (index->count > 1U) {
        qsort(index->documents,
              index->count,
              sizeof(*index->documents),
              compare_documents);
    }
    index->revision = next_revision(index->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_index_remove(
    UmiEditorWorkspaceSearchIndex *index,
    const char *uri)
{
    size_t position;
    size_t removed_length;
    if (index == NULL || uri == NULL || uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_document(index, uri);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    removed_length = index->documents[position].view.content_length;
    release_document(&index->documents[position]);
    if (position + 1U < index->count) {
        (void)memmove(&index->documents[position],
                      &index->documents[position + 1U],
                      (index->count - position - 1U) *
                          sizeof(*index->documents));
    }
    --index->count;
    index->total_bytes -= removed_length;
    index->revision = next_revision(index->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_index_at(
    const UmiEditorWorkspaceSearchIndex *index,
    size_t position,
    UmiEditorWorkspaceSearchDocumentView *out_document)
{
    if (index == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= index->count) return UMI_STATUS_NOT_FOUND;
    *out_document = index->documents[position].view;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_index_find_uri(
    const UmiEditorWorkspaceSearchIndex *index,
    const char *uri,
    UmiEditorWorkspaceSearchDocumentView *out_document)
{
    size_t position;
    if (index == NULL || uri == NULL || uri[0] == '\0' ||
        out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_document(index, uri);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_document = index->documents[position].view;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_index_snapshot(
    const UmiEditorWorkspaceSearchIndex *index,
    UmiEditorWorkspaceSearchIndexSnapshot *out_snapshot)
{
    size_t position;
    if (index == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
    out_snapshot->document_count = index->count;
    out_snapshot->total_content_bytes = index->total_bytes;
    out_snapshot->maximum_documents = index->config.maximum_documents;
    out_snapshot->maximum_document_bytes =
        index->config.maximum_document_bytes;
    out_snapshot->maximum_total_bytes = index->config.maximum_total_bytes;
    out_snapshot->revision = index->revision;
    for (position = 0U; position < index->count; ++position) {
        const UmiEditorWorkspaceSearchDocumentView *view =
            &index->documents[position].view;
        if (view->binary) {
            ++out_snapshot->binary_document_count;
        } else {
            ++out_snapshot->text_document_count;
        }
        if (view->generated) ++out_snapshot->generated_document_count;
        if (view->read_only) ++out_snapshot->read_only_document_count;
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_workspace_search_index_count(
    const UmiEditorWorkspaceSearchIndex *index)
{
    return index != NULL ? index->count : 0U;
}

uint64_t umi_editor_workspace_search_index_revision(
    const UmiEditorWorkspaceSearchIndex *index)
{
    return index != NULL ? index->revision : 0U;
}
