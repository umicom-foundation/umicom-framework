/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/embedding_provider.h
 * PURPOSE: Define vendor-neutral local embedding providers and their registry.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Providers turn text into numeric vectors; retrieval remains vendor-neutral. */
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

typedef struct UmiKnowledgeEmbeddingRegistry {
    UmiKnowledgeEmbeddingProvider providers[UMI_KNOWLEDGE_PROVIDER_MAX];
    size_t count;
} UmiKnowledgeEmbeddingRegistry;

void umi_knowledge_embedding_registry_init(
    UmiKnowledgeEmbeddingRegistry *registry);
UmiStatus umi_knowledge_embedding_registry_add(
    UmiKnowledgeEmbeddingRegistry *registry,
    const UmiKnowledgeEmbeddingProvider *provider);
const UmiKnowledgeEmbeddingProvider *umi_knowledge_embedding_registry_find(
    const UmiKnowledgeEmbeddingRegistry *registry,
    const char *provider_id);
UmiStatus umi_knowledge_hash_embedding_provider(
    const char *provider_id,
    size_t dimension,
    UmiKnowledgeEmbeddingProvider *out_provider);

#ifdef __cplusplus
}
#endif
#endif
