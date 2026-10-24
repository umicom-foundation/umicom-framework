/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_tape_rejections.c
 * PURPOSE: Prove invalid-layout and mid-operation memory-budget failures
 * retain the earlier tape graph and never publish an output handle.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_tape.h"
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; \
} } while (0)
int main(void)
{
    const UmiAiTrainingRotaryConfig cfg={2U,2U,10000.0};
    const UmiAiTrainingRotaryConfig invalid={3U,3U,10000.0};
    const uint64_t overflow[2]={UMI_AI_ROTARY_MAX_POSITION, UMI_AI_ROTARY_MAX_POSITION};
    const float values[16]={.1F,.2F,.3F,.4F,.5F,.6F,.7F,.8F,
                            .2F,.3F,.4F,.5F,.6F,.7F,.8F,.9F};
    UmiAiTrainingTape *tape=NULL;
    UmiAiTrainingTensor *input=NULL,*result=NULL,*q=NULL,*k=NULL,*v=NULL;
    UmiAiTrainingTapeMemory before={0},after={0};
    CHECK(UmiAiTrainingTapeCreate(&tape)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,4U,4U,values,16U,1,&input)==UMI_STATUS_OK);
    result=input;
    CHECK(UmiAiTrainingRotaryTapeApply(NULL,input,&cfg,2U,2U,NULL,0U,&result)
        ==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryTapeApply(tape,input,&invalid,2U,2U,NULL,0U,&result)
        ==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryTapeApply(tape,input,&cfg,3U,2U,NULL,0U,&result)
        ==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryTapeApply(tape,input,&cfg,2U,2U,overflow,2U,&result)
        ==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result==input);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,before.currentBytes+100U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryTapeApply(tape,input,&cfg,2U,2U,NULL,0U,&result)
        ==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result==input);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
    CHECK(after.currentBytes==before.currentBytes && after.tensorCount==before.tensorCount);
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,0U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,4U,4U,values,16U,1,&q)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,4U,4U,values,16U,1,&k)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,4U,4U,values,16U,0,&v)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&before)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,before.currentBytes+450U)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryTapeAttention(tape,q,k,v,&cfg,2U,2U,NULL,0U,&result)
        ==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(result==input);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape,&after)==UMI_STATUS_OK);
    CHECK(after.currentBytes==before.currentBytes && after.tensorCount==before.tensorCount);
    CHECK(UmiAiTrainingTapeSetMemoryBudget(tape,0U)==UMI_STATUS_OK);
    UmiAiTrainingTapeDestroy(tape);
    puts("rotary tape: preflight rejection, rollback after allocation, caller output preservation PASS");
    return 0;
}
