/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_retrieval_publication.c
 *
 * PURPOSE:
 *   Verify lexical preflight, finite hybrid blends and atomic reranker publication.
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
#include "umicom/ai/hybrid_retrieval.h"

static UmiAiChunk chunks[3];
static UmiAiRetrievalResult results[4];
static UmiAiRetrievalResult saved[4];
static UmiAiVectorStore store;
static UmiAiEmbedding query_embedding;

typedef struct ScoreState {
    size_t calls;
    size_t fail_at;
    double invalid;
    bool negate;
    UmiAiReranker *descriptor;
} ScoreState;

static double Score(const char *query, const UmiAiChunk *chunk,
                    double base_score, void *data)
{
    ScoreState *state = data;
    size_t call = state->calls++;
    assert(strcmp(query, "target") == 0);
    assert(umi_ai_chunk_storage_valid(chunk));
    /* Replacing the descriptor does not replace the captured call contract. */
    if (state->descriptor != NULL) state->descriptor->score = NULL;
    if (call == state->fail_at) return state->invalid;
    return state->negate ? -base_score : base_score + 0.5;
}

static void Prepare(void)
{
    const float values[] = {1.0F, 0.0F};
    assert(umi_ai_chunk_set(&chunks[0], "doc", 0U, 0U, "target") == UMI_STATUS_OK);
    assert(umi_ai_chunk_set(&chunks[1], "doc", 1U, 6U, "TARGET target") == UMI_STATUS_OK);
    assert(umi_ai_chunk_set(&chunks[2], "doc", 2U, 19U, "other") == UMI_STATUS_OK);
    umi_ai_vector_store_init(&store);
    assert(umi_ai_embedding_set(&query_embedding, values, 2U) == UMI_STATUS_OK);
    assert(umi_ai_vector_store_add(&store, &chunks[0], &query_embedding) == UMI_STATUS_OK);
}

static void SeedOutput(void)
{
    (void)memset(results, 0xa5, sizeof(results));
    (void)memcpy(saved, results, sizeof(results));
}

static void SeedResults(void)
{
    for (size_t index = 0U; index < 3U; ++index) {
        results[index].chunk = chunks[index];
        results[index].score = index == 1U ? 1.0 : 2.0;
    }
    (void)memcpy(saved, results, sizeof(results));
}

static void DamageChunk(UmiAiChunk *chunk, size_t field)
{
    if (field == 0U) (void)memset(chunk->chunk_id, 'x', sizeof(chunk->chunk_id));
    else if (field == 1U) (void)memset(chunk->document_id, 'x', sizeof(chunk->document_id));
    else (void)memset(chunk->text, 'x', sizeof(chunk->text));
}

static void LexicalCases(void)
{
    Prepare();
    assert(umi_ai_retrieval_chunks_valid(NULL, 0U));
    assert(!umi_ai_retrieval_chunks_valid(NULL, 1U));
    assert(!umi_ai_retrieval_chunks_valid(chunks, SIZE_MAX));
    assert(umi_ai_retrieval_lexical_score("aa", "aaa") == 2.5);
    assert(umi_ai_retrieval_lexical_score("TARGET", "target") > 1.0);
    SeedOutput();
    assert(umi_ai_retrieval_rank("target", chunks, 3U, results, 4U) == 2U);
    assert(strcmp(results[0].chunk.chunk_id, chunks[1].chunk_id) == 0);
    assert(strcmp(results[1].chunk.chunk_id, chunks[0].chunk_id) == 0);
    assert(memcmp(&results[2], &saved[2], 2U * sizeof(results[0])) == 0);
    for (size_t field = 0U; field < 3U; ++field) {
        Prepare();
        DamageChunk(&chunks[2], field);
        SeedOutput();
        assert(!umi_ai_retrieval_chunks_valid(chunks, 3U));
        assert(umi_ai_retrieval_rank("target", chunks, 3U, results, 1U) == 0U);
        assert(memcmp(results, saved, sizeof(results)) == 0);
    }
    Prepare();
    chunks[1] = chunks[0];
    (void)strcpy(chunks[1].chunk_id, "second");
    SeedOutput();
    assert(umi_ai_retrieval_rank("target", chunks, 2U, results, 1U) == 1U);
    assert(strcmp(results[0].chunk.chunk_id, chunks[0].chunk_id) == 0);
    SeedOutput();
    assert(umi_ai_retrieval_rank("target", chunks, SIZE_MAX, results, 1U) == 0U);
    assert(umi_ai_retrieval_rank("target", chunks, 3U, results, 0U) == 0U);
    assert(umi_ai_retrieval_rank("", chunks, 3U, results, 4U) == 0U);
    assert(umi_ai_retrieval_rank("missing", chunks, 3U, results, 4U) == 0U);
    assert(memcmp(results, saved, sizeof(results)) == 0);
}

static void RerankerCases(void)
{
    const double invalid[] = {NAN, INFINITY, -INFINITY};
    Prepare();
    SeedOutput();
    SeedResults();
    assert(umi_ai_retrieval_results_valid(NULL, 0U));
    assert(!umi_ai_retrieval_results_valid(NULL, 1U));
    assert(!umi_ai_retrieval_results_valid(results, SIZE_MAX));
    assert(umi_ai_reranker_apply(NULL, "target", NULL, 0U) == UMI_STATUS_OK);
    for (size_t index = 0U; index < 3U; ++index) {
        ScoreState state = {0U, 1U, invalid[index], false, NULL};
        UmiAiReranker reranker = {Score, &state};
        SeedResults();
        assert(umi_ai_reranker_apply(&reranker, "target", results, 3U) == UMI_STATUS_INVALID_STATE);
        assert(state.calls == 2U);
        assert(memcmp(results, saved, sizeof(results)) == 0);
        results[2].score = invalid[index];
        (void)memcpy(saved, results, sizeof(results));
        state.calls = 0U;
        assert(!umi_ai_retrieval_results_valid(results, 3U));
        assert(umi_ai_reranker_apply(&reranker, "target", results, 3U) == UMI_STATUS_INVALID_ARGUMENT);
        assert(state.calls == 0U);
        assert(umi_ai_reranker_apply(NULL, "target", results, 3U) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(results, saved, sizeof(results)) == 0);
    }
    for (size_t field = 0U; field < 3U; ++field) {
        ScoreState state = {0U, SIZE_MAX, 0.0, false, NULL};
        UmiAiReranker reranker = {Score, &state};
        SeedResults();
        DamageChunk(&results[2].chunk, field);
        (void)memcpy(saved, results, sizeof(results));
        assert(umi_ai_reranker_apply(&reranker, "target", results, 3U) == UMI_STATUS_INVALID_ARGUMENT);
        assert(state.calls == 0U && memcmp(results, saved, sizeof(results)) == 0);
    }
    SeedResults();
    assert(umi_ai_reranker_apply(NULL, "target", results, 3U) == UMI_STATUS_OK);
    assert(strcmp(results[0].chunk.chunk_id, chunks[0].chunk_id) == 0);
    assert(strcmp(results[1].chunk.chunk_id, chunks[2].chunk_id) == 0);
    assert(strcmp(results[2].chunk.chunk_id, chunks[1].chunk_id) == 0);
    assert(memcmp(&results[3], &saved[3], sizeof(results[3])) == 0);
    {
        ScoreState state = {0U, SIZE_MAX, 0.0, true, NULL};
        UmiAiReranker reranker = {Score, &state};
        state.descriptor = &reranker;
        SeedResults();
        assert(umi_ai_reranker_apply(&reranker, "target", results, 3U) == UMI_STATUS_OK);
        assert(state.calls == 3U && reranker.score == NULL);
        assert(results[0].score == -1.0 && results[1].score == -2.0);
        assert(strcmp(results[0].chunk.chunk_id, chunks[1].chunk_id) == 0);
        assert(strcmp(results[1].chunk.chunk_id, chunks[0].chunk_id) == 0);
        assert(strcmp(results[2].chunk.chunk_id, chunks[2].chunk_id) == 0);
    }
    SeedResults();
    assert(umi_ai_reranker_apply(NULL, NULL, results, 3U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_ai_reranker_apply(NULL, "target", results, SIZE_MAX) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_ai_reranker_apply(NULL, "target", NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(memcmp(results, saved, sizeof(results)) == 0);
}

static void HybridCases(void)
{
    const double invalid[] = {NAN, INFINITY, -INFINITY};
    UmiAiHybridRetrievalSettings settings;
    ScoreState state = {0U, SIZE_MAX, 0.0, false, NULL};
    UmiAiReranker reranker = {Score, &state};
    Prepare();
    for (size_t field = 0U; field < 2U; ++field) {
        for (size_t index = 0U; index < 3U; ++index) {
            settings = umi_ai_hybrid_retrieval_settings_default();
            if (field == 0U) settings.lexical_weight = invalid[index];
            else settings.vector_weight = invalid[index];
            SeedOutput();
            assert(umi_ai_hybrid_retrieval_rank("target", chunks, 3U, &store,
                &query_embedding, &settings, &reranker, results, 4U) == 0U);
            assert(state.calls == 0U && memcmp(results, saved, sizeof(results)) == 0);
        }
    }
    settings = umi_ai_hybrid_retrieval_settings_default();
    settings.lexical_weight = DBL_MAX;
    settings.vector_weight = DBL_MAX;
    SeedOutput();
    /* One matching chunk receives two finite terms whose sum overflows. */
    assert(umi_ai_hybrid_retrieval_rank("target", chunks, 1U, &store,
        &query_embedding, &settings, &reranker, results, 4U) == 0U);
    assert(state.calls == 0U && memcmp(results, saved, sizeof(results)) == 0);
    for (size_t field = 0U; field < 3U; ++field) {
        Prepare();
        DamageChunk(&chunks[2], field);
        SeedOutput();
        assert(umi_ai_hybrid_retrieval_rank("target", chunks, 3U, &store,
            &query_embedding, NULL, &reranker, results, 4U) == 0U);
        assert(state.calls == 0U && memcmp(results, saved, sizeof(results)) == 0);
    }
    Prepare();
    SeedOutput();
    store.count = SIZE_MAX;
    assert(umi_ai_hybrid_retrieval_rank("target", chunks, 3U, &store,
        &query_embedding, NULL, &reranker, results, 4U) == 0U);
    assert(state.calls == 0U && memcmp(results, saved, sizeof(results)) == 0);
    /* An absent embedding deliberately requests the previous lexical fallback. */
    assert(umi_ai_hybrid_retrieval_rank("target", chunks, 3U, &store,
        NULL, NULL, NULL, results, 4U) == 2U);
    Prepare();
    SeedOutput();
    query_embedding.dimension = SIZE_MAX;
    assert(umi_ai_hybrid_retrieval_rank("target", chunks, 3U, &store,
        &query_embedding, NULL, &reranker, results, 4U) == 0U);
    assert(state.calls == 0U && memcmp(results, saved, sizeof(results)) == 0);
    Prepare();
    state.fail_at = 1U;
    state.invalid = NAN;
    SeedOutput();
    assert(umi_ai_hybrid_retrieval_rank("target", chunks, 3U, &store,
        &query_embedding, NULL, &reranker, results, 4U) == 0U);
    assert(state.calls == 2U && memcmp(results, saved, sizeof(results)) == 0);
    settings = umi_ai_hybrid_retrieval_settings_default();
    settings.lexical_weight = -1.0; /* finite signed weights remain supported */
    settings.vector_weight = 0.0;
    settings.candidate_limit = SIZE_MAX; /* retains the existing clamp */
    assert(umi_ai_hybrid_retrieval_rank("target", chunks, 3U, NULL,
        NULL, &settings, NULL, results, 4U) == 2U);
    assert(isfinite(results[0].score) && results[0].score >= results[1].score);
    SeedOutput();
    assert(umi_ai_hybrid_retrieval_rank("target", chunks, SIZE_MAX, NULL,
        NULL, NULL, NULL, results, 4U) == 0U);
    assert(umi_ai_hybrid_retrieval_rank("target", chunks, 3U, NULL,
        NULL, NULL, NULL, results, 0U) == 0U);
    assert(memcmp(results, saved, sizeof(results)) == 0);
}

int main(void)
{
    LexicalCases();
    RerankerCases();
    HybridCases();
    return 0;
}
