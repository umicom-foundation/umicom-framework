/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_reranker_identity.c
 *
 * PURPOSE:
 *   Verify AI hybrid retrieval reranker identity behavior over the existing retrieval/vector APIs.
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
#include "umicom/ai/reranker.h"
int test_ai_hybrid_reranker_identity(void){UmiAiRetrievalResult r[2]={0};strcpy(r[0].chunk.chunk_id,"a");r[0].score=1.0;strcpy(r[1].chunk.chunk_id,"b");r[1].score=2.0;assert(umi_ai_reranker_apply(NULL,"q",r,2)==UMI_STATUS_OK);assert(strcmp(r[0].chunk.chunk_id,"b")==0);return 0;}
