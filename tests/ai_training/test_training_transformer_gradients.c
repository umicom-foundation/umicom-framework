/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_transformer_gradients.c
 * PURPOSE: Verify the complete composed attention+FFN+residual block using
 *          independent central finite differences on selected parameters.
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_transformer_gradients.c
 * LICENCE: MIT
 */
#include "transformer_fixture.h"
#include <math.h>
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"transformer derivatives line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static UmiStatus Measure(const UmiTransformerTestData *data,float *outLoss,
                         float inputGradients[16],
                         float paramGradients[T_PARAMETER_COUNT][T_MAX_VALUES],
                         float output[16])
{
    UmiTransformerTestFixture f;
    UmiAiTrainingTensor *out=NULL,*target=NULL,*loss=NULL;
    const float goal[16]={0.6F,-0.1F,0.2F,0.3F,
                          0.4F,0.8F,-0.1F,0.5F,
                          0.3F,-0.2F,0.2F,0.4F,
                          0.1F,0.4F,-0.2F,0.6F};
    UmiStatus s=CreateTestFixture(&f,data);
    if(s!=UMI_STATUS_OK)return s;
    s=UmiAiTrainingTensorCreate(f.tape,4U,4U,goal,16U,0,&target);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingTransformerBlockForward(f.tape,f.input,
        &f.block,&f.config,&out);
    if(s==UMI_STATUS_OK && output!=NULL)
        s=UmiAiTrainingTensorValues(out,output,16U);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingMeanSquaredError(f.tape,out,target,&loss);
    if(s==UMI_STATUS_OK)s=UmiAiTrainingTensorValues(loss,outLoss,1U);
    if(s==UMI_STATUS_OK && inputGradients!=NULL)
        s=UmiAiTrainingBackward(f.tape,loss);
    if(s==UMI_STATUS_OK && inputGradients!=NULL)
        s=UmiAiTrainingTensorGradients(f.input,inputGradients,16U);
    if(s==UMI_STATUS_OK && paramGradients!=NULL){
        for(size_t p=0U;p<T_PARAMETER_COUNT;++p){
            s=UmiAiTrainingTensorGradients(f.parameters[p],paramGradients[p],
                                              TestRows(p)*TestColumns(p));
            if(s!=UMI_STATUS_OK)break;
        }
    }
    FreeTestFixture(&f);
    return s;
}

int main(void)
{
    UmiTransformerTestData data;
    float loss=0.0F,inputGrad[16]={0};
    float grads[T_PARAMETER_COUNT][T_MAX_VALUES]={{0}};
    const size_t groups[]={T_NORM_SCALE_1,T_NORM_BIAS_1,T_QUERY,T_KEY,T_VALUE,
                           T_OUTPUT,T_NORM_SCALE_2,T_NORM_BIAS_2,
                           T_FF_UP,T_FF_UP_BIAS,T_FF_DOWN,T_FF_DOWN_BIAS};
    const float step=0.001F;
    InitialiseTestData(&data);
    CHECK(Measure(&data,&loss,inputGrad,grads,NULL)==UMI_STATUS_OK);
    CHECK(isfinite(loss));
    for(size_t g=0U;g<sizeof(groups)/sizeof(groups[0]);++g){
        size_t p=groups[g];
        for(size_t j=0U;j<3U;++j){
            size_t idx=(j*7U+3U)%(TestRows(p)*TestColumns(p));
            float prev=data.values[p][idx],plus,minus;
            data.values[p][idx]=prev+step;
            CHECK(Measure(&data,&plus,NULL,NULL,NULL)==UMI_STATUS_OK);
            data.values[p][idx]=prev-step;
            CHECK(Measure(&data,&minus,NULL,NULL,NULL)==UMI_STATUS_OK);
            data.values[p][idx]=prev;
            float finite=(plus-minus)/(2.0F*step);
            if(fabsf(finite-grads[p][idx])>0.005F){
                fprintf(stderr,"parameter %zu[%zu] expected %.7f got %.7f\n",
                        p,idx,(double)finite,(double)grads[p][idx]);
                return 1;
            }
        }
    }
    for(size_t j=0U;j<16U;j+=3U){
        float prev=data.input[j],plus,minus;
        data.input[j]=prev+step;
        CHECK(Measure(&data,&plus,NULL,NULL,NULL)==UMI_STATUS_OK);
        data.input[j]=prev-step;
        CHECK(Measure(&data,&minus,NULL,NULL,NULL)==UMI_STATUS_OK);
        data.input[j]=prev;
        CHECK(fabsf((plus-minus)/(2.0F*step)-inputGrad[j])<0.005F);
    }
    /* Complete decoder block must not access the next token even when both
     * layer norms, Q/K/V projections and feed-forward stages are enabled. */
    float baseline[16],modified[16];
    CHECK(Measure(&data,&loss,NULL,NULL,baseline)==UMI_STATUS_OK);
    data.input[7]+=0.3F; /* time 1, sequence 0 */
    data.input[15]-=0.4F;/* time 1, sequence 1 */
    CHECK(Measure(&data,&loss,NULL,NULL,modified)==UMI_STATUS_OK);
    for(size_t c=0U;c<4U;++c){
        CHECK(fabsf(modified[c]-baseline[c])<0.00001F);
        CHECK(fabsf(modified[8U+c]-baseline[8U+c])<0.00001F);
    }
    puts("transformer full-path finite differences and causality passed");
    return 0;
}
