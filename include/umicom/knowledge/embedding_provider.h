/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/embedding_provider.h
 * PURPOSE: Define vendor-neutral local embedding providers and their registry.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Providers turn text into numeric vectors; retrieval remains vendor-neutral. */
#ifndef UMICOM_KNOWLEDGE_EMBEDDING_PROVIDER_H
#define UMICOM_KNOWLEDGE_EMBEDDING_PROVIDER_H

#include "umicom/knowledge/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_KNOWLEDGE_EMBEDDING_PROVIDER_ABI 1U

typedef UmiStatus (*UmiKnowledgeEmbedTextFn)(void *instance,
                                             const char *text,
                                             UmiKnowledgeEmbedding *out_value);

/**
 * Represent the knowledge embedding provider data shared with callers of this public
 * contract.
 */
typedef struct UmiKnowledgeEmbeddingProvider {
    uint32_t structure_size;
    uint32_t abi_version;
    const char *provider_id;
    const char *display_name;
    size_t dimension;
    int local;
    void *instance;
    UmiKnowledgeEmbedTextFn embed_text;
} UmiKnowledgeEmbeddingProvider;

/**
 * Represent the knowledge embedding registry data shared with callers of this public
 * contract.
 */
typedef struct UmiKnowledgeEmbeddingRegistry {
    UmiKnowledgeEmbeddingProvider providers[UMI_KNOWLEDGE_PROVIDER_MAX];
    size_t count;
} UmiKnowledgeEmbeddingRegistry;

/**
 * Initialise knowledge embedding registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_knowledge_embedding_registry_init(
    UmiKnowledgeEmbeddingRegistry *registry);
/**
 * Add knowledge embedding registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_knowledge_embedding_registry_add(
    UmiKnowledgeEmbeddingRegistry *registry,
    const UmiKnowledgeEmbeddingProvider *provider);
/**
 * Find knowledge embedding registry while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiKnowledgeEmbeddingProvider *umi_knowledge_embedding_registry_find(
    const UmiKnowledgeEmbeddingRegistry *registry,
    const char *provider_id);
/**
 * Provide the knowledge hash embedding provider operation used by this module and its
 * client applications.
 */
UmiStatus umi_knowledge_hash_embedding_provider(
    const char *provider_id,
    size_t dimension,
    UmiKnowledgeEmbeddingProvider *out_provider);

#ifdef __cplusplus
}
#endif
#endif
