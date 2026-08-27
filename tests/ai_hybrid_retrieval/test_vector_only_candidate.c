/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_vector_only_candidate.c
 *
 * PURPOSE:
 *   Verify AI hybrid retrieval vector only candidate behavior over the existing retrieval/vector APIs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/hybrid_retrieval.h"
int test_ai_hybrid_vector_only_candidate(void){UmiAiChunk chunks[1]={0};UmiAiVectorStore store;UmiAiEmbedding e,q;UmiAiRetrievalResult results[2];float values[2]={1.0f,0.0f};strcpy(chunks[0].chunk_id,"a");strcpy(chunks[0].text,"unrelated");umi_ai_vector_store_init(&store);assert(umi_ai_embedding_set(&e,values,2)==UMI_STATUS_OK);assert(umi_ai_embedding_set(&q,values,2)==UMI_STATUS_OK);assert(umi_ai_vector_store_add(&store,&chunks[0],&e)==UMI_STATUS_OK);assert(umi_ai_hybrid_retrieval_rank("target",chunks,1,&store,&q,NULL,NULL,results,2)==1U);assert(strcmp(results[0].chunk.chunk_id,"a")==0);return 0;}
