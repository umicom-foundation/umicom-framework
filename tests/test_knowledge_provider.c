/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_knowledge_provider.c
 * PURPOSE: Verify the embedding ABI, registry and deterministic local provider.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/knowledge/embedding_provider.h"

int main(void)
{
    UmiKnowledgeEmbeddingRegistry registry;
    UmiKnowledgeEmbeddingProvider provider;
    UmiKnowledgeEmbedding first;
    UmiKnowledgeEmbedding second;
    umi_knowledge_embedding_registry_init(&registry);
    assert(umi_knowledge_hash_embedding_provider(
        "test.hash", 64U, &provider) == UMI_STATUS_OK);
    assert(umi_knowledge_embedding_registry_add(
        &registry, &provider) == UMI_STATUS_OK);
    assert(umi_knowledge_embedding_registry_find(
        &registry, "test.hash") != NULL);
    assert(provider.embed_text(provider.instance, "native rag", &first)
           == UMI_STATUS_OK);
    assert(provider.embed_text(provider.instance, "native rag", &second)
           == UMI_STATUS_OK);
    assert(first.dimension == 64U);
    for (size_t index = 0U; index < first.dimension; ++index) {
        assert(first.values[index] == second.values[index]);
    }
    return 0;
}
