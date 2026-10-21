/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_multihead_gradients.c
 * PURPOSE: Independent central finite-difference checks for Q/K/V gradients
 *          across two heads, including aliased inputs.
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr,"multi-head gradient test line %d: %s\n",__LINE__,#expr);return 1; \
} } while (0)

static UmiStatus Loss(const float params[3][12], float *outLoss,
                      float grads[3][12], int aliased)
{
    const float zeros[12] = {0};
    UmiAiTrainingTape *tape=NULL;
    UmiAiTrainingTensor *q=NULL,*k=NULL,*v=NULL,*target=NULL,*out=NULL,*loss=NULL;
    UmiStatus status=UmiAiTrainingTapeCreate(&tape);
    if (status == UMI_STATUS_OK)
        status=UmiAiTrainingTensorCreate(tape,3U,4U,params[0],12U,1,&q);
    if (aliased) {k=q;v=q;}
    else {
        if (status == UMI_STATUS_OK)
            status=UmiAiTrainingTensorCreate(tape,3U,4U,params[1],12U,1,&k);
        if (status == UMI_STATUS_OK)
            status=UmiAiTrainingTensorCreate(tape,3U,4U,params[2],12U,1,&v);
    }
    if(status == UMI_STATUS_OK)
        status=UmiAiTrainingTensorCreate(tape,3U,4U,zeros,12U,0,&target);
    if(status == UMI_STATUS_OK)
        status=UmiAiTrainingMultiHeadCausalAttention(tape,q,k,v,3U,2U,&out);
    if(status == UMI_STATUS_OK)
        status=UmiAiTrainingMeanSquaredError(tape,out,target,&loss);
    if(status == UMI_STATUS_OK)
        status=UmiAiTrainingTensorValues(loss,outLoss,1U);
    if(status == UMI_STATUS_OK && grads != NULL)
        status=UmiAiTrainingBackward(tape,loss);
    if(status == UMI_STATUS_OK && grads != NULL) {
        status=UmiAiTrainingTensorGradients(q,grads[0],12U);
        if (!aliased && status == UMI_STATUS_OK)
            status=UmiAiTrainingTensorGradients(k,grads[1],12U);
        if (!aliased && status == UMI_STATUS_OK)
            status=UmiAiTrainingTensorGradients(v,grads[2],12U);
    }
    UmiAiTrainingTapeDestroy(tape);
    return status;
}

int main(void)
{
    float params[3][12];
    float grads[3][12]={{0}};
    float loss;
    const float step=0.001F;
    for(size_t n=0U;n<3U;++n)
        for(size_t j=0U;j<12U;++j)
            params[n][j]=(float)((int)((n*19U+j*7U)%29U)-14)*0.037F;
    CHECK(Loss((const float (*)[12])params,&loss,grads,0)==UMI_STATUS_OK);
    for(size_t n=0U;n<3U;++n){
        for(size_t j=0U;j<12U;++j){
            float before=params[n][j],plus,minus;
            params[n][j]=before+step;
            CHECK(Loss((const float (*)[12])params,&plus,NULL,0)==UMI_STATUS_OK);
            params[n][j]=before-step;
            CHECK(Loss((const float (*)[12])params,&minus,NULL,0)==UMI_STATUS_OK);
            params[n][j]=before;
            if(fabsf((plus-minus)/(2.0F*step)-grads[n][j])>0.003F){
                fprintf(stderr,"QKV[%zu][%zu] expected %f got %f\n",n,j,
                        (double)((plus-minus)/(2.0F*step)),(double)grads[n][j]);
                return 1;
            }
        }
    }
    /* Aliased operands: all three paths contribute to the SAME gradient. */
    CHECK(Loss((const float (*)[12])params,&loss,grads,1)==UMI_STATUS_OK);
    for(size_t j=0U;j<12U;++j){
        float before=params[0][j],plus,minus;
        params[0][j]=before+step;
        CHECK(Loss((const float (*)[12])params,&plus,NULL,1)==UMI_STATUS_OK);
        params[0][j]=before-step;
        CHECK(Loss((const float (*)[12])params,&minus,NULL,1)==UMI_STATUS_OK);
        params[0][j]=before;
        CHECK(fabsf((plus-minus)/(2.0F*step)-grads[0][j])<0.004F);
    }
    puts("multi-head analytic, numerical and alias derivatives passed");
    return 0;
}
