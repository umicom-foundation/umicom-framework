/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/service.c
 * PURPOSE: Compose Native RAG collections, ingestion and retrieval services.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiKnowledgeService {
    UmiKnowledgeServiceConfig config;
    UmiKnowledgeEmbeddingRegistry providers;
    UmiKnowledgeCollection collections[UMI_KNOWLEDGE_COLLECTION_MAX];
    size_t collection_count;
    UmiKnowledgeCatalogue *catalogue;
    UmiKnowledgeVectorIndex *index;
    uint64_t revision;
    char last_source_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char last_query[UMI_KNOWLEDGE_TEXT_CAPACITY];
    size_t last_result_count;
};

UmiKnowledgeServiceConfig umi_knowledge_service_config_default(void)
{
    UmiKnowledgeServiceConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.source_capacity = 1024U;
    config.vector_capacity = 8192U;
    config.embedding_dimension = 64U;
    config.metric = UMI_KNOWLEDGE_DISTANCE_COSINE;
    config.chunk_policy = umi_knowledge_chunk_policy_default();
    (void)snprintf(config.default_provider_id,
                   sizeof(config.default_provider_id), "%s",
                   "umicom.local.hash");
    return config;
}

UmiStatus umi_knowledge_service_create(
    const UmiKnowledgeServiceConfig *config,
    UmiKnowledgeService **out_service)
{
    UmiKnowledgeService *service;
    UmiKnowledgeEmbeddingProvider provider;
    UmiStatus status;
    if (config == NULL || out_service == NULL ||
        config->source_capacity == 0U || config->vector_capacity == 0U ||
        config->default_provider_id[0] == '\0' ||
        umi_knowledge_chunk_policy_validate(&config->chunk_policy) !=
            UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiKnowledgeService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->config = *config;
    umi_knowledge_embedding_registry_init(&service->providers);
    status = umi_knowledge_catalogue_create(config->source_capacity,
                                            &service->catalogue);
    if (status == UMI_STATUS_OK) {
        status = umi_knowledge_vector_index_create(
            config->vector_capacity, config->embedding_dimension,
            config->metric, &service->index);
    }
    if (status == UMI_STATUS_OK &&
        strcmp(config->default_provider_id, "umicom.local.hash") == 0) {
        status = umi_knowledge_hash_embedding_provider(
            "umicom.local.hash", config->embedding_dimension, &provider);
        if (status == UMI_STATUS_OK) {
            status = umi_knowledge_embedding_registry_add(
                &service->providers, &provider);
        }
    }
    if (status != UMI_STATUS_OK) {
        umi_knowledge_service_destroy(service);
        return status;
    }
    service->revision = 1U;
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_knowledge_service_destroy(UmiKnowledgeService *service)
{
    if (service == NULL) return;
    umi_knowledge_vector_index_destroy(service->index);
    umi_knowledge_catalogue_destroy(service->catalogue);
    free(service);
}

UmiStatus umi_knowledge_service_register_provider(
    UmiKnowledgeService *service,
    const UmiKnowledgeEmbeddingProvider *provider)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_knowledge_embedding_registry_add(&service->providers, provider);
}

UmiStatus umi_knowledge_service_add_collection(
    UmiKnowledgeService *service,
    const UmiKnowledgeCollection *collection)
{
    size_t position;
    if (service == NULL || collection == NULL ||
        collection->collection_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (position = 0U; position < service->collection_count; ++position) {
        if (strcmp(service->collections[position].collection_id,
                   collection->collection_id) == 0) {
            service->collections[position] = *collection;
            ++service->revision;
            return UMI_STATUS_OK;
        }
    }
    if (service->collection_count >= UMI_KNOWLEDGE_COLLECTION_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    service->collections[service->collection_count++] = *collection;
    ++service->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_knowledge_service_collection_at(
    const UmiKnowledgeService *service,
    size_t position,
    UmiKnowledgeCollection *out_collection)
{
    if (service == NULL || out_collection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= service->collection_count) return UMI_STATUS_NOT_FOUND;
    *out_collection = service->collections[position];
    return UMI_STATUS_OK;
}

static int collection_enabled(const UmiKnowledgeService *service,
                              const char *collection_id)
{
    size_t position;
    for (position = 0U; position < service->collection_count; ++position) {
        if (strcmp(service->collections[position].collection_id,
                   collection_id) == 0) {
            return service->collections[position].enabled;
        }
    }
    return 0;
}

UmiStatus umi_knowledge_service_ingest_text(
    UmiKnowledgeService *service,
    const UmiKnowledgeSource *source,
    const char *text,
    UmiKnowledgeIngestionReport *out_report)
{
    const UmiKnowledgeEmbeddingProvider *provider;
    UmiStatus status;
    if (service == NULL || source == NULL || text == NULL ||
        out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!collection_enabled(service, source->collection_id)) {
        return UMI_STATUS_NOT_FOUND;
    }
    provider = umi_knowledge_embedding_registry_find(
        &service->providers, service->config.default_provider_id);
    if (provider == NULL) return UMI_STATUS_UNAVAILABLE;
    status = umi_knowledge_ingest_text(
        service->catalogue, service->index, provider,
        &service->config.chunk_policy, source, text, out_report);
    if (status == UMI_STATUS_OK) {
        if (out_report->decision != UMI_KNOWLEDGE_REFRESH_UNCHANGED) {
            ++service->revision;
        }
        (void)snprintf(service->last_source_id,
                       sizeof(service->last_source_id), "%s",
                       source->source_id);
    }
    return status;
}

UmiStatus umi_knowledge_service_query(
    UmiKnowledgeService *service,
    const UmiKnowledgeQuery *query,
    UmiKnowledgeMatch *matches,
    size_t capacity,
    size_t *out_count)
{
    const UmiKnowledgeEmbeddingProvider *provider;
    UmiStatus status;
    if (service == NULL || query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    provider = umi_knowledge_embedding_registry_find(
        &service->providers, service->config.default_provider_id);
    if (provider == NULL) return UMI_STATUS_UNAVAILABLE;
    status = umi_knowledge_retrieve(
        service->catalogue, service->index, provider, query, matches,
        capacity, out_count);
    if (status == UMI_STATUS_OK) {
        (void)snprintf(service->last_query, sizeof(service->last_query),
                       "%s", query->text);
        service->last_result_count = *out_count;
    }
    return status;
}

UmiStatus umi_knowledge_service_snapshot(
    const UmiKnowledgeService *service,
    UmiKnowledgeServiceSnapshot *out_snapshot)
{
    if (service == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->revision = service->revision;
    out_snapshot->collection_count = service->collection_count;
    out_snapshot->source_count = umi_knowledge_catalogue_count(
        service->catalogue);
    out_snapshot->chunk_count = umi_knowledge_vector_index_count(service->index);
    out_snapshot->embedding_dimension = service->config.embedding_dimension;
    (void)snprintf(out_snapshot->default_provider_id,
                   sizeof(out_snapshot->default_provider_id), "%s",
                   service->config.default_provider_id);
    (void)snprintf(out_snapshot->last_source_id,
                   sizeof(out_snapshot->last_source_id), "%s",
                   service->last_source_id);
    (void)snprintf(out_snapshot->last_query,
                   sizeof(out_snapshot->last_query), "%s",
                   service->last_query);
    out_snapshot->last_result_count = service->last_result_count;
    return UMI_STATUS_OK;
}

UmiKnowledgeCatalogue *umi_knowledge_service_catalogue(
    UmiKnowledgeService *service)
{
    return service != NULL ? service->catalogue : NULL;
}

UmiKnowledgeVectorIndex *umi_knowledge_service_vector_index(
    UmiKnowledgeService *service)
{
    return service != NULL ? service->index : NULL;
}
