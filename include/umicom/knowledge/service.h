/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/service.h
 * PURPOSE: Own reusable collections, sources, embeddings and retrieval state.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: This service coordinates retrieval evidence, not model inference. */
#ifndef UMICOM_KNOWLEDGE_SERVICE_H
#define UMICOM_KNOWLEDGE_SERVICE_H

#include "umicom/knowledge/collection.h"
#include "umicom/knowledge/ingestion.h"
#include "umicom/knowledge/retrieval.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiKnowledgeServiceConfig {
    size_t source_capacity;
    size_t vector_capacity;
    size_t embedding_dimension;
    UmiKnowledgeDistanceMetric metric;
    UmiKnowledgeChunkPolicy chunk_policy;
    char default_provider_id[UMI_KNOWLEDGE_ID_CAPACITY];
} UmiKnowledgeServiceConfig;

typedef struct UmiKnowledgeServiceSnapshot {
    uint64_t revision;
    size_t collection_count;
    size_t source_count;
    size_t chunk_count;
    size_t embedding_dimension;
    char default_provider_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char last_source_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char last_query[UMI_KNOWLEDGE_TEXT_CAPACITY];
    size_t last_result_count;
} UmiKnowledgeServiceSnapshot;

typedef struct UmiKnowledgeService UmiKnowledgeService;

UmiKnowledgeServiceConfig umi_knowledge_service_config_default(void);
UmiStatus umi_knowledge_service_create(
    const UmiKnowledgeServiceConfig *config,
    UmiKnowledgeService **out_service);
void umi_knowledge_service_destroy(UmiKnowledgeService *service);
UmiStatus umi_knowledge_service_register_provider(
    UmiKnowledgeService *service,
    const UmiKnowledgeEmbeddingProviderV1 *provider);
UmiStatus umi_knowledge_service_add_collection(
    UmiKnowledgeService *service,
    const UmiKnowledgeCollection *collection);
UmiStatus umi_knowledge_service_collection_at(
    const UmiKnowledgeService *service,
    size_t position,
    UmiKnowledgeCollection *out_collection);
UmiStatus umi_knowledge_service_ingest_text(
    UmiKnowledgeService *service,
    const UmiKnowledgeSource *source,
    const char *text,
    UmiKnowledgeIngestionReport *out_report);
UmiStatus umi_knowledge_service_query(
    UmiKnowledgeService *service,
    const UmiKnowledgeQuery *query,
    UmiKnowledgeMatch *matches,
    size_t capacity,
    size_t *out_count);
UmiStatus umi_knowledge_service_snapshot(
    const UmiKnowledgeService *service,
    UmiKnowledgeServiceSnapshot *out_snapshot);
UmiKnowledgeCatalogue *umi_knowledge_service_catalogue(
    UmiKnowledgeService *service);
UmiKnowledgeVectorIndex *umi_knowledge_service_vector_index(
    UmiKnowledgeService *service);

#ifdef __cplusplus
}
#endif
#endif
