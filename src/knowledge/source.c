/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/source.c
 * PURPOSE: Implement source initialisation and incremental-refresh decisions.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/source.h"

#include <stdio.h>
#include <string.h>

static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

UmiStatus umi_knowledge_source_init(UmiKnowledgeSource *source,
                                    const char *source_id,
                                    const char *collection_id,
                                    const char *title,
                                    const char *uri,
                                    UmiKnowledgeSourceKind kind)
{
    if (source == NULL || source_id == NULL || source_id[0] == '\0' ||
        collection_id == NULL || collection_id[0] == '\0' ||
        title == NULL || title[0] == '\0' || uri == NULL ||
        uri[0] == '\0' || kind < UMI_KNOWLEDGE_SOURCE_DOCUMENT ||
        kind > UMI_KNOWLEDGE_SOURCE_GENERATED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(source, 0, sizeof(*source));
    if (!copy_text(source->source_id, sizeof(source->source_id), source_id) ||
        !copy_text(source->collection_id, sizeof(source->collection_id),
                   collection_id) ||
        !copy_text(source->title, sizeof(source->title), title) ||
        !copy_text(source->uri, sizeof(source->uri), uri) ||
        !copy_text(source->language, sizeof(source->language), "und") ||
        !copy_text(source->media_type, sizeof(source->media_type),
                   "text/plain")) return UMI_STATUS_CAPACITY_EXCEEDED;
    source->kind = kind;
    source->classification = UMI_AI_DATA_INTERNAL;
    source->revision = 1U;
    source->active = 1;
    return UMI_STATUS_OK;
}

UmiKnowledgeRefreshDecision umi_knowledge_source_refresh_decision(
    const UmiKnowledgeSource *current,
    const UmiKnowledgeSource *candidate)
{
    if (candidate == NULL) return UMI_KNOWLEDGE_REFRESH_REPLACE;
    if (current == NULL || current->source_id[0] == '\0') {
        return UMI_KNOWLEDGE_REFRESH_NEW;
    }
    if (strcmp(current->source_id, candidate->source_id) == 0 &&
        current->content_hash == candidate->content_hash &&
        current->content_hash != 0U && current->active == candidate->active) {
        return UMI_KNOWLEDGE_REFRESH_UNCHANGED;
    }
    return UMI_KNOWLEDGE_REFRESH_REPLACE;
}
