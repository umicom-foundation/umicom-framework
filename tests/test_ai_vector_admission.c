/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_vector_admission.c
 *
 * PURPOSE:
 *   Exercise bounded AI record admission, alias-safe construction and stable vector ranking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "umicom/ai/vector_store.h"

/* These owners are deliberately outside the thread stack. Tests run serially;
 * every scenario initializes its fixture and compares the entire refused owner. */
static UmiAiVectorStore store;
static UmiAiVectorStore before;
static UmiAiChunk chunk;
static UmiAiEmbedding embedding;
static size_t indices[UMI_AI_VECTOR_STORE_CAPACITY + 2U];
static double scores[UMI_AI_VECTOR_STORE_CAPACITY + 2U];

static void Prepare(void)
{
    const float components[] = {1.0F, 0.0F};
    umi_ai_vector_store_init(&store);
    assert(umi_ai_chunk_set(&chunk, "guide", 0U, 0U, "sample") == UMI_STATUS_OK);
    assert(umi_ai_embedding_set(&embedding, components, 2U) == UMI_STATUS_OK);
}

static void SeedOutputs(void)
{
    for (size_t i = 0U; i < UMI_AI_VECTOR_STORE_CAPACITY + 2U; ++i) {
        indices[i] = SIZE_MAX;
        scores[i] = 1234.0;
    }
}

static void OutputsUnchanged(void)
{
    for (size_t i = 0U; i < UMI_AI_VECTOR_STORE_CAPACITY + 2U; ++i) {
        assert(indices[i] == SIZE_MAX);
        assert(scores[i] == 1234.0);
    }
}

static void EmbeddingCases(void)
{
    UmiAiEmbedding value;
    UmiAiEmbedding saved;
    UmiAiEmbedding other;
    float components[UMI_AI_EMBEDDING_CAPACITY];
    const size_t bad_dimensions[] = {0U, UMI_AI_EMBEDDING_CAPACITY + 1U, SIZE_MAX};
    const float bad_components[] = {NAN, INFINITY, -INFINITY};
    for (size_t i = 0U; i < UMI_AI_EMBEDDING_CAPACITY; ++i) components[i] = (float)i;
    assert(umi_ai_embedding_set(&value, components, UMI_AI_EMBEDDING_CAPACITY) == UMI_STATUS_OK);
    assert(umi_ai_embedding_valid(&value));
    assert(umi_ai_embedding_set(&value, value.values + 1U, 2U) == UMI_STATUS_OK);
    assert(value.values[0] == 1.0F && value.values[1] == 2.0F && value.dimension == 2U);
    for (size_t i = 2U; i < UMI_AI_EMBEDDING_CAPACITY; ++i) assert(value.values[i] == 0.0F);
    assert(umi_ai_embedding_set(&value, value.values, 2U) == UMI_STATUS_OK);
    assert(value.values[0] == 1.0F && value.values[1] == 2.0F);
    (void)memcpy(&saved, &value, sizeof(saved));
    assert(!umi_ai_embedding_valid(NULL));
    assert(umi_ai_embedding_set(NULL, components, 2U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_ai_embedding_set(&value, NULL, 2U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&saved, &value, sizeof(value)) == 0);
    for (size_t i = 0U; i < sizeof(bad_dimensions) / sizeof(bad_dimensions[0]); ++i) {
        assert(umi_ai_embedding_set(&value, components, bad_dimensions[i]) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&saved, &value, sizeof(value)) == 0);
        other = value;
        other.dimension = bad_dimensions[i];
        assert(!umi_ai_embedding_valid(&other));
        assert(umi_ai_embedding_cosine(&other, &other) == 0.0);
    }
    for (size_t i = 0U; i < sizeof(bad_components) / sizeof(bad_components[0]); ++i) {
        components[1] = bad_components[i];
        assert(umi_ai_embedding_set(&value, components, 2U) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&saved, &value, sizeof(value)) == 0);
        other = value;
        other.values[1] = bad_components[i];
        assert(!umi_ai_embedding_valid(&other));
        assert(umi_ai_embedding_cosine(&value, &other) == 0.0);
    }
    other = value;
    other.values[UMI_AI_EMBEDDING_CAPACITY - 1U] = NAN;
    assert(umi_ai_embedding_valid(&other)); /* inactive storage is not vector data */
    components[0] = FLT_MAX;
    components[1] = FLT_MAX;
    assert(umi_ai_embedding_set(&other, components, 2U) == UMI_STATUS_OK);
    assert(isfinite(umi_ai_embedding_cosine(&other, &other)));
    assert(fabs(umi_ai_embedding_cosine(&other, &other) - 1.0) < 1e-12);
    components[0] = FLT_MIN;
    components[1] = 0.0F;
    assert(umi_ai_embedding_set(&other, components, 2U) == UMI_STATUS_OK);
    assert(fabs(umi_ai_embedding_cosine(&other, &other) - 1.0) < 1e-12);
    components[0] = 0.0F;
    assert(umi_ai_embedding_set(&other, components, 2U) == UMI_STATUS_OK);
    assert(umi_ai_embedding_valid(&other));
    assert(umi_ai_embedding_cosine(&other, &other) == 0.0);
}

static void ChunkCases(void)
{
    UmiAiChunk saved;
    char long_document[UMI_AI_ID_CAPACITY];
    char long_text[UMI_AI_TEXT_CAPACITY];
    Prepare();
    assert(umi_ai_chunk_set(&chunk, chunk.document_id, 7U, 10U, chunk.text + 1U) == UMI_STATUS_OK);
    assert(strcmp(chunk.document_id, "guide") == 0);
    assert(strcmp(chunk.chunk_id, "guide:7") == 0);
    assert(strcmp(chunk.text, "ample") == 0);
    assert(chunk.start_offset == 10U && chunk.end_offset == 15U);
    (void)memcpy(&saved, &chunk, sizeof(saved));
    (void)memset(long_document, 'd', sizeof(long_document));
    (void)memset(long_text, 'x', sizeof(long_text));
    assert(umi_ai_chunk_set(&chunk, long_document, 0U, 0U, "a") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&saved, &chunk, sizeof(chunk)) == 0);
    assert(umi_ai_chunk_set(&chunk, "doc", 0U, 0U, long_text) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&saved, &chunk, sizeof(chunk)) == 0);
    /* The document itself fits, but appending the generated index does not. */
    long_document[sizeof(long_document) - 1U] = '\0';
    assert(umi_ai_chunk_set(&chunk, long_document, 0U, 0U, "a") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&saved, &chunk, sizeof(chunk)) == 0);
    assert(umi_ai_chunk_set(&chunk, "doc", 0U, SIZE_MAX - 2U, "abc") == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&saved, &chunk, sizeof(chunk)) == 0);
    assert(umi_ai_chunk_set(&chunk, NULL, 0U, 0U, "a") == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_ai_chunk_set(&chunk, "doc", 0U, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&saved, &chunk, sizeof(chunk)) == 0);
    assert(umi_ai_chunk_set(NULL, "doc", 0U, 0U, "a") == UMI_STATUS_INVALID_ARGUMENT);
    long_document[sizeof(long_document) - 3U] = '\0';
    assert(umi_ai_chunk_set(&chunk, long_document, 0U, 0U, "a") == UMI_STATUS_OK);
    assert(strlen(chunk.chunk_id) == sizeof(chunk.chunk_id) - 1U);
    long_text[sizeof(long_text) - 1U] = '\0';
    assert(umi_ai_chunk_set(&chunk, "doc", SIZE_MAX, 0U, long_text) == UMI_STATUS_OK);
    assert(chunk.end_offset == sizeof(long_text) - 1U);
    assert(umi_ai_chunk_set(&chunk, "", 7U, SIZE_MAX, "") == UMI_STATUS_OK);
    assert(strcmp(chunk.chunk_id, ":7") == 0 && chunk.end_offset == SIZE_MAX);
    assert(umi_ai_chunk_storage_valid(&chunk));
    assert(!umi_ai_chunk_storage_valid(NULL));
}

static void DamageEntry(UmiAiVectorEntry *entry, size_t fault)
{
    switch (fault) {
    case 0U: (void)memset(entry->chunk.chunk_id, 'x', sizeof(entry->chunk.chunk_id)); break;
    case 1U: (void)memset(entry->chunk.document_id, 'x', sizeof(entry->chunk.document_id)); break;
    case 2U: (void)memset(entry->chunk.text, 'x', sizeof(entry->chunk.text)); break;
    case 3U: entry->embedding.dimension = 0U; break;
    case 4U: entry->embedding.dimension = UMI_AI_EMBEDDING_CAPACITY + 1U; break;
    case 5U: entry->embedding.dimension = SIZE_MAX; break;
    case 6U: entry->embedding.values[1] = NAN; break;
    case 7U: entry->embedding.values[1] = INFINITY; break;
    default: entry->embedding.values[1] = -INFINITY; break;
    }
}

static void StoreRefusals(void)
{
    const size_t bad_counts[] = {UMI_AI_VECTOR_STORE_CAPACITY + 1U, SIZE_MAX};
    Prepare();
    assert(umi_ai_vector_store_valid(&store));
    assert(!umi_ai_vector_store_valid(NULL));
    for (size_t i = 0U; i < sizeof(bad_counts) / sizeof(bad_counts[0]); ++i) {
        store.count = bad_counts[i];
        (void)memcpy(&before, &store, sizeof(before));
        SeedOutputs();
        assert(!umi_ai_vector_store_valid(&store));
        assert(umi_ai_vector_store_add(&store, &chunk, &embedding) == UMI_STATUS_INVALID_STATE);
        assert(umi_ai_vector_store_search(&store, &embedding, indices, scores, 2U) == 0U);
        OutputsUnchanged();
        assert(memcmp(&before, &store, sizeof(store)) == 0);
    }
    for (size_t fault = 0U; fault < 9U; ++fault) {
        UmiAiVectorEntry incoming;
        Prepare();
        assert(umi_ai_vector_store_add(&store, &chunk, &embedding) == UMI_STATUS_OK);
        assert(umi_ai_vector_store_add(&store, &chunk, &embedding) == UMI_STATUS_OK);
        incoming = store.entries[0];
        DamageEntry(&incoming, fault);
        (void)memcpy(&before, &store, sizeof(before));
        assert(umi_ai_vector_store_add(&store, &incoming.chunk, &incoming.embedding) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&before, &store, sizeof(store)) == 0);
        /* Corrupt the last row: search must not publish a valid earlier result. */
        store.entries[1] = incoming;
        (void)memcpy(&before, &store, sizeof(before));
        SeedOutputs();
        assert(!umi_ai_vector_store_valid(&store));
        assert(umi_ai_vector_store_add(&store, &chunk, &embedding) == UMI_STATUS_INVALID_STATE);
        assert(umi_ai_vector_store_search(&store, &embedding, indices, scores, 1U) == 0U);
        OutputsUnchanged();
        assert(memcmp(&before, &store, sizeof(store)) == 0);
    }
    Prepare();
    for (size_t i = 0U; i < UMI_AI_VECTOR_STORE_CAPACITY; ++i)
        assert(umi_ai_vector_store_add(&store, &chunk, &embedding) == UMI_STATUS_OK);
    (void)memcpy(&before, &store, sizeof(before));
    assert(umi_ai_vector_store_add(&store, &chunk, &embedding) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(memcmp(&before, &store, sizeof(store)) == 0);
    SeedOutputs();
    assert(umi_ai_vector_store_search(&store, &embedding, indices, scores, UMI_AI_VECTOR_STORE_CAPACITY + 2U) == UMI_AI_VECTOR_STORE_CAPACITY);
    for (size_t i = 0U; i < UMI_AI_VECTOR_STORE_CAPACITY; ++i) assert(indices[i] == i);
    assert(indices[UMI_AI_VECTOR_STORE_CAPACITY] == SIZE_MAX);
    assert(scores[UMI_AI_VECTOR_STORE_CAPACITY] == 1234.0);
}

static void RankingAndAliases(void)
{
    const float values[6][2] = {{1.0F,0.0F},{0.0F,1.0F},{-1.0F,0.0F},
                                {2.0F,0.0F},{0.0F,0.0F},{1.0F,0.0F}};
    const size_t expected[] = {0U,3U,1U,4U,5U,2U};
    UmiAiEmbedding query;
    Prepare();
    query = embedding;
    for (size_t i = 0U; i < 6U; ++i) {
        assert(umi_ai_embedding_set(&embedding, values[i], i == 5U ? 1U : 2U) == UMI_STATUS_OK);
        assert(umi_ai_vector_store_add(&store, &chunk, &embedding) == UMI_STATUS_OK);
    }
    SeedOutputs();
    assert(umi_ai_vector_store_search(&store, &query, indices, scores, 8U) == 6U);
    for (size_t i = 0U; i < 6U; ++i) assert(indices[i] == expected[i]);
    assert(scores[0] == 1.0 && scores[1] == 1.0 && scores[2] == 0.0 && scores[5] == -1.0);
    assert(indices[6] == SIZE_MAX && scores[6] == 1234.0);
    SeedOutputs();
    assert(umi_ai_vector_store_search(&store, &store.entries[0].embedding, indices, scores, 3U) == 3U);
    for (size_t i = 0U; i < 3U; ++i) assert(indices[i] == expected[i]);
    assert(indices[3] == SIZE_MAX && scores[3] == 1234.0);
    assert(umi_ai_vector_store_add(&store, &store.entries[0].chunk, &store.entries[1].embedding) == UMI_STATUS_OK);
    assert(strcmp(store.entries[6].chunk.chunk_id, store.entries[0].chunk.chunk_id) == 0);
    assert(store.entries[6].embedding.values[1] == 1.0F);
    /* Inactive malformed bytes do not invalidate an initialized store. */
    (void)memset(&store.entries[7], 0xff, sizeof(store.entries[7]));
    assert(umi_ai_vector_store_valid(&store));
    store.entries[7] = store.entries[0];
    assert(umi_ai_vector_store_add(&store, &store.entries[7].chunk, &store.entries[7].embedding) == UMI_STATUS_OK);
    assert(store.count == 8U);
    SeedOutputs();
    assert(umi_ai_vector_store_search(NULL, NULL, NULL, NULL, 0U) == 0U);
    assert(umi_ai_vector_store_search(&store, &query, indices, scores, 0U) == 0U);
    assert(umi_ai_vector_store_search(NULL, &query, indices, scores, 1U) == 0U);
    assert(umi_ai_vector_store_search(&store, NULL, indices, scores, 1U) == 0U);
    assert(umi_ai_vector_store_search(&store, &query, NULL, scores, 1U) == 0U);
    assert(umi_ai_vector_store_search(&store, &query, indices, NULL, 1U) == 0U);
    for (size_t fault = 3U; fault < 9U; ++fault) {
        UmiAiVectorEntry malformed = store.entries[0];
        DamageEntry(&malformed, fault);
        assert(umi_ai_vector_store_search(&store, &malformed.embedding, indices, scores, 1U) == 0U);
    }
    OutputsUnchanged();
    (void)memcpy(&before, &store, sizeof(before));
    assert(umi_ai_vector_store_add(NULL, &chunk, &query) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_ai_vector_store_add(&store, NULL, &query) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_ai_vector_store_add(&store, &chunk, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(&before, &store, sizeof(store)) == 0);
    /* Existing hybrid clients supply empty document metadata and zero offsets. */
    (void)memset(&chunk, 0, sizeof(chunk));
    (void)strcpy(chunk.chunk_id, "manual");
    (void)strcpy(chunk.text, "target");
    assert(umi_ai_vector_store_add(&store, &chunk, &query) == UMI_STATUS_OK);
}

int main(void)
{
    EmbeddingCases();
    ChunkCases();
    StoreRefusals();
    RankingAndAliases();
    return 0;
}
