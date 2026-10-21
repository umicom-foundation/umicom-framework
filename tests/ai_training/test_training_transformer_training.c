/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_transformer_training.c
 * PURPOSE: Check the composed decoder block can learn a deterministic
 *          input-to-target mapping while retaining source parameters.
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "transformer_fixture.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"block training line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void)
{
    UmiTransformerTestData data;
    UmiTransformerTestFixture f;
    UmiAiTrainingTensor *goal=NULL,*out=NULL,*loss=NULL;
    const float target[16]={0.6F,-0.5F,0.2F,0.9F,
                            -0.1F,0.1F,0.3F,0.4F,
                            0.1F,-0.2F,0.7F,0.2F,
                            0.3F,0.5F,0.1F,-0.4F};
    float first=0.0F,last=0.0F;
    size_t keep=0U;
    InitialiseTestData(&data);
    CHECK(CreateTestFixture(&f,&data)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(f.tape,4U,4U,target,16U,0,&goal)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(f.tape,&keep)==UMI_STATUS_OK);
    for(size_t epoch=0U;epoch<160U;++epoch){
        CHECK(UmiAiTrainingTransformerBlockForward(f.tape,f.input,
            &f.block,&f.config,&out)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingMeanSquaredError(f.tape,out,goal,&loss)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorValues(loss,&last,1U)==UMI_STATUS_OK);
        if(epoch==0U)first=last;
        CHECK(isfinite(last));
        CHECK(UmiAiTrainingBackward(f.tape,loss)==UMI_STATUS_OK);
        for(size_t p=0U;p<T_PARAMETER_COUNT;++p)
            CHECK(UmiAiTrainingSgdStep(f.tape,f.parameters[p],0.08F)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingTapeRollback(f.tape,keep)==UMI_STATUS_OK);
    }
    FreeTestFixture(&f);
    if(!(last<first*0.65F)){
        fprintf(stderr,"block did not train: first %.8f last %.8f\n",
                (double)first,(double)last);
        return 1;
    }
    printf("trainable decoder block regression loss %.7f -> %.7f\n",
           (double)first,(double)last);
    return 0;
}
