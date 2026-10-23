/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_layernorm.c
 * PURPOSE: Layer normalisation row invariance, affine outputs and refusal.
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_layernorm.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"layernorm forward line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
    UmiAiTrainingTape *tape=NULL;
    UmiAiTrainingTensor *x=NULL,*gamma=NULL,*beta=NULL,*out=NULL;
    const float input[8]={1.0F,2.0F,3.0F,4.0F, 2.0F,4.0F,6.0F,8.0F};
    const float unit[4]={1.0F,1.0F,1.0F,1.0F};
    const float zeros[4]={0};
    float result[8]={0};
    CHECK(UmiAiTrainingTapeCreate(&tape)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,2U,4U,input,8U,0,&x)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,1U,4U,unit,4U,1,&gamma)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,1U,4U,zeros,4U,1,&beta)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingLayerNorm(tape,x,gamma,beta,1e-5F,&out)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(out,result,8U)==UMI_STATUS_OK);
    for(size_t row=0U;row<2U;++row){
        double mean=0.0,var=0.0;
        for(size_t c=0U;c<4U;++c)mean+=(double)result[row*4U+c]/4.0;
        for(size_t c=0U;c<4U;++c){double v=(double)result[row*4U+c];var+=v*v/4.0;}
        CHECK(fabs(mean)<1e-5);
        CHECK(fabs(var-1.0)<3e-5);
    }
    /* All tokens are normalised per-row: proportional input rows differ only
     * by the finite epsilon term, not by scale or unrelated batch values. */
    CHECK(fabsf(result[0]-result[4])<0.00001F);
    UmiAiTrainingTensor *sentinel=out;
    CHECK(UmiAiTrainingLayerNorm(tape,x,gamma,beta,0.0F,&sentinel)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(sentinel==out);
    CHECK(UmiAiTrainingLayerNorm(tape,x,gamma,beta,NAN,&sentinel)
          == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiTrainingTapeDestroy(tape);
    puts("layer normalisation forward/affine rejection passed");
    return 0;
}
