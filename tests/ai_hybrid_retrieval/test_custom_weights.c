/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_custom_weights.c
 *
 * PURPOSE:
 *   Verify AI hybrid retrieval custom weights behavior over the existing retrieval/vector APIs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/hybrid_retrieval.h"
/*
 * Exercise test ai hybrid custom weights and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_hybrid_custom_weights(void){UmiAiChunk chunks[1]={0};UmiAiRetrievalResult results[1];UmiAiHybridRetrievalSettings s=umi_ai_hybrid_retrieval_settings_default();strcpy(chunks[0].chunk_id,"a");strcpy(chunks[0].text,"target");s.lexical_weight=1.0;s.vector_weight=0.0;assert(umi_ai_hybrid_retrieval_rank("target",chunks,1,NULL,NULL,&s,NULL,results,1)==1U);assert(results[0].score>=1.0);return 0;}
