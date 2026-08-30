/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_search_index.h
 *
 * PURPOSE:
 *   Define the Framework-owned, provider-neutral document index consumed by
 *   workspace search and replace. File-system, remote-workspace, generated-
 *   source and in-memory editor adapters populate the same index contract.
 *
 * DESIGN:
 *   The index copies document content and metadata. A document view returned by
 *   this API contains a borrowed content pointer that remains valid until the
 *   next mutation of the index. Callers that retain content must make a copy.
 *
 * OWNERSHIP:
 *   - The index owns all copied documents.
 *   - Input pointers are borrowed only for the duration of an upsert call.
 *   - Document-view structures are caller-owned values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_SEARCH_INDEX_H
#define UMICOM_EDITOR_WORKSPACE_SEARCH_INDEX_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION 1U
#define UMI_EDITOR_WORKSPACE_SEARCH_URI_CAPACITY 1024U
#define UMI_EDITOR_WORKSPACE_SEARCH_PATH_CAPACITY 1024U
#define UMI_EDITOR_WORKSPACE_SEARCH_FILE_NAME_CAPACITY 256U
#define UMI_EDITOR_WORKSPACE_SEARCH_EXTENSION_CAPACITY 64U
#define UMI_EDITOR_WORKSPACE_SEARCH_INDEX_LANGUAGE_CAPACITY 64U

typedef struct UmiEditorWorkspaceSearchIndexConfig {
    uint32_t struct_size;
    uint32_t api_version;
    size_t maximum_documents;
    size_t maximum_document_bytes;
    size_t maximum_total_bytes;
} UmiEditorWorkspaceSearchIndexConfig;

typedef struct UmiEditorWorkspaceSearchDocumentInput {
    uint32_t struct_size;
    uint32_t api_version;
    const char *uri;
    const char *relative_path;
    const char *file_name;
    const char *extension;
    const char *language_id;
    const char *content;
    size_t content_length;
    uint64_t document_revision;
    uint64_t modified_nanoseconds;
    int hidden;
    int generated;
    int binary;
    int read_only;
} UmiEditorWorkspaceSearchDocumentInput;

/**
 * Borrowed immutable document view. content is valid until the next successful
 * clear, upsert or remove call on the owning index.
 */
typedef struct UmiEditorWorkspaceSearchDocumentView {
    uint32_t struct_size;
    uint32_t api_version;
    char uri[UMI_EDITOR_WORKSPACE_SEARCH_URI_CAPACITY];
    char relative_path[UMI_EDITOR_WORKSPACE_SEARCH_PATH_CAPACITY];
    char file_name[UMI_EDITOR_WORKSPACE_SEARCH_FILE_NAME_CAPACITY];
    char extension[UMI_EDITOR_WORKSPACE_SEARCH_EXTENSION_CAPACITY];
    char language_id[UMI_EDITOR_WORKSPACE_SEARCH_INDEX_LANGUAGE_CAPACITY];
    const char *content;
    size_t content_length;
    uint64_t document_revision;
    uint64_t modified_nanoseconds;
    uint64_t fingerprint;
    int hidden;
    int generated;
    int binary;
    int read_only;
} UmiEditorWorkspaceSearchDocumentView;

typedef struct UmiEditorWorkspaceSearchIndexSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t document_count;
    size_t text_document_count;
    size_t binary_document_count;
    size_t generated_document_count;
    size_t read_only_document_count;
    size_t total_content_bytes;
    size_t maximum_documents;
    size_t maximum_document_bytes;
    size_t maximum_total_bytes;
    uint64_t revision;
} UmiEditorWorkspaceSearchIndexSnapshot;

typedef struct UmiEditorWorkspaceSearchIndex UmiEditorWorkspaceSearchIndex;

void umi_editor_workspace_search_index_config_init(
    UmiEditorWorkspaceSearchIndexConfig *config);

UmiStatus umi_editor_workspace_search_index_create(
    const UmiEditorWorkspaceSearchIndexConfig *config,
    UmiEditorWorkspaceSearchIndex **out_index);
void umi_editor_workspace_search_index_destroy(
    UmiEditorWorkspaceSearchIndex *index);

UmiStatus umi_editor_workspace_search_index_clear(
    UmiEditorWorkspaceSearchIndex *index);
UmiStatus umi_editor_workspace_search_index_upsert(
    UmiEditorWorkspaceSearchIndex *index,
    const UmiEditorWorkspaceSearchDocumentInput *input);
UmiStatus umi_editor_workspace_search_index_remove(
    UmiEditorWorkspaceSearchIndex *index,
    const char *uri);

UmiStatus umi_editor_workspace_search_index_at(
    const UmiEditorWorkspaceSearchIndex *index,
    size_t position,
    UmiEditorWorkspaceSearchDocumentView *out_document);
UmiStatus umi_editor_workspace_search_index_find_uri(
    const UmiEditorWorkspaceSearchIndex *index,
    const char *uri,
    UmiEditorWorkspaceSearchDocumentView *out_document);
UmiStatus umi_editor_workspace_search_index_snapshot(
    const UmiEditorWorkspaceSearchIndex *index,
    UmiEditorWorkspaceSearchIndexSnapshot *out_snapshot);

size_t umi_editor_workspace_search_index_count(
    const UmiEditorWorkspaceSearchIndex *index);
uint64_t umi_editor_workspace_search_index_revision(
    const UmiEditorWorkspaceSearchIndex *index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_SEARCH_INDEX_H */
