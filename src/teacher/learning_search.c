/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/learning_search.c
 *
 * PURPOSE:
 *   Search admitted metadata with bounded work and independently owned results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_search.h"
#include <stdlib.h>
#include <string.h>

struct UmiLearningSearch {
    UmiLearningSearchInfo info;
    UmiLearningSearchHit *hits;
};
typedef struct SearchTerm { const char *text; size_t size; } SearchTerm;
static unsigned char SearchFold(unsigned char value)
{
    return value >= 'A' && value <= 'Z' ? (unsigned char)(value + ('a' - 'A')) : value;
}
static UmiStatus SearchText(const char *text, size_t capacity, size_t *out_size)
{
    for (size_t i = 0U; i < capacity; ++i) {
        unsigned char value = (unsigned char)text[i];
        if (value == 0U) { *out_size = i; return UMI_STATUS_OK; }
        if (value < 32U || value == 127U) return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
static int SearchContains(const char *text, const SearchTerm *term)
{
    size_t size = strlen(text);
    if (term->size > size) return 0;
    for (size_t start = 0U; start <= size - term->size; ++start) {
        size_t i = 0U;
        while (i < term->size &&
               SearchFold((unsigned char)text[start + i]) == SearchFold((unsigned char)term->text[i])) ++i;
        if (i == term->size) return 1;
    }
    return 0;
}
static int SearchLocale(const char *left, const char *right)
{
    size_t a = strlen(left), b = strlen(right);
    if (a != b) return 0;
    for (size_t i = 0U; i < a; ++i)
        if (SearchFold((unsigned char)left[i]) != SearchFold((unsigned char)right[i])) return 0;
    return 1;
}
static int SearchMatch(const UmiLearningPackInfo *pack, const UmiLearningPackResource *resource,
    const SearchTerm *terms, size_t term_count, unsigned *out_fields)
{
    unsigned fields = 0U;
    for (size_t i = 0U; i < term_count; ++i) {
        unsigned found = 0U;
        if (SearchContains(resource->id, &terms[i])) found |= UMI_LEARNING_MATCH_ID;
        if (SearchContains(resource->title, &terms[i])) found |= UMI_LEARNING_MATCH_TITLE;
        if (SearchContains(resource->topic, &terms[i])) found |= UMI_LEARNING_MATCH_TOPIC;
        if (SearchContains(pack->title, &terms[i])) found |= UMI_LEARNING_MATCH_PACK_TITLE;
        if (found == 0U) return 0;
        fields |= found;
    }
    *out_fields = fields;
    return 1;
}
UmiStatus UmiLearningSearchCreate(const UmiLearningShelf *shelf,
    const UmiLearningSearchQuery *query, size_t maximum, UmiLearningSearch **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (shelf == NULL || query == NULL || maximum == 0U || maximum > UMI_LEARNING_SEARCH_MAX_RESULTS ||
        query->kind > (unsigned)UMI_LEARNING_RESOURCE_IMAGE) return UMI_STATUS_INVALID_ARGUMENT;
    size_t size = 0U, ignored = 0U;
    UmiStatus status = SearchText(query->text, sizeof(query->text), &size);
    if (status == UMI_STATUS_OK) status = SearchText(query->product, sizeof(query->product), &ignored);
    if (status == UMI_STATUS_OK) status = SearchText(query->locale, sizeof(query->locale), &ignored);
    if (status != UMI_STATUS_OK) return status;
    SearchTerm terms[UMI_LEARNING_SEARCH_MAX_TERMS] = {{0}};
    size_t term_count = 0U, cursor = 0U;
    while (cursor < size) {
        while (cursor < size && query->text[cursor] == ' ') ++cursor;
        if (cursor == size) break;
        if (term_count == UMI_LEARNING_SEARCH_MAX_TERMS) return UMI_STATUS_CAPACITY_EXCEEDED;
        size_t start = cursor;
        while (cursor < size && query->text[cursor] != ' ') ++cursor;
        terms[term_count++] = (SearchTerm){query->text + start, cursor - start};
    }
    UmiLearningSearch *search = calloc(1U, sizeof(*search));
    if (search == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    search->hits = calloc(maximum, sizeof(*search->hits));
    if (search->hits == NULL) { free(search); return UMI_STATUS_OUT_OF_MEMORY; }
    size_t packs = UmiLearningShelfCount(shelf);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < packs; ++i) {
        UmiLearningPackInfo pack;
        status = UmiLearningShelfPackAt(shelf, i, &pack);
        if (status != UMI_STATUS_OK) break;
        if ((query->product[0] != '\0' && strcmp(query->product, pack.product) != 0) ||
            (query->locale[0] != '\0' && !SearchLocale(query->locale, pack.locale))) continue;
        for (size_t j = 0U; j < pack.resource_count; ++j) {
            UmiLearningPackResource resource;
            status = UmiLearningShelfResourceAt(shelf, pack.id, j, &resource);
            if (status != UMI_STATUS_OK) break;
            ++search->info.examined_resources;
            if (query->kind != 0U && query->kind != (unsigned)resource.kind) continue;
            unsigned fields = 0U;
            if (!SearchMatch(&pack, &resource, terms, term_count, &fields)) continue;
            ++search->info.matching_resources;
            if (search->info.result_count < maximum) {
                UmiLearningSearchHit *hit = &search->hits[search->info.result_count++];
                hit->pack = pack;
                hit->resource = resource;
                hit->matched_fields = fields;
            }
        }
    }
    if (status != UMI_STATUS_OK) { UmiLearningSearchDestroy(search); return status; }
    search->info.truncated = search->info.result_count != search->info.matching_resources;
    *out = search;
    return UMI_STATUS_OK;
}
void UmiLearningSearchDestroy(UmiLearningSearch *search)
{
    if (search == NULL) return;
    free(search->hits);
    free(search);
}
UmiStatus UmiLearningSearchCopyInfo(const UmiLearningSearch *search, UmiLearningSearchInfo *out)
{
    if (search == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = search->info;
    return UMI_STATUS_OK;
}
UmiStatus UmiLearningSearchResultAt(const UmiLearningSearch *search, size_t index, UmiLearningSearchHit *out)
{
    if (search == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= search->info.result_count) return UMI_STATUS_NOT_FOUND;
    *out = search->hits[index];
    return UMI_STATUS_OK;
}
