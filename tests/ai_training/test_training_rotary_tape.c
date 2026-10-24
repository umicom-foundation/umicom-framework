/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_tape.c
 * PURPOSE: Verify end-to-end forward/adjoint against the existing canonical
 * tape and independent RoPE forward/adjoint services. No mocked tensor maths.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_tape.h"
#include "umicom/ai/training/rotary_training.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; \
} } while (0)

static int Near(float actual, float expected, float tolerance)
{
    return isfinite(actual) && isfinite(expected) &&
           fabsf(actual - expected) <= tolerance;
}

/* This first test proves the tape really propagates the chain rule through
 * row gathers, constant rotation matrices, scattering, and final addition. */
static int RunForwardAndGradient(void)
{
    const UmiAiTrainingRotaryConfig cfg = {2U, 2U, 10000.0};
    const uint64_t starts[2] = {0U, 9U};
    const float q[16] = {.12F,.24F,.31F,.42F, .53F,.64F,.75F,.86F,
                         .91F,.81F,.71F,.61F, .51F,.41F,.31F,.21F};
    const float goal[16] = {.05F,.04F,.03F,.02F, .01F,.02F,.03F,.04F,
                            .02F,.03F,.04F,.05F, .06F,.07F,.08F,.09F};
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *input = NULL, *target = NULL;
    UmiAiTrainingTensor *rotated = NULL, *loss = NULL;
    float actual[16], expected[16], gradient[16], expectedGrad[16];
    float upstream[16];
    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,4U,4U,q,16U,1,&input) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape,4U,4U,goal,16U,0,&target) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryTapeApply(tape,input,&cfg,2U,2U,
                                      starts,2U,&rotated) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(rotated,actual,16U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryTensorForward(&cfg,q,4U,2U,2U,
                    starts,2U,expected,16U) == UMI_STATUS_OK);
    for(size_t i=0U;i<16U;++i) CHECK(Near(actual[i],expected[i],0.00004F));
    CHECK(UmiAiTrainingMeanSquaredError(tape,rotated,target,&loss)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingBackward(tape,loss)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorGradients(input,gradient,16U)==UMI_STATUS_OK);
    for(size_t i=0U;i<16U;++i) upstream[i] = 2.0F*(expected[i]-goal[i])/16.0F;
    CHECK(UmiAiTrainingRotaryTensorBackward(&cfg,upstream,4U,2U,2U,
                       starts,2U,expectedGrad,16U) == UMI_STATUS_OK);
    for(size_t i=0U;i<16U;++i) CHECK(Near(gradient[i],expectedGrad[i],0.00005F));
    /* The original Framework SGD owner can update a projected tensor leaf. */
    CHECK(UmiAiTrainingSgdStep(tape,input,0.1F)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(input,actual,16U)==UMI_STATUS_OK);
    CHECK(fabsf(actual[0]-q[0])>0.00001F);
    UmiAiTrainingTapeDestroy(tape);
    return 0;
}

/* Return a scalar loss for three rows of real tape-owned rotary attention.
 * This helper is used to obtain independent central-difference derivatives. */
static UmiStatus AttentionLoss(
    const float q[12], const float k[12], const float v[12],
    float *outLoss, float *outQDerivative, float *outKDerivative)
{
    const UmiAiTrainingRotaryConfig cfg={2U,2U,10000.0};
    const uint64_t start[1] = {UINT64_C(23)};
    const float zero[12]={0};
    UmiAiTrainingTape *tape=NULL;
    UmiAiTrainingTensor *Q=NULL,*K=NULL,*V=NULL,*goal=NULL,*result=NULL,*loss=NULL;
    UmiStatus status=UmiAiTrainingTapeCreate(&tape);
    if(status != UMI_STATUS_OK) return status;
#define STEP(expr) do { status=(expr); if(status!=UMI_STATUS_OK) goto cleanup; } while(0)
    STEP(UmiAiTrainingTensorCreate(tape,3U,4U,q,12U,1,&Q));
    STEP(UmiAiTrainingTensorCreate(tape,3U,4U,k,12U,1,&K));
    STEP(UmiAiTrainingTensorCreate(tape,3U,4U,v,12U,0,&V));
    STEP(UmiAiTrainingTensorCreate(tape,3U,4U,zero,12U,0,&goal));
    STEP(UmiAiTrainingRotaryTapeAttention(tape,Q,K,V,&cfg,3U,2U,
                                               start,1U,&result));
    STEP(UmiAiTrainingMeanSquaredError(tape,result,goal,&loss));
    STEP(UmiAiTrainingTensorValues(loss,outLoss,1U));
    if(outQDerivative!=NULL || outKDerivative!=NULL) {
        STEP(UmiAiTrainingBackward(tape,loss));
        if(outQDerivative!=NULL) STEP(UmiAiTrainingTensorGradients(Q,outQDerivative,12U));
        if(outKDerivative!=NULL) STEP(UmiAiTrainingTensorGradients(K,outKDerivative,12U));
    }
#undef STEP
cleanup:
    UmiAiTrainingTapeDestroy(tape);
    return status;
}

static int RunAttentionGradient(void)
{
    float q[12] = {.2F,.5F,.4F,.3F,.6F,.1F,.7F,.8F,.5F,.4F,.3F,.9F};
    float k[12] = {.7F,.2F,.3F,.8F,.4F,.9F,.2F,.6F,.3F,.5F,.8F,.2F};
    const float v[12]={.2F,.6F,.9F,.1F,.8F,.3F,.4F,.7F,.3F,.1F,.5F,.6F};
    float analyticQ[12]={0}, analyticK[12]={0};
    float baseline=0.0F, plus=0.0F, minus=0.0F;
    const float epsilon=0.001F;
    CHECK(AttentionLoss(q,k,v,&baseline,analyticQ,analyticK)==UMI_STATUS_OK);
    CHECK(baseline>0.0F);
    for(size_t index=0U;index<12U;index+=3U) {
        const float saved=q[index];
        q[index]=saved+epsilon;
        CHECK(AttentionLoss(q,k,v,&plus,NULL,NULL)==UMI_STATUS_OK);
        q[index]=saved-epsilon;
        CHECK(AttentionLoss(q,k,v,&minus,NULL,NULL)==UMI_STATUS_OK);
        q[index]=saved;
        const float numeric=(plus-minus)/(2.0F*epsilon);
        CHECK(Near(analyticQ[index],numeric,0.004F));
    }
    for(size_t index=0U;index<12U;index+=4U) {
        const float saved=k[index];
        k[index]=saved+epsilon;
        CHECK(AttentionLoss(q,k,v,&plus,NULL,NULL)==UMI_STATUS_OK);
        k[index]=saved-epsilon;
        CHECK(AttentionLoss(q,k,v,&minus,NULL,NULL)==UMI_STATUS_OK);
        k[index]=saved;
        const float numeric=(plus-minus)/(2.0F*epsilon);
        CHECK(Near(analyticK[index],numeric,0.004F));
    }
    return 0;
}
int main(void)
{
    CHECK(RunForwardAndGradient()==0);
    CHECK(RunAttentionGradient()==0);
    puts("rotary tape: forward parity, backward adjoint, SGD, attention finite differences PASS");
    return 0;
}
