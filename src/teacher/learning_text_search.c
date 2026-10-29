/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_text_search.c
 *
 * PURPOSE:
 *   Return independently owned phrase matches over verified learning snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "learning_text_internal.h"
#include "umicom/editor/workspace_search_pattern.h"
#include <stdlib.h>

struct UmiLearningTextSearch {
    UmiLearningTextSearchInfo info;
    UmiLearningTextHit *hits;
};
void UmiLearningTextQueryInit(UmiLearningTextQuery *query)
{
    if (query != NULL) {
        *query = (UmiLearningTextQuery){0};
        query->maximum_results = 64U;
    }
}
static UmiStatus QueryText(const char *text, size_t capacity, size_t *length)
{
    const char *end = memchr(text, '\0', capacity);
    if (end == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    *length = (size_t)(end - text);
    if (!UmiUtf8TextValid(text, *length)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < *length; ++i)
        if ((unsigned char)text[i] < 32U || (unsigned char)text[i] == 127U) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static UmiStatus QueryValid(const UmiLearningTextQuery *query)
{
    if (query == NULL || query->maximum_results == 0U ||
        query->maximum_results > UMI_LEARNING_TEXT_RESULTS_MAX ||
        query->kind > (unsigned)UMI_LEARNING_RESOURCE_IMAGE ||
        (query->case_sensitive != 0 && query->case_sensitive != 1) ||
        (query->whole_word != 0 && query->whole_word != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length;
    UmiStatus status = QueryText(query->phrase, sizeof(query->phrase), &length);
    if (status != UMI_STATUS_OK) return status;
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = QueryText(query->product, sizeof(query->product), &length);
    if (status == UMI_STATUS_OK) status = QueryText(query->locale, sizeof(query->locale), &length);
    return status;
}
UmiStatus UmiLearningTextQueryValidate(const UmiLearningTextQuery *query)
{
    return QueryValid(query);
}
static void CopyPreview(UmiLearningTextHit *hit, const char *text, size_t length)
{
    size_t line_end = hit->byte_start;
    while (line_end < length && text[line_end] != '\r' && text[line_end] != '\n') ++line_end;
    size_t end = line_end;
    if (end - hit->byte_start >= sizeof(hit->preview))
        end = hit->byte_start + sizeof(hit->preview) - 1U;
    /* The complete indexed document is valid UTF-8. Backing away from a
     * continuation byte leaves the preview on a scalar boundary. */
    while (end > hit->byte_start && end < length &&
        ((unsigned char)text[end] & 0xc0U) == 0x80U) --end;
    size_t bytes = end - hit->byte_start;
    memcpy(hit->preview, text + hit->byte_start, bytes);
    hit->preview[bytes] = '\0';
    hit->preview_truncated = end < line_end;
}
UmiStatus UmiLearningTextSearchCreate(const UmiLearningTextIndex *index,
    const UmiLearningTextQuery *query, UmiLearningTextSearch **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Native dispatch and direct SDK consumers share the same bounded query contract. */
    UmiStatus status = UmiLearningTextQueryValidate(query);
    if (status != UMI_STATUS_OK) return status;
    if (query->cancellation != NULL && query->cancellation(query->cancellation_user))
        return UMI_STATUS_CANCELLED;
    UmiLearningTextSearch *search = calloc(1U, sizeof(*search));
    if (search == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    search->hits = calloc(query->maximum_results, sizeof(*search->hits));
    if (search->hits == NULL) { UmiLearningTextSearchDestroy(search); return UMI_STATUS_OUT_OF_MEMORY; }
    search->info.shelf_generation = index->info.shelf_generation;
    UmiEditorWorkspaceSearchPattern *pattern = NULL;
    status = umi_editor_workspace_search_pattern_create(&pattern);
    UmiEditorWorkspaceSearchPatternRequest request;
    umi_editor_workspace_search_pattern_request_init(&request, query->phrase);
    request.mode = UMI_EDITOR_WORKSPACE_SEARCH_PATTERN_LITERAL;
    request.case_mode = query->case_sensitive ? UMI_EDITOR_WORKSPACE_SEARCH_CASE_SENSITIVE :
        UMI_EDITOR_WORKSPACE_SEARCH_CASE_INSENSITIVE;
    request.whole_word = query->whole_word;
    request.maximum_execution_steps = UINT64_C(33554432);
    if (status == UMI_STATUS_OK) status = umi_editor_workspace_search_pattern_compile(pattern, &request, NULL);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < index->info.document_count; ++i) {
        if (query->cancellation != NULL && query->cancellation(query->cancellation_user)) {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        const LearningTextDocument *metadata = &index->metadata[i];
        if ((query->product[0] != '\0' && strcmp(query->product, metadata->pack.product) != 0) ||
            (query->locale[0] != '\0' && !LearningTextEqualFold(query->locale, metadata->pack.locale)) ||
            (query->kind != 0U && query->kind != (unsigned)metadata->resource.kind)) continue;
        ++search->info.examined_documents;
        UmiEditorWorkspaceSearchDocumentView document;
        status = umi_editor_workspace_search_index_at(index->documents, i, &document);
        if (status != UMI_STATUS_OK) break;
        UmiEditorWorkspaceSearchPatternMatch match;
        status = umi_editor_workspace_search_pattern_find(pattern, document.content, document.content_length, 0U, &match);
        if (status == UMI_STATUS_NOT_FOUND) { status = UMI_STATUS_OK; continue; }
        if (status != UMI_STATUS_OK) break;
        ++search->info.matching_documents;
        if (search->info.result_count == query->maximum_results) continue;
        UmiLearningTextHit *hit = &search->hits[search->info.result_count++];
        hit->pack = metadata->pack;
        hit->resource = metadata->resource;
        hit->reading_projection = metadata->reading_projection;
        hit->byte_start = (size_t)match.start_byte_offset;
        hit->byte_end = (size_t)match.end_byte_offset;
        CopyPreview(hit, document.content, document.content_length);
    }
    if (status == UMI_STATUS_OK && query->cancellation != NULL &&
        query->cancellation(query->cancellation_user)) status = UMI_STATUS_CANCELLED;
    umi_editor_workspace_search_pattern_destroy(pattern);
    if (status != UMI_STATUS_OK) { UmiLearningTextSearchDestroy(search); return status; }
    search->info.truncated = search->info.result_count != search->info.matching_documents;
    *out = search;
    return UMI_STATUS_OK;
}
void UmiLearningTextSearchDestroy(UmiLearningTextSearch *search)
{
    if (search == NULL) return;
    free(search->hits);
    free(search);
}
UmiStatus UmiLearningTextSearchCopyInfo(const UmiLearningTextSearch *search, UmiLearningTextSearchInfo *out)
{
    if (search == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = search->info;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningTextSearchResultAt(const UmiLearningTextSearch *search, size_t position, UmiLearningTextHit *out)
{
    if (search == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= search->info.result_count) return UMI_STATUS_NOT_FOUND;
    *out = search->hits[position];
    return UMI_STATUS_OK;
}
