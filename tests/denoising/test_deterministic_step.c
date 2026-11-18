/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/denoising/test_deterministic_step.c
 *
 * PURPOSE:
 *   Verify deterministic epsilon steps, schedule direction, transactional refusals and bounded trajectory composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/denoising.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)
static int Refused(const float *noisy, const float *noise, size_t count,
    float current, float previous, size_t capacity, UmiStatus expected)
{
    float output[5]={91,92,93,94,95},before[5];
    memcpy(before,output,sizeof output);
    CHECK(UmiAiDenoisingDeterministicStep(noisy,noise,count,current,previous,
        output,capacity)==expected);
    CHECK(!memcmp(output,before,sizeof output));
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *mode=argv[1];
    const float noisy[3]={1,2,-1},noise[3]={0.5F,-1,0.25F};
    float output[5]={91,92,93,94,95};
    if(!strcmp(mode,"reference")) {
        /* Independently evaluated scalar equation for alpha=.25 -> .64. */
        const float expected[3]={1.207179677F,3.985640646F,-1.796410162F};
        CHECK(UmiAiDenoisingDeterministicStep(noisy,noise,3U,.25F,.64F,output,5U)==UMI_STATUS_OK);
        for(size_t i=0U;i<3U;++i)CHECK(fabsf(output[i]-expected[i])<1e-6F);
        CHECK(output[3]==94 && output[4]==95);
    } else if(!strcmp(mode,"identity")) {
        const float x[3]={FLT_MAX,-FLT_MAX,-0.0F},e[3]={-FLT_MAX,FLT_MAX,0};
        const float points[3]={FLT_MIN,.25F,1};
        for(size_t t=0U;t<3U;++t) {
            CHECK(UmiAiDenoisingDeterministicStep(x,e,3U,points[t],points[t],output,3U)==UMI_STATUS_OK);
            CHECK(!memcmp(output,x,sizeof x));
        }
    } else if(!strcmp(mode,"recovery")) {
        float expected[3];
        CHECK(UmiAiDenoisingRecover(noisy,noise,3U,.25F,expected,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingDeterministicStep(noisy,noise,3U,.25F,1,output,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(output,expected,sizeof expected));
    } else if(!strcmp(mode,"trajectory")) {
        const float clean[3]={.2F,-.8F,1.2F},eps[3]={1,-.5F,.25F};
        const float points[4]={.0625F,.25F,.5625F,1};
        float state[3],next[3],expected[3],direct[3];
        CHECK(UmiAiDenoisingMix(clean,eps,3U,points[0],state,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingDeterministicStep(state,eps,3U,points[0],1,direct,3U)==UMI_STATUS_OK);
        for(size_t t=1U;t<4U;++t) {
            CHECK(UmiAiDenoisingDeterministicStep(state,eps,3U,points[t-1U],points[t],next,3U)==UMI_STATUS_OK);
            CHECK(UmiAiDenoisingMix(clean,eps,3U,points[t],expected,3U)==UMI_STATUS_OK);
            for(size_t i=0U;i<3U;++i)CHECK(fabsf(next[i]-expected[i])<2e-6F);
            memcpy(state,next,sizeof state);
        }
        for(size_t i=0U;i<3U;++i)CHECK(fabsf(state[i]-clean[i])<2e-6F && fabsf(direct[i]-clean[i])<2e-6F);
    } else if(!strcmp(mode,"coefficients")) {
        const float bad[5]={-.1F,0,NAN,INFINITY,-INFINITY};
        for(size_t i=0U;i<5U;++i) {
            CHECK(!Refused(noisy,noise,3U,bad[i],1,5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(noisy,noise,3U,.25F,bad[i],5U,UMI_STATUS_INVALID_ARGUMENT));
        }
        CHECK(!Refused(noisy,noise,3U,1.01F,1,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(noisy,noise,3U,.25F,1.01F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(noisy,noise,3U,.25F,.125F,5U,UMI_STATUS_INVALID_ARGUMENT));
    } else if(!strcmp(mode,"nonfinite")) {
        const float bad[3]={NAN,INFINITY,-INFINITY};
        for(size_t i=0U;i<3U;++i) {
            const float values[3]={1,2,bad[i]};
            CHECK(!Refused(values,noise,3U,.25F,.64F,5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(noisy,values,3U,.25F,.64F,5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(noisy,values,3U,1,1,5U,UMI_STATUS_INVALID_ARGUMENT));
        }
    } else if(!strcmp(mode,"overflow")) {
        const float x[3]={1,2,FLT_MAX},e[3]={0,0,0};
        CHECK(!Refused(x,e,3U,FLT_MIN,1,5U,UMI_STATUS_INVALID_ARGUMENT));
        const float negative[3]={1,2,-FLT_MAX};
        CHECK(!Refused(negative,e,3U,.25F,1,5U,UMI_STATUS_INVALID_ARGUMENT));
    } else if(!strcmp(mode,"arguments")) {
        CHECK(!Refused(NULL,noise,3U,.25F,1,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(noisy,NULL,3U,.25F,1,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(noisy,noise,0U,.25F,1,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(noisy,noise,UMI_AI_DENOISING_MAX_VALUES+1U,.25F,1,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(noisy,noise,3U,.25F,1,2U,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(UmiAiDenoisingDeterministicStep(noisy,noise,3U,.25F,1,NULL,3U)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(mode,"maximum")) {
        static float x[UMI_AI_DENOISING_MAX_VALUES],e[UMI_AI_DENOISING_MAX_VALUES];
        static float out[UMI_AI_DENOISING_MAX_VALUES+1U];
        for(size_t i=0U;i<UMI_AI_DENOISING_MAX_VALUES;++i){x[i]=1;e[i]=0;}
        out[UMI_AI_DENOISING_MAX_VALUES]=99;
        CHECK(UmiAiDenoisingDeterministicStep(x,e,UMI_AI_DENOISING_MAX_VALUES,.25F,1,out,UMI_AI_DENOISING_MAX_VALUES+1U)==UMI_STATUS_OK);
        for(size_t i=0U;i<UMI_AI_DENOISING_MAX_VALUES;++i)CHECK(out[i]==2);
        CHECK(out[UMI_AI_DENOISING_MAX_VALUES]==99);
    } else if(!strcmp(mode,"grid")) {
        /* Rearranged long-double reference has no x0 intermediate. */
        const float points[5]={.001F,.1F,.5F,.99F,1};
        for(size_t t=0U;t<5U;++t)for(size_t s=t;s<5U;++s) {
            long double ratio=sqrtl((long double)points[s]/points[t]);
            long double weight=sqrtl(1.L-points[s])-ratio*sqrtl(1.L-points[t]);
            CHECK(UmiAiDenoisingDeterministicStep(noisy,noise,3U,points[t],points[s],output,3U)==UMI_STATUS_OK);
            for(size_t i=0U;i<3U;++i) {
                long double expected=ratio*noisy[i]+weight*noise[i];
                CHECK(fabsl((long double)output[i]-expected)<2e-6L*(1.L+fabsl(expected)));
            }
        }
    } else if(!strcmp(mode,"input-lifetime")) {
        float x[3]={1,2,-1},before[3]; memcpy(before,x,sizeof x);
        CHECK(UmiAiDenoisingDeterministicStep(x,x,3U,.25F,.64F,output,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(x,before,sizeof x));
        float saved[3];memcpy(saved,output,sizeof saved);
        memset(x,0,sizeof x);
        CHECK(!memcmp(output,saved,sizeof saved));
    } else return 2;
    return 0;
}
