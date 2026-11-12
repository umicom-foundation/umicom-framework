/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/reranker.c
 *
 * PURPOSE:
 *   Apply optional reranking to existing retrieval results while retaining deterministic ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/reranker.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Provide the sort results operation used by this module and its client applications. */
static void sort_results(
    UmiAiRetrievalResult *results,
    size_t count)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < count; ++index) {
        UmiAiRetrievalResult value = results[index];
        size_t position = index;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (position > 0U &&
               value.score > results[position - 1U].score) {
            results[position] = results[position - 1U];
            --position;
        }

        results[position] = value;
    }
}

/*
 * Perform ai reranker through the module contract so client applications do not duplicate
 * its policy.
 */
/* Validated candidate storage prevents callback failures from publishing partial scores or reordered caller results; the direct-write implementation is The previous implementation is retained for review. */
#if 0
UmiStatus umi_ai_reranker_apply(
    const UmiAiReranker *reranker,
    const char *query,
    UmiAiRetrievalResult *results,
    size_t result_count)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL ||
        (result_count > 0U && results == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (reranker != NULL && reranker->score != NULL) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < result_count; ++index) {
            results[index].score = reranker->score(
                query,
                &results[index].chunk,
                results[index].score,
                reranker->user_data);
        }
    }

    sort_results(results, result_count);
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_ai_reranker_apply(
    const UmiAiReranker *reranker,
    const char *query,
    UmiAiRetrievalResult *results,
    size_t result_count)
{
    UmiAiReranker callback = {0};
    UmiAiRetrievalResult *candidate;
    if (query == NULL || !umi_ai_retrieval_results_valid(results, result_count))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (result_count == 0U) return UMI_STATUS_OK;
    /* Snapshot callback identity before invoking user code. Its borrowed inputs
     * remain valid only for the call; mutation/reentry on this owner is excluded. */
    if (reranker != NULL) callback = *reranker;
    if (callback.score == NULL) {
        sort_results(results, result_count);
        return UMI_STATUS_OK;
    }
    candidate = malloc(result_count * sizeof(*candidate));
    if (candidate == NULL) {
        /* Use Framework's declared status. The earlier undeclared spelling is
         * retained as disabled source so the integration correction is reviewable. */
#if 0
        return UMI_STATUS_NO_MEMORY;
#else
        return UMI_STATUS_OUT_OF_MEMORY;
#endif
    }
    (void)memcpy(candidate, results, result_count * sizeof(*candidate));
    for (size_t index = 0U; index < result_count; ++index) {
        double score = callback.score(query, &candidate[index].chunk,
                                      candidate[index].score, callback.user_data);
        if (!isfinite(score)) {
            free(candidate);
            return UMI_STATUS_INVALID_STATE;
        }
        candidate[index].score = score;
    }
    /* Publication occurs only after every callback score is accepted. Sorting is
     * stable, and neither partial scores nor a changed order escape on refusal. */
    sort_results(candidate, result_count);
    (void)memcpy(results, candidate, result_count * sizeof(*candidate));
    free(candidate);
    return UMI_STATUS_OK;
}
