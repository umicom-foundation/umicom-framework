/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_lexical_only.c
 *
 * PURPOSE:
 *   Verify AI hybrid retrieval lexical only behavior over the existing retrieval/vector APIs.
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
 * Exercise test ai hybrid lexical only and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_hybrid_lexical_only(void){UmiAiChunk chunks[2]={0};UmiAiRetrievalResult results[2];strcpy(chunks[0].chunk_id,"a");strcpy(chunks[0].text,"alpha target text");strcpy(chunks[1].chunk_id,"b");strcpy(chunks[1].text,"other");assert(umi_ai_hybrid_retrieval_rank("target",chunks,2,NULL,NULL,NULL,NULL,results,2)==1U);assert(strcmp(results[0].chunk.chunk_id,"a")==0);return 0;}
