/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/retrieval.c
 *
 * PURPOSE:
 *   Rank retrieval chunks using a deterministic case-insensitive substring baseline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This baseline requires no model and provides a safe fallback when an embedding service is offline.
 */

#include "umicom/ai/retrieval.h"
#include <stddef.h>

#include <ctype.h>
#include <string.h>

/* Provide the same char operation used by this module and its client applications. */
static int same_char(char left, char right)
{
    return tolower((unsigned char)left) == tolower((unsigned char)right);
}

/*
 * Provide the ai retrieval lexical score operation used by this module and its client
 * applications.
 */
double umi_ai_retrieval_lexical_score(const char *query, const char *text)
{
    size_t qlen;
    size_t tlen;
    size_t i;
    size_t hits = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || text == NULL || query[0] == '\0') return 0.0;
    qlen = strlen(query);
    tlen = strlen(text);
    /* Apply this branch only when its contract condition is satisfied. */
    if (qlen > tlen) return 0.0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i + qlen <= tlen; ++i) {
        size_t j;
        /* Visit each bounded item once so every record receives the same rule. */
        for (j = 0U; j < qlen && same_char(query[j], text[i + j]); ++j) {}
        /* Apply this branch only when its contract condition is satisfied. */
        if (j == qlen) ++hits;
    }
    return hits == 0U ? 0.0 : (double)hits + ((double)qlen / (double)(tlen + 1U));
}

/* Provide the ai retrieval rank operation used by this module and its client applications. */
size_t umi_ai_retrieval_rank(const char *query,
                             const UmiAiChunk *chunks,
                             size_t chunk_count,
                             UmiAiRetrievalResult *results,
                             size_t capacity)
{
    size_t i;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || chunks == NULL || results == NULL || capacity == 0U) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < chunk_count; ++i) {
        double score = umi_ai_retrieval_lexical_score(query, chunks[i].text);
        size_t pos;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (score <= 0.0) continue;
        pos = count < capacity ? count++ : capacity - 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count == capacity && score <= results[capacity - 1U].score) continue;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (pos > 0U && score > results[pos - 1U].score) {
            results[pos] = results[pos - 1U];
            --pos;
        }
        results[pos].chunk = chunks[i];
        results[pos].score = score;
    }
    return count;
}
