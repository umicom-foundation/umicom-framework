/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_hybrid_retrieval/test_reranker.c
 *
 * PURPOSE:
 *   Verify AI hybrid retrieval reranker behavior over the existing retrieval/vector APIs.
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
 * Exercise reverse score and return a clear result when the behaviour no longer matches
 * its contract.
 */
static double reverse_score(const char*q,const UmiAiChunk*c,double base,void*u){(void)q;(void)u;return strcmp(c->chunk_id,"b")==0?base+100.0:base;}
/*
 * Exercise test ai hybrid reranker and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_hybrid_reranker(void){UmiAiChunk chunks[2]={0};UmiAiRetrievalResult results[2];UmiAiReranker r={reverse_score,NULL};strcpy(chunks[0].chunk_id,"a");strcpy(chunks[0].text,"target");strcpy(chunks[1].chunk_id,"b");strcpy(chunks[1].text,"target");assert(umi_ai_hybrid_retrieval_rank("target",chunks,2,NULL,NULL,NULL,&r,results,2)==2U);assert(strcmp(results[0].chunk.chunk_id,"b")==0);return 0;}
