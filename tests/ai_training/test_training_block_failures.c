/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_block_failures.c
 * PURPOSE: Enforce bounded graph rollback, parameter-shape validation and
 *          preservation of caller output on failure.
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_block_failures.c
 * LICENCE: MIT
 */
#include "transformer_fixture.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"block rollback line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
    UmiTransformerTestData data;
    UmiTransformerTestFixture f,other;
    UmiAiTrainingTensor *out=NULL, *already=NULL;
    UmiAiTrainingTransformerBlockConfig invalid;
    size_t before=0U,after=0U,rows=0U,cols=0U;
    InitialiseTestData(&data);
    CHECK(CreateTestFixture(&f,&data)==UMI_STATUS_OK);
    CHECK(CreateTestFixture(&other,&data)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(f.tape,&before)==UMI_STATUS_OK);
    out=f.input;
    invalid=f.config;invalid.attentionHeads=3U;
    CHECK(UmiAiTrainingTransformerBlockForward(f.tape,f.input,&f.block,&invalid,
          &out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(out==f.input);
    invalid=f.config;invalid.normEpsilon=NAN;
    CHECK(UmiAiTrainingTransformerBlockForward(f.tape,f.input,&f.block,&invalid,
          &out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(out==f.input);
    UmiAiTrainingTransformerBlockParameters bad=f.block;
    bad.queryWeights=other.parameters[T_QUERY];
    CHECK(UmiAiTrainingTransformerBlockForward(f.tape,f.input,&bad,&f.config,
          &out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(out==f.input);
    CHECK(UmiAiTrainingTapeCheckpoint(f.tape,&after)==UMI_STATUS_OK);
    CHECK(before==after);
    CHECK(UmiAiTrainingTransformerBlockForward(f.tape,f.input,&f.block,&f.config,
          &already)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(f.tape,&after)==UMI_STATUS_OK);
    CHECK(after>before);
    CHECK(UmiAiTrainingTapeRollback(f.tape,before)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(f.tape,&after)==UMI_STATUS_OK);
    CHECK(after==before);
    CHECK(UmiAiTrainingTensorShape(f.input,&rows,&cols)==UMI_STATUS_OK);
    CHECK(rows==4U && cols==4U);
    CHECK(UmiAiTrainingTapeRollback(f.tape,before+1U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingTapeCheckpoint(f.tape,&after)==UMI_STATUS_OK);
    CHECK(after==before);
    FreeTestFixture(&other);
    FreeTestFixture(&f);
    puts("invalid shapes/foreign handles/rollback preserve graph passed");
    return 0;
}
