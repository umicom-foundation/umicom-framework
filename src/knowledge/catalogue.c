/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/catalogue.c
 * PURPOSE: Implement the bounded source catalogue used for incremental refresh.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/catalogue.h"

#include <stdlib.h>
#include <string.h>

struct UmiKnowledgeCatalogue {
    UmiKnowledgeSource *sources;
    size_t count;
    size_t capacity;
};

/*
 * Initialise knowledge catalogue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_knowledge_catalogue_create(
    size_t capacity, UmiKnowledgeCatalogue **out_catalogue)
{
    UmiKnowledgeCatalogue *catalogue;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_catalogue == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_catalogue = NULL;
    catalogue = (UmiKnowledgeCatalogue *)calloc(1U, sizeof(*catalogue));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    catalogue->sources = (UmiKnowledgeSource *)calloc(
        capacity, sizeof(*catalogue->sources));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue->sources == NULL) {
        free(catalogue);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    catalogue->capacity = capacity;
    *out_catalogue = catalogue;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by knowledge catalogue so the same storage can be reused
 * safely.
 */
void umi_knowledge_catalogue_destroy(UmiKnowledgeCatalogue *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return;
    free(catalogue->sources);
    free(catalogue);
}

/*
 * Provide the knowledge catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_knowledge_catalogue_upsert(UmiKnowledgeCatalogue *catalogue,
                                         const UmiKnowledgeSource *source)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || source == NULL || source->source_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < catalogue->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(catalogue->sources[position].source_id,
                   source->source_id) == 0) {
            catalogue->sources[position] = *source;
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= catalogue->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->sources[catalogue->count++] = *source;
    return UMI_STATUS_OK;
}

/*
 * Find knowledge catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_knowledge_catalogue_find(
    const UmiKnowledgeCatalogue *catalogue,
    const char *source_id,
    UmiKnowledgeSource *out_source)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || source_id == NULL || out_source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < catalogue->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(catalogue->sources[position].source_id, source_id) == 0) {
            *out_source = catalogue->sources[position];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Remove knowledge catalogue while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_knowledge_catalogue_remove(UmiKnowledgeCatalogue *catalogue,
                                         const char *source_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || source_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < catalogue->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(catalogue->sources[position].source_id, source_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (position + 1U < catalogue->count) {
                (void)memmove(&catalogue->sources[position],
                              &catalogue->sources[position + 1U],
                              (catalogue->count - position - 1U) *
                                  sizeof(*catalogue->sources));
            }
            --catalogue->count;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by knowledge catalogue without changing their
 * state.
 */
size_t umi_knowledge_catalogue_count(
    const UmiKnowledgeCatalogue *catalogue)
{
    return catalogue != NULL ? catalogue->count : 0U;
}

/*
 * Find knowledge catalogue source while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_knowledge_catalogue_source_at(
    const UmiKnowledgeCatalogue *catalogue,
    size_t position,
    UmiKnowledgeSource *out_source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || out_source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= catalogue->count) return UMI_STATUS_NOT_FOUND;
    *out_source = catalogue->sources[position];
    return UMI_STATUS_OK;
}
