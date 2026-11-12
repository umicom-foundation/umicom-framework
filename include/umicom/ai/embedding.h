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

/*
 * This compact reference representation supports deterministic retrieval tests; production providers may supply larger vectors behind the same higher-level contracts.
 */

#ifndef INCLUDE_UMICOM_AI_EMBEDDING_H
#define INCLUDE_UMICOM_AI_EMBEDDING_H

#include <stddef.h>
#include <stdbool.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai embedding data shared with callers of this public contract.
 */
typedef struct UmiAiEmbedding {
    float values[UMI_AI_EMBEDDING_CAPACITY];
    size_t dimension;
} UmiAiEmbedding;

/** Check the active dimension and finite components. Inactive values are ignored. */
bool umi_ai_embedding_valid(const UmiAiEmbedding *embedding);

/**
 * Copy ai embedding into module-owned storage so callers keep ownership of their input
 * values. Inputs may borrow from the destination; refusal preserves every byte.
 * The values array must contain at least dimension readable floats.
 */
UmiStatus umi_ai_embedding_set(UmiAiEmbedding *embedding,
                               const float *values,
                               size_t dimension);
/**
 * Provide the ai embedding cosine operation used by this module and its client
 * applications. Invalid or unequal dimensions and zero magnitude return 0.0.
 * A neutral score alone does not distinguish invalid data from orthogonal vectors.
 */
double umi_ai_embedding_cosine(const UmiAiEmbedding *left,
                               const UmiAiEmbedding *right);

#ifdef __cplusplus
}
#endif

#endif
