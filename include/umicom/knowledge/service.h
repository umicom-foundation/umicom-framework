/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/service.h
 * PURPOSE: Own reusable collections, sources, embeddings and retrieval state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* This service coordinates retrieval evidence, not model inference. */
#ifndef UMICOM_KNOWLEDGE_SERVICE_H
#define UMICOM_KNOWLEDGE_SERVICE_H

#include "umicom/knowledge/collection.h"
#include "umicom/knowledge/ingestion.h"
#include "umicom/knowledge/retrieval.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the knowledge service config data shared with callers of this public contract.
 */
typedef struct UmiKnowledgeServiceConfig {
    size_t source_capacity;
    size_t vector_capacity;
    size_t embedding_dimension;
    UmiKnowledgeDistanceMetric metric;
    UmiKnowledgeChunkPolicy chunk_policy;
    char default_provider_id[UMI_KNOWLEDGE_ID_CAPACITY];
} UmiKnowledgeServiceConfig;

/**
 * Represent the knowledge service snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the knowledge service data shared with callers of this public contract.
 */
typedef struct UmiKnowledgeService UmiKnowledgeService;

/**
 * Provide the knowledge service config default operation used by this module and its
 * client applications.
 */
UmiKnowledgeServiceConfig umi_knowledge_service_config_default(void);
/**
 * Initialise knowledge service from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_knowledge_service_create(
    const UmiKnowledgeServiceConfig *config,
    UmiKnowledgeService **out_service);
/**
 * Release or reset state held by knowledge service so the same storage can be reused
 * safely.
 */
void umi_knowledge_service_destroy(UmiKnowledgeService *service);
/**
 * Provide the knowledge service register provider operation used by this module and its
 * client applications.
 */
UmiStatus umi_knowledge_service_register_provider(
    UmiKnowledgeService *service,
    const UmiKnowledgeEmbeddingProvider *provider);
/**
 * Provide the knowledge service add collection operation used by this module and its
 * client applications.
 */
UmiStatus umi_knowledge_service_add_collection(
    UmiKnowledgeService *service,
    const UmiKnowledgeCollection *collection);
/**
 * Find knowledge service collection while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_knowledge_service_collection_at(
    const UmiKnowledgeService *service,
    size_t position,
    UmiKnowledgeCollection *out_collection);
/**
 * Provide the knowledge service ingest text operation used by this module and its client
 * applications.
 */
UmiStatus umi_knowledge_service_ingest_text(
    UmiKnowledgeService *service,
    const UmiKnowledgeSource *source,
    const char *text,
    UmiKnowledgeIngestionReport *out_report);
/**
 * Provide the knowledge service query operation used by this module and its client
 * applications.
 */
UmiStatus umi_knowledge_service_query(
    UmiKnowledgeService *service,
    const UmiKnowledgeQuery *query,
    UmiKnowledgeMatch *matches,
    size_t capacity,
    size_t *out_count);
/**
 * Provide the knowledge service snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_knowledge_service_snapshot(
    const UmiKnowledgeService *service,
    UmiKnowledgeServiceSnapshot *out_snapshot);
/**
 * Provide the knowledge service catalogue operation used by this module and its client
 * applications.
 */
UmiKnowledgeCatalogue *umi_knowledge_service_catalogue(
    UmiKnowledgeService *service);
/**
 * Provide the knowledge service vector index operation used by this module and its client
 * applications.
 */
UmiKnowledgeVectorIndex *umi_knowledge_service_vector_index(
    UmiKnowledgeService *service);

#ifdef __cplusplus
}
#endif
#endif
