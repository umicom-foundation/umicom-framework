/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_knowledge_vector_index.c
 * PURPOSE: Verify vector ranking, collection filters and source replacement.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/knowledge/embedding_provider.h"
#include "umicom/knowledge/vector_index.h"

int main(void)
{
    UmiKnowledgeVectorIndex *index = NULL;
    UmiKnowledgeEmbeddingProvider provider;
    UmiKnowledgeChunk chunk = {0};
    UmiKnowledgeEmbedding embedding;
    UmiKnowledgeMatch matches[4];
    UmiKnowledgeFilter filter = {0};
    size_t count = 0U;
    size_t removed = 0U;
    assert(umi_knowledge_vector_index_create(
        8U, 64U, UMI_KNOWLEDGE_DISTANCE_COSINE, &index) == UMI_STATUS_OK);
    assert(umi_knowledge_hash_embedding_provider(
        "test.hash", 64U, &provider) == UMI_STATUS_OK);
    (void)strcpy(chunk.chunk_id, "a#0");
    (void)strcpy(chunk.source_id, "a");
    (void)strcpy(chunk.collection_id, "code");
    (void)strcpy(chunk.text, "native retrieval engine");
    chunk.classification = UMI_AI_DATA_INTERNAL;
    assert(provider.embed_text(provider.instance, chunk.text, &embedding)
           == UMI_STATUS_OK);
    assert(umi_knowledge_vector_index_upsert(index, &chunk, &embedding)
           == UMI_STATUS_OK);
    (void)strcpy(filter.collection_id, "code");
    filter.maximum_classification = UMI_AI_DATA_INTERNAL;
    assert(umi_knowledge_vector_index_search(
        index, &embedding, &filter, 4U, matches, 4U, &count) == UMI_STATUS_OK);
    assert(count == 1U && matches[0].score > 0.99);
    assert(umi_knowledge_vector_index_remove_source(
        index, "a", &removed) == UMI_STATUS_OK);
    assert(removed == 1U && umi_knowledge_vector_index_count(index) == 0U);
    umi_knowledge_vector_index_destroy(index);
    return 0;
}
