/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_main.c
 *
 * PURPOSE:
 *   Run focused hybrid retrieval and reranker tests over the existing lexical and vector stores.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

int test_ai_hybrid_settings(void);
int test_ai_hybrid_lexical_only(void);
int test_ai_hybrid_vector_only_candidate(void);
int test_ai_hybrid_merge_same_chunk(void);
int test_ai_hybrid_capacity(void);
int test_ai_hybrid_custom_weights(void);
int test_ai_hybrid_reranker(void);
int test_ai_hybrid_invalid(void);
int test_ai_hybrid_reranker_identity(void);
int test_ai_hybrid_reranker_invalid(void);

int main(void)
{
    if (test_ai_hybrid_settings() != 0) return 1;
    if (test_ai_hybrid_lexical_only() != 0) return 2;
    if (test_ai_hybrid_vector_only_candidate() != 0) return 3;
    if (test_ai_hybrid_merge_same_chunk() != 0) return 4;
    if (test_ai_hybrid_capacity() != 0) return 5;
    if (test_ai_hybrid_custom_weights() != 0) return 6;
    if (test_ai_hybrid_reranker() != 0) return 7;
    if (test_ai_hybrid_invalid() != 0) return 8;
    if (test_ai_hybrid_reranker_identity() != 0) return 9;
    if (test_ai_hybrid_reranker_invalid() != 0) return 10;
    puts("hybrid retrieval tests passed");
    return 0;
}
