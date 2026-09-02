/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_main.c
 *
 * PURPOSE:
 *   Run focused hybrid retrieval and reranker tests over the existing lexical and vector stores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

/*
 * Exercise test ai hybrid settings and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_hybrid_settings(void);
/*
 * Exercise test ai hybrid lexical only and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_hybrid_lexical_only(void);
/*
 * Exercise test ai hybrid vector only candidate and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_hybrid_vector_only_candidate(void);
/*
 * Exercise test ai hybrid merge same chunk and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_hybrid_merge_same_chunk(void);
/*
 * Exercise test ai hybrid capacity and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_hybrid_capacity(void);
/*
 * Exercise test ai hybrid custom weights and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_hybrid_custom_weights(void);
/*
 * Exercise test ai hybrid reranker and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_hybrid_reranker(void);
/*
 * Exercise test ai hybrid invalid and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_hybrid_invalid(void);
/*
 * Exercise test ai hybrid reranker identity and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_hybrid_reranker_identity(void);
/*
 * Exercise test ai hybrid reranker invalid and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_hybrid_reranker_invalid(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_hybrid_settings() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_hybrid_lexical_only() != 0) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_hybrid_vector_only_candidate() != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_hybrid_merge_same_chunk() != 0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_hybrid_capacity() != 0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_hybrid_custom_weights() != 0) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_ai_hybrid_reranker() != 0) return 7;
    /* Apply this operation only while the related capability or state is available. */
    if (test_ai_hybrid_invalid() != 0) return 8;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (test_ai_hybrid_reranker_identity() != 0) return 9;
    /* Apply this operation only while the related capability or state is available. */
    if (test_ai_hybrid_reranker_invalid() != 0) return 10;
    puts("hybrid retrieval tests passed");
    return 0;
}
