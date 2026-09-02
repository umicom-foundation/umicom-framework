/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/embedding.c
 *
 * PURPOSE:
 *   Copy bounded embeddings and calculate cosine similarity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Cosine similarity compares direction rather than magnitude and is a common baseline score for semantic retrieval.
 */

#include "umicom/ai/embedding.h"
#include <stddef.h>

#include <math.h>
#include <string.h>

/*
 * Copy ai embedding into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_ai_embedding_set(UmiAiEmbedding *embedding,
                               const float *values,
                               size_t dimension)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (embedding == NULL || values == NULL || dimension == 0U ||
        dimension > UMI_AI_EMBEDDING_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(embedding, 0, sizeof(*embedding));
    (void)memcpy(embedding->values, values, dimension * sizeof(values[0]));
    embedding->dimension = dimension;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai embedding cosine operation used by this module and its client
 * applications.
 */
double umi_ai_embedding_cosine(const UmiAiEmbedding *left,
                               const UmiAiEmbedding *right)
{
    size_t index;
    double dot = 0.0;
    double a = 0.0;
    double b = 0.0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL || left->dimension == 0U ||
        left->dimension != right->dimension) {
        return 0.0;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < left->dimension; ++index) {
        double lv = (double)left->values[index];
        double rv = (double)right->values[index];
        dot += lv * rv;
        a += lv * lv;
        b += rv * rv;
    }
    return a > 0.0 && b > 0.0 ? dot / (sqrt(a) * sqrt(b)) : 0.0;
}
