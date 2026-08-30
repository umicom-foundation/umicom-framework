/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_reranker_invalid.c
 *
 * PURPOSE:
 *   Verify AI hybrid retrieval reranker invalid behavior over the existing retrieval/vector APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/reranker.h"
int test_ai_hybrid_reranker_invalid(void){assert(umi_ai_reranker_apply(NULL,NULL,NULL,0)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
