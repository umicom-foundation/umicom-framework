/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/embedding_provider.c
 * PURPOSE: Implement the provider registry and deterministic offline embedding.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/embedding_provider.h"

#include <ctype.h>
#include <math.h>
#include <string.h>

void umi_knowledge_embedding_registry_init(
    UmiKnowledgeEmbeddingRegistry *registry)
{
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

UmiStatus umi_knowledge_embedding_registry_add(
    UmiKnowledgeEmbeddingRegistry *registry,
    const UmiKnowledgeEmbeddingProviderV1 *provider)
{
    size_t index;
    if (registry == NULL || provider == NULL || provider->provider_id == NULL ||
        provider->provider_id[0] == '\0' || provider->embed_text == NULL ||
        provider->structure_size < sizeof(*provider) ||
        provider->abi_version != UMI_KNOWLEDGE_EMBEDDING_PROVIDER_ABI ||
        provider->dimension == 0U ||
        provider->dimension > UMI_KNOWLEDGE_EMBEDDING_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index].provider_id,
                   provider->provider_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_KNOWLEDGE_PROVIDER_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->providers[registry->count++] = *provider;
    return UMI_STATUS_OK;
}

const UmiKnowledgeEmbeddingProviderV1 *umi_knowledge_embedding_registry_find(
    const UmiKnowledgeEmbeddingRegistry *registry,
    const char *provider_id)
{
    size_t index;
    if (registry == NULL || provider_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->providers[index].provider_id, provider_id) == 0) {
            return &registry->providers[index];
        }
    }
    return NULL;
}

static UmiStatus hash_embed(void *instance, const char *text,
                            UmiKnowledgeEmbedding *out_value)
{
    const unsigned char *cursor = (const unsigned char *)text;
    double sum = 0.0;
    size_t previous = 0U;
    (void)instance;
    if (text == NULL || text[0] == '\0' || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_value, 0, sizeof(*out_value));
    out_value->dimension = 64U;
    while (*cursor != 0U) {
        size_t current = (size_t)(unsigned char)tolower((int)*cursor);
        size_t bucket = (current * 33U + previous * 17U) % 64U;
        float sign = ((current + previous) & 1U) != 0U ? 1.0F : -1.0F;
        out_value->values[bucket] += sign;
        previous = current;
        ++cursor;
    }
    for (size_t index = 0U; index < out_value->dimension; ++index) {
        sum += (double)out_value->values[index] *
               (double)out_value->values[index];
    }
    if (sum > 0.0) {
        double magnitude = sqrt(sum);
        for (size_t index = 0U; index < out_value->dimension; ++index) {
            out_value->values[index] =
                (float)((double)out_value->values[index] / magnitude);
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_knowledge_hash_embedding_provider(
    const char *provider_id,
    size_t dimension,
    UmiKnowledgeEmbeddingProviderV1 *out_provider)
{
    if (provider_id == NULL || provider_id[0] == '\0' ||
        dimension != 64U || out_provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_provider, 0, sizeof(*out_provider));
    out_provider->structure_size = (uint32_t)sizeof(*out_provider);
    out_provider->abi_version = UMI_KNOWLEDGE_EMBEDDING_PROVIDER_ABI;
    out_provider->provider_id = provider_id;
    out_provider->display_name = "Deterministic Local Hash Embedding";
    out_provider->dimension = dimension;
    out_provider->local = 1;
    out_provider->embed_text = hash_embed;
    return UMI_STATUS_OK;
}
