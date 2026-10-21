/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_layernorm_gradients.c
 * PURPOSE: Check row-normalisation x, gamma and beta gradients against finite
 *          differences, including repeated affine parameters across rows.
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"layernorm derivative line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static UmiStatus Evaluate(const float x[8],const float gamma[4],const float beta[4],
                          float *outLoss,float gx[8],float gg[4],float gb[4]){
    UmiAiTrainingTape *tape=NULL;
    UmiAiTrainingTensor *a=NULL,*g=NULL,*b=NULL,*target=NULL,*out=NULL,*loss=NULL;
    const float targetValues[8]={0.2F,-0.4F,0.8F,0.1F,0.3F,-0.1F,0.6F,-0.5F};
    UmiStatus s=UmiAiTrainingTapeCreate(&tape);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorCreate(tape,2U,4U,x,8U,1,&a);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorCreate(tape,1U,4U,gamma,4U,1,&g);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorCreate(tape,1U,4U,beta,4U,1,&b);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorCreate(tape,2U,4U,targetValues,8U,0,&target);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingLayerNorm(tape,a,g,b,1e-4F,&out);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingMeanSquaredError(tape,out,target,&loss);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorValues(loss,outLoss,1U);
    if(s==UMI_STATUS_OK && gx != NULL)s=UmiAiTrainingBackward(tape,loss);
    if(s==UMI_STATUS_OK && gx != NULL)s=UmiAiTrainingTensorGradients(a,gx,8U);
    if(s==UMI_STATUS_OK && gg != NULL)s=UmiAiTrainingTensorGradients(g,gg,4U);
    if(s==UMI_STATUS_OK && gb != NULL)s=UmiAiTrainingTensorGradients(b,gb,4U);
    UmiAiTrainingTapeDestroy(tape);
    return s;
}
int main(void){
    float x[8]={0.2F,-0.3F,0.5F,0.7F, -0.1F,0.9F,0.6F,-0.5F};
    float gamma[4]={1.1F,0.8F,1.3F,0.9F};
    float beta[4]={0.1F,-0.2F,0.05F,0.2F};
    float gx[8],gg[4],gb[4],loss;
    const float step=0.001F;
    CHECK(Evaluate(x,gamma,beta,&loss,gx,gg,gb)==UMI_STATUS_OK);
    for(size_t group=0U;group<3U;++group){
        float *arr=group==0U?x:(group==1U?gamma:beta);
        const float *grad=group==0U?gx:(group==1U?gg:gb);
        size_t count=group==0U?8U:4U;
        for(size_t j=0U;j<count;++j){
            float prev=arr[j],plus,minus;
            arr[j]=prev+step;
            CHECK(Evaluate(x,gamma,beta,&plus,NULL,NULL,NULL)==UMI_STATUS_OK);
            arr[j]=prev-step;
            CHECK(Evaluate(x,gamma,beta,&minus,NULL,NULL,NULL)==UMI_STATUS_OK);
            arr[j]=prev;
            if(fabsf((plus-minus)/(2.0F*step)-grad[j])>0.0025F){
                fprintf(stderr,"layernorm group %zu element %zu finite %f reverse %f\n",
                    group,j,(double)((plus-minus)/(2.0F*step)),(double)grad[j]);
                return 1;
            }
        }
    }
    puts("layernorm analytic/numerical x/gamma/beta derivatives passed");
    return 0;
}
