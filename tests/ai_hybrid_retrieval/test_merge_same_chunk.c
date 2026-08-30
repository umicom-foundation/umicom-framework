/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_merge_same_chunk.c
 *
 * PURPOSE:
 *   Verify AI hybrid retrieval merge same chunk behavior over the existing retrieval/vector APIs.
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
int test_ai_hybrid_merge_same_chunk(void){UmiAiChunk chunks[1]={0};UmiAiVectorStore store;UmiAiEmbedding e,q;UmiAiRetrievalResult results[2];float values[2]={1.0f,0.0f};strcpy(chunks[0].chunk_id,"a");strcpy(chunks[0].text,"target");umi_ai_vector_store_init(&store);assert(umi_ai_embedding_set(&e,values,2)==UMI_STATUS_OK);assert(umi_ai_embedding_set(&q,values,2)==UMI_STATUS_OK);assert(umi_ai_vector_store_add(&store,&chunks[0],&e)==UMI_STATUS_OK);assert(umi_ai_hybrid_retrieval_rank("target",chunks,1,&store,&q,NULL,NULL,results,2)==1U);assert(results[0].score>0.5);return 0;}
