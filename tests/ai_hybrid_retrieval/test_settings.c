/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_settings.c
 *
 * PURPOSE:
 *   Verify AI hybrid retrieval settings behavior over the existing retrieval/vector APIs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/hybrid_retrieval.h"
int test_ai_hybrid_settings(void){UmiAiHybridRetrievalSettings s=umi_ai_hybrid_retrieval_settings_default();assert(s.lexical_weight==0.5);assert(s.vector_weight==0.5);assert(s.candidate_limit==UMI_AI_MAX_RETRIEVAL_RESULTS);return 0;}
