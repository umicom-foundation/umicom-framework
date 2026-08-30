/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/embedding.h
 *
 * PURPOSE:
 *   Store small reference embeddings and compare them using cosine similarity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This compact reference representation supports deterministic retrieval tests; production providers may supply larger vectors behind the same higher-level contracts.
 */

#ifndef INCLUDE_UMICOM_AI_EMBEDDING_H
#define INCLUDE_UMICOM_AI_EMBEDDING_H

#include <stddef.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiEmbedding {
    float values[UMI_AI_EMBEDDING_CAPACITY];
    size_t dimension;
} UmiAiEmbedding;

UmiStatus umi_ai_embedding_set(UmiAiEmbedding *embedding,
                               const float *values,
                               size_t dimension);
double umi_ai_embedding_cosine(const UmiAiEmbedding *left,
                               const UmiAiEmbedding *right);

#ifdef __cplusplus
}
#endif

#endif
