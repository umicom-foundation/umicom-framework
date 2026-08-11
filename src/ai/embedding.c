/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/embedding.c
 *
 * PURPOSE:
 *   Copy bounded embeddings and calculate cosine similarity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Cosine similarity compares direction rather than magnitude and is a common baseline score for semantic retrieval.
 */

#include "umicom/ai/embedding.h"
#include <stddef.h>

#include <math.h>
#include <string.h>

UmiStatus umi_ai_embedding_set(UmiAiEmbedding *embedding,
                               const float *values,
                               size_t dimension)
{
    if (embedding == NULL || values == NULL || dimension == 0U ||
        dimension > UMI_AI_EMBEDDING_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(embedding, 0, sizeof(*embedding));
    (void)memcpy(embedding->values, values, dimension * sizeof(values[0]));
    embedding->dimension = dimension;
    return UMI_STATUS_OK;
}

double umi_ai_embedding_cosine(const UmiAiEmbedding *left,
                               const UmiAiEmbedding *right)
{
    size_t index;
    double dot = 0.0;
    double a = 0.0;
    double b = 0.0;
    if (left == NULL || right == NULL || left->dimension == 0U ||
        left->dimension != right->dimension) {
        return 0.0;
    }
    for (index = 0U; index < left->dimension; ++index) {
        double lv = (double)left->values[index];
        double rv = (double)right->values[index];
        dot += lv * rv;
        a += lv * lv;
        b += rv * rv;
    }
    return a > 0.0 && b > 0.0 ? dot / (sqrt(a) * sqrt(b)) : 0.0;
}
