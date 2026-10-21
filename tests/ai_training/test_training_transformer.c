/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_transformer.c
 * PURPOSE: Verify complete pre-LN block forward shape and exact residual
 *          identity when its attention/FF projection outputs are zero.
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "transformer_fixture.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"block forward line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
    UmiTransformerTestData data;
    UmiTransformerTestFixture f;
    UmiAiTrainingTensor *out=NULL;
    float observed[16];
    size_t rows=0U,cols=0U;
    InitialiseTestData(&data);
    /* Set the two output paths to exactly zero without modifying the
     * existing block implementation; the two residuals then preserve input. */
    for(size_t i=0U;i<16U;++i)data.values[T_OUTPUT][i]=0.0F;
    for(size_t i=0U;i<24U;++i)data.values[T_FF_DOWN][i]=0.0F;
    for(size_t i=0U;i<4U;++i)data.values[T_FF_DOWN_BIAS][i]=0.0F;
    CHECK(CreateTestFixture(&f,&data)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTransformerBlockForward(f.tape,f.input,&f.block,
          &f.config,&out)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorShape(out,&rows,&cols)==UMI_STATUS_OK);
    CHECK(rows==4U && cols==4U);
    CHECK(UmiAiTrainingTensorValues(out,observed,16U)==UMI_STATUS_OK);
    for(size_t i=0U;i<16U;++i)CHECK(fabsf(observed[i]-data.input[i])<0.000001F);
    FreeTestFixture(&f);
    puts("pre-LN transformer complete residual identity passed");
    return 0;
}
