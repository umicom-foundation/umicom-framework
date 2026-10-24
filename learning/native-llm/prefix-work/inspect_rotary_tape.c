/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/prefix-work/inspect_rotary_tape.c
 * PURPOSE: Teaching demonstration of training-tape rotary attention,
 *          gradient propagation, and a single approved SGD update.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_tape.h"
#include <stdio.h>
int main(void)
{
    const UmiAiTrainingRotaryConfig cfg={2U,2U,10000.0};
    const float q[8]={.3F,.1F,.5F,.2F,.7F,.4F,.2F,.6F};
    const float k[8]={.4F,.2F,.1F,.5F,.5F,.6F,.3F,.1F};
    const float v[8]={.6F,.1F,.4F,.5F,.1F,.7F,.6F,.2F};
    const float goal[8]={0};
    UmiAiTrainingTape *tape=NULL;
    UmiAiTrainingTensor *Q=NULL,*K=NULL,*V=NULL,*target=NULL,*attended=NULL,*loss=NULL;
    float lossValue=0.0F;
    float gradients[8]={0};
    UmiStatus status=UmiAiTrainingTapeCreate(&tape);
    if(status!=UMI_STATUS_OK) return 1;
#define STEP(x) do { status=(x); if(status!=UMI_STATUS_OK) goto end; } while(0)
    STEP(UmiAiTrainingTensorCreate(tape,2U,4U,q,8U,1,&Q));
    STEP(UmiAiTrainingTensorCreate(tape,2U,4U,k,8U,1,&K));
    STEP(UmiAiTrainingTensorCreate(tape,2U,4U,v,8U,0,&V));
    STEP(UmiAiTrainingTensorCreate(tape,2U,4U,goal,8U,0,&target));
    STEP(UmiAiTrainingRotaryTapeAttention(tape,Q,K,V,&cfg,2U,2U,NULL,0U,&attended));
    STEP(UmiAiTrainingMeanSquaredError(tape,attended,target,&loss));
    STEP(UmiAiTrainingTensorValues(loss,&lossValue,1U));
    STEP(UmiAiTrainingBackward(tape,loss));
    STEP(UmiAiTrainingTensorGradients(Q,gradients,8U));
    printf("Rotary tape laboratory | loss = %.8f | query gradient[0] = %.8f\n",
           (double)lossValue,(double)gradients[0]);
    STEP(UmiAiTrainingSgdStep(tape,Q,0.05F));
    puts("C23 tape composition, Backward, and native SGD: PASS");
#undef STEP
end:
    if(status!=UMI_STATUS_OK) fprintf(stderr,"rotary tape error: %d\n",(int)status);
    UmiAiTrainingTapeDestroy(tape);
    return status==UMI_STATUS_OK?0:1;
}
