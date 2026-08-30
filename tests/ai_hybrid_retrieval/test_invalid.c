/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_invalid.c
 *
 * PURPOSE:
 *   Verify AI hybrid retrieval invalid behavior over the existing retrieval/vector APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/hybrid_retrieval.h"
int test_ai_hybrid_invalid(void){UmiAiRetrievalResult r[1];assert(umi_ai_hybrid_retrieval_rank(NULL,NULL,0,NULL,NULL,NULL,NULL,r,1)==0U);return 0;}
