/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_loss/test_loss_allocation.c
 *
 * PURPOSE:
 *   Inject every loss-node allocation failure and verify graph, output and allocation ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
static void *live[64];
static size_t liveCount,attempts,failAt;
static int trackingError;
static void *Remember(void *value)
{
    if(!value)return NULL;
    for(size_t i=0U;i<64U;++i)if(!live[i]){live[i]=value;++liveCount;return value;}
    trackingError=1;free(value);return NULL;
}
static void *CountedMalloc(size_t bytes)
{
    ++attempts;if(attempts==failAt)return NULL;return Remember(malloc(bytes));
}
static void *CountedCalloc(size_t count,size_t bytes)
{
    ++attempts;if(attempts==failAt)return NULL;return Remember(calloc(count,bytes));
}
static void CountedFree(void *value)
{
    if(!value)return;
    for(size_t i=0U;i<64U;++i)if(live[i]==value){live[i]=NULL;--liveCount;free(value);return;}
    trackingError=1;
}
/* Compile the production owner with observed allocations. Its archive supplies
 * the normal matrix dependency; this fixture contains no replacement math. */
#define malloc CountedMalloc
#define calloc CountedCalloc
#define free CountedFree
#include "../../src/ai/training/tape.c"
#undef malloc
#undef calloc
#undef free
int main(void)
{
    UmiAiTrainingTape *tape=NULL;UmiAiTrainingTensor *logits=NULL,*loss=NULL;
    const float values[6]={1,2,3,0,-1,2};const size_t targets[2]={2U,1U};
    const unsigned char selected[2]={1U,0U};
    CHECK(UmiAiTrainingTapeCreate(&tape)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,2U,3U,values,6U,1,&logits)==UMI_STATUS_OK);
    const size_t baselineLive=liveCount;
    attempts=0U;
    CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,2U,selected,2U,&loss)==UMI_STATUS_OK);
    const size_t allocations=attempts;CHECK(allocations==4U&&!trackingError);
    CHECK(UmiAiTrainingTapeRewind(tape,1U)==UMI_STATUS_OK&&liveCount==baselineLive);
    UmiAiTrainingTapeMemory before,after;
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);
    for(size_t failure=1U;failure<=allocations;++failure) {
        attempts=0U;failAt=failure;loss=logits;
        CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,2U,selected,2U,&loss)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(loss==logits&&liveCount==baselineLive&&!trackingError);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
        CHECK(!memcmp(&before,&after,sizeof before));
    }
    failAt=0U;
    CHECK(UmiAiTrainingMaskedCrossEntropy(tape,logits,targets,2U,selected,2U,&loss)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingBackward(tape,loss)==UMI_STATUS_OK);
    /* Exercise the new byte-mask ownership independently of categorical targets.
     * This uses the real production allocator calls and checks both rollback
     * and the existing graph-budget rejection path for leaks. */
    CHECK(UmiAiTrainingTapeRewind(tape,1U)==UMI_STATUS_OK&&liveCount==baselineLive);
    const unsigned char elementMask[6]={1,1,0,0,0,1};
    attempts=0U;
    CHECK(UmiAiTrainingMaskedMeanSquaredError(tape,logits,logits,elementMask,6U,&loss)==UMI_STATUS_OK);
    const size_t elementAllocations=attempts;CHECK(elementAllocations==4U&&!trackingError);
    CHECK(UmiAiTrainingTapeRewind(tape,1U)==UMI_STATUS_OK&&liveCount==baselineLive);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);
    for(size_t failure=1U;failure<=elementAllocations;++failure) {
        attempts=0U;failAt=failure;loss=logits;
        CHECK(UmiAiTrainingMaskedMeanSquaredError(tape,logits,logits,elementMask,6U,&loss)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(loss==logits&&liveCount==baselineLive&&!trackingError);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
        CHECK(!memcmp(&before,&after,sizeof before));
    }
    failAt=0U;
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,before.currentBytes)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingMaskedMeanSquaredError(tape,logits,logits,elementMask,6U,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss==logits&&liveCount==baselineLive&&!trackingError);
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,0U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingMaskedMeanSquaredError(tape,logits,logits,elementMask,6U,&loss)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingBackward(tape,loss)==UMI_STATUS_OK);
    /* Attention adds a probability cache, temporary scores and an owned mask.
     * Refuse each real allocation and require the original tape ownership. */
    CHECK(UmiAiTrainingTapeRewind(tape,1U)==UMI_STATUS_OK&&liveCount==baselineLive);
    attempts=0U;
    CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,logits,logits,logits,2U,1U,selected,2U,&loss)==UMI_STATUS_OK);
    const size_t attentionAllocations=attempts;CHECK(attentionAllocations==6U&&!trackingError);
    CHECK(UmiAiTrainingTapeRewind(tape,1U)==UMI_STATUS_OK&&liveCount==baselineLive);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);
    for(size_t failure=1U;failure<=attentionAllocations;++failure){
        attempts=0U;failAt=failure;loss=logits;
        CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,logits,logits,logits,2U,1U,selected,2U,&loss)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(loss==logits&&liveCount==baselineLive&&!trackingError);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
        CHECK(!memcmp(&before,&after,sizeof before));
    }
    failAt=0U;
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,before.currentBytes)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,logits,logits,logits,2U,1U,selected,2U,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss==logits&&liveCount==baselineLive&&!trackingError);
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,0U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingMaskedMultiHeadCausalAttention(tape,logits,logits,logits,2U,1U,selected,2U,&loss)==UMI_STATUS_OK);
    puts("All six masked-attention allocations and budget refusal preserve ownership");

    /* Weighted error reuses the operation-specific float cache slot. Inject
     * all four allocations and verify this alternate owner is freed too. */
    CHECK(UmiAiTrainingTapeRewind(tape,1U)==UMI_STATUS_OK&&liveCount==baselineLive);
    const float elementWeights[6]={1,0,2,0,3,0};
    attempts=0U;
    CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,logits,logits,elementWeights,6U,&loss)==UMI_STATUS_OK);
    const size_t weightAllocations=attempts;CHECK(weightAllocations==4U&&!trackingError);
    CHECK(UmiAiTrainingTapeRewind(tape,1U)==UMI_STATUS_OK&&liveCount==baselineLive);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);
    for(size_t failure=1U;failure<=weightAllocations;++failure) {
        attempts=0U;failAt=failure;loss=logits;
        CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,logits,logits,elementWeights,6U,&loss)==UMI_STATUS_OUT_OF_MEMORY);
        CHECK(loss==logits&&liveCount==baselineLive&&!trackingError);
        CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
        CHECK(!memcmp(&before,&after,sizeof before));
    }
    failAt=0U;
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,before.currentBytes)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,logits,logits,elementWeights,6U,&loss)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss==logits&&liveCount==baselineLive&&!trackingError);
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,0U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingWeightedMeanSquaredError(tape,logits,logits,elementWeights,6U,&loss)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingBackward(tape,loss)==UMI_STATUS_OK);
    puts("All four weighted-loss allocations and budget refusal preserve ownership");
    UmiAiTrainingTapeDestroy(tape);CHECK(liveCount==0U&&!trackingError);
    puts("All four element-mask allocations and a graph-budget refusal preserve ownership");
    puts("All four loss allocations were refused without leaking or publishing a node");
    return 0;
}
