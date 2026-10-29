/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_text_index.c
 *
 * PURPOSE:
 *   Build an owned search snapshot from verified learning resources without execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_text_internal.h"
#include "umicom/browser/reading_document.h"
#include <stdio.h>
#include <stdlib.h>

void UmiLearningTextLimitsInit(UmiLearningTextLimits *limits)
{
    if (limits != NULL) *limits = (UmiLearningTextLimits){
        256U, UMI_LEARNING_TEXT_SOURCE_MAX, UMI_LEARNING_TEXT_DOCUMENT_MAX, 8U * 1024U * 1024U};
}
static int ValidLimits(const UmiLearningTextLimits *limits)
{
    return limits->maximum_documents > 0U &&
        limits->maximum_documents <= UMI_LEARNING_TEXT_DOCUMENTS_MAX &&
        limits->maximum_source_bytes > 0U &&
        limits->maximum_source_bytes <= UMI_LEARNING_TEXT_SOURCE_MAX &&
        limits->maximum_document_bytes > 0U &&
        limits->maximum_document_bytes <= UMI_LEARNING_TEXT_DOCUMENT_MAX &&
        limits->maximum_document_bytes <= limits->maximum_total_bytes &&
        limits->maximum_total_bytes > 0U &&
        limits->maximum_total_bytes <= UMI_LEARNING_TEXT_TOTAL_MAX;
}
static int HtmlResource(const char *name)
{
    const char *suffix = strrchr(name, '.');
    return suffix != NULL &&
        (LearningTextEqualFold(suffix, ".html") || LearningTextEqualFold(suffix, ".htm"));
}
static int MetadataText(const UmiLearningPackInfo *pack, const UmiLearningPackResource *resource)
{
    return LearningTextValid(pack->title, strlen(pack->title)) &&
        LearningTextValid(resource->title, strlen(resource->title)) &&
        LearningTextValid(resource->path, strlen(resource->path));
}
static UmiStatus IndexResource(UmiLearningTextIndex *index, const UmiLearningShelf *shelf,
    const UmiLearningPackInfo *pack, const UmiLearningPackResource *resource,
    const UmiLearningTextLimits *limits)
{
    if (index->info.document_count == limits->maximum_documents) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!MetadataText(pack, resource)) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t length = 0U;
    UmiStatus status = UmiLearningShelfReadResource(shelf, pack->id, resource->id,
        limits->maximum_source_bytes, &bytes, &length);
    UmiBrowserReadingDocument *reading = NULL;
    int html = HtmlResource(resource->path);
    const char *text = (const char *)bytes;
    if (status == UMI_STATUS_OK && !LearningTextValid(text, length)) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && html) {
        status = UmiBrowserReadingDocumentCreate(bytes, length, &reading);
        if (status == UMI_STATUS_OK) {
            text = UmiBrowserReadingDocumentText(reading);
            length = strlen(text);
        }
    }
    if (status == UMI_STATUS_OK && (length > limits->maximum_document_bytes ||
        length > limits->maximum_total_bytes - index->info.text_bytes))
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && !LearningTextValid(text, length)) status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK) {
        /* Fixed-width numeric identities preserve manifest order under Editor's
         * deterministic path/URI ordering. Actual resource paths stay in
         * copied metadata; these internal identifiers never resolve to files. */
        char uri[64];
        int n = snprintf(uri, sizeof(uri), "learning://%08zu", index->info.document_count);
        if (n < 0 || (size_t)n >= sizeof(uri)) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else {
            UmiEditorWorkspaceSearchDocumentInput input = {0};
            input.struct_size = (uint32_t)sizeof(input);
            input.api_version = UMI_EDITOR_WORKSPACE_SEARCH_INDEX_API_VERSION;
            input.uri = uri;
            input.relative_path = uri;
            /* Manifest resource IDs already obey the portable bounded identity
             * contract; long path leaf names must not narrow the pack contract. */
            input.file_name = resource->id;
            input.extension = "";
            input.language_id = "text";
            input.content = text;
            input.content_length = length;
            input.document_revision = pack->revision;
            input.read_only = 1;
            status = umi_editor_workspace_search_index_upsert(index->documents, &input);
        }
    }
    if (status == UMI_STATUS_OK) {
        LearningTextDocument *row = &index->metadata[index->info.document_count++];
        row->pack = *pack;
        row->resource = *resource;
        row->reading_projection = html;
        index->info.text_bytes += length;
    }
    UmiBrowserReadingDocumentDestroy(reading);
    UmiLearningPackFreeBytes(bytes);
    return status;
}
UmiStatus UmiLearningTextIndexCreate(const UmiLearningShelf *shelf,
    const UmiLearningTextLimits *supplied, UmiLearningTextCancellation cancellation,
    void *user, UmiLearningTextIndex **out, UmiLearningTextProblem *problem)
{
    UmiLearningTextProblem failure = {0};
    UmiLearningTextLimits limits;
    UmiLearningTextLimitsInit(&limits);
    if (supplied != NULL) limits = *supplied;
    if (out != NULL) *out = NULL;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    UmiLearningTextIndex *index = NULL;
    if (shelf == NULL || out == NULL || !ValidLimits(&limits)) goto finish;
    if (cancellation != NULL && cancellation(user)) { status = UMI_STATUS_CANCELLED; goto finish; }
    index = calloc(1U, sizeof(*index));
    if (index == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto finish; }
    index->metadata = calloc(limits.maximum_documents, sizeof(*index->metadata));
    if (index->metadata == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto finish; }
    UmiEditorWorkspaceSearchIndexConfig config;
    umi_editor_workspace_search_index_config_init(&config);
    config.maximum_documents = limits.maximum_documents;
    config.maximum_document_bytes = limits.maximum_document_bytes;
    config.maximum_total_bytes = limits.maximum_total_bytes;
    status = umi_editor_workspace_search_index_create(&config, &index->documents);
    UmiLearningShelfSnapshot snapshot;
    if (status == UMI_STATUS_OK) status = UmiLearningShelfCopySnapshot(shelf, &snapshot);
    if (status != UMI_STATUS_OK) goto finish;
    index->info.shelf_generation = snapshot.generation;
    index->info.pack_count = snapshot.pack_count;
    for (size_t p = 0U; status == UMI_STATUS_OK && p < snapshot.pack_count; ++p) {
        UmiLearningPackInfo pack;
        status = UmiLearningShelfPackAt(shelf, p, &pack);
        if (status != UMI_STATUS_OK) break;
        memcpy(failure.pack_id, pack.id, sizeof(failure.pack_id));
        failure.resource_id[0] = '\0';
        for (size_t n = 0U; status == UMI_STATUS_OK && n < pack.resource_count; ++n) {
            failure.resource_id[0] = '\0';
            if (cancellation != NULL && cancellation(user)) { status = UMI_STATUS_CANCELLED; break; }
            UmiLearningPackResource resource;
            status = UmiLearningShelfResourceAt(shelf, pack.id, n, &resource);
            if (status != UMI_STATUS_OK) break;
            memcpy(failure.resource_id, resource.id, sizeof(failure.resource_id));
            if (resource.kind == UMI_LEARNING_RESOURCE_IMAGE) { ++index->info.excluded_images; continue; }
            status = IndexResource(index, shelf, &pack, &resource, &limits);
        }
    }
    if (status == UMI_STATUS_OK && cancellation != NULL && cancellation(user)) {
        failure = (UmiLearningTextProblem){0};
        status = UMI_STATUS_CANCELLED;
    }
finish:
    if (status == UMI_STATUS_OK) {
        *out = index;
        if (problem != NULL) *problem = (UmiLearningTextProblem){0};
    } else {
        UmiLearningTextIndexDestroy(index);
        failure.status = status;
        if (problem != NULL) *problem = failure;
    }
    return status;
}
void UmiLearningTextIndexDestroy(UmiLearningTextIndex *index)
{
    if (index == NULL) return;
    umi_editor_workspace_search_index_destroy(index->documents);
    free(index->metadata);
    free(index);
}
UmiStatus UmiLearningTextIndexCopyInfo(const UmiLearningTextIndex *index,
    UmiLearningTextIndexInfo *out)
{
    if (index == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = index->info;
    return UMI_STATUS_OK;
}
