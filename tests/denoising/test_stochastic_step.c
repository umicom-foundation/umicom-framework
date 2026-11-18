/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/denoising/test_stochastic_step.c
 *
 * PURPOSE:
 *   Check stochastic DDIM coefficients, deterministic endpoints and all-or-nothing output publication.
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
static int Refused(const float *x,const float *eps,const float *z,size_t n,
    float a,float p,float eta,size_t capacity,UmiStatus expected)
{
    float out[5]={91,92,93,94,95},before[5];memcpy(before,out,sizeof out);
    CHECK(UmiAiDenoisingStochasticStep(x,eps,z,n,a,p,eta,out,capacity)==expected);
    CHECK(!memcmp(out,before,sizeof out));return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    const float x[3]={0,1,0},eps[3]={0,0,1},z[3]={1,0,0};
    float out[5]={91,92,93,94,95};
    if(!strcmp(mode,"reference")) {
        /* Independent basis vectors isolate sigma, x and epsilon coefficients. */
        const float expected[3]={.5773502692F,1.4142135624F,-.8164965809F};
        CHECK(UmiAiDenoisingStochasticStep(x,eps,z,3,.25F,.5F,1,out,5)==UMI_STATUS_OK);
        for(size_t i=0;i<3;++i)CHECK(fabsf(out[i]-expected[i])<1e-6F);
        CHECK(out[3]==94 && out[4]==95);
    } else if(!strcmp(mode,"deterministic")) {
        float expected[3];
        const float noise[3]={FLT_MAX,-FLT_MAX,17};
        CHECK(UmiAiDenoisingDeterministicStep(x,eps,3,.25F,.75F,expected,3)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingStochasticStep(x,eps,noise,3,.25F,.75F,0,out,5)==UMI_STATUS_OK);
        CHECK(!memcmp(out,expected,sizeof expected) && out[3]==94);
    } else if(!strcmp(mode,"identity")) {
        const float original[3]={FLT_MAX,-FLT_MAX,-0.0F};
        for(unsigned i=1;i<=4;++i) {
            float a=(float)i/4.0F;
            CHECK(UmiAiDenoisingStochasticStep(original,original,original,3,a,a,1,out,5)==UMI_STATUS_OK);
            CHECK(!memcmp(out,original,sizeof original));
        }
    } else if(!strcmp(mode,"recovery")) {
        float expected[3];
        CHECK(UmiAiDenoisingRecover(x,eps,3,.25F,expected,3)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingStochasticStep(x,eps,z,3,.25F,1,1,out,5)==UMI_STATUS_OK);
        CHECK(!memcmp(out,expected,sizeof expected));
    } else if(!strcmp(mode,"coefficients")) {
        const float bad[5]={-.1F,1.1F,NAN,INFINITY,-INFINITY};
        for(size_t i=0;i<5;++i) {
            CHECK(!Refused(x,eps,z,3,bad[i],1,.5F,5,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(x,eps,z,3,.25F,bad[i],.5F,5,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(x,eps,z,3,.25F,.5F,bad[i],5,UMI_STATUS_INVALID_ARGUMENT));
        }
        CHECK(!Refused(x,eps,z,3,0,1,1,5,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(x,eps,z,3,.75F,.5F,1,5,UMI_STATUS_INVALID_ARGUMENT));
    } else if(!strcmp(mode,"nonfinite")) {
        const float bad[3]={NAN,INFINITY,-INFINITY};
        for(size_t i=0;i<3;++i)for(unsigned endpoint=0;endpoint<4;++endpoint) {
            const float data[3]={1,2,bad[i]};
            float a=.25F,p=endpoint==2?1.0F:endpoint==3?.25F:.5F,e=endpoint==1?0.0F:1.0F;
            CHECK(!Refused(data,eps,z,3,a,p,e,5,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(x,data,z,3,a,p,e,5,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(x,eps,data,3,a,p,e,5,UMI_STATUS_INVALID_ARGUMENT));
        }
    } else if(!strcmp(mode,"overflow")) {
        const float big[3]={1,2,FLT_MAX},zero[3]={0,0,0};
        CHECK(!Refused(big,zero,zero,3,.25F,.5F,1,5,UMI_STATUS_INVALID_ARGUMENT));
        const float neg[3]={1,2,-FLT_MAX};
        CHECK(!Refused(neg,zero,zero,3,.25F,.5F,1,5,UMI_STATUS_INVALID_ARGUMENT));
    } else if(!strcmp(mode,"arguments")) {
        CHECK(!Refused(NULL,eps,z,3,.25F,.5F,1,5,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(x,NULL,z,3,.25F,.5F,1,5,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(x,eps,NULL,3,.25F,.5F,1,5,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(x,eps,z,0,.25F,.5F,1,5,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(x,eps,z,UMI_AI_DENOISING_MAX_VALUES+1U,.25F,.5F,1,5,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(x,eps,z,3,.25F,.5F,1,2,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(UmiAiDenoisingStochasticStep(x,eps,z,3,.25F,.5F,1,NULL,3)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(mode,"maximum")) {
        static float values[UMI_AI_DENOISING_MAX_VALUES],noise[UMI_AI_DENOISING_MAX_VALUES];
        static float result[UMI_AI_DENOISING_MAX_VALUES+1U];
        for(size_t i=0;i<UMI_AI_DENOISING_MAX_VALUES;++i)noise[i]=1;
        result[UMI_AI_DENOISING_MAX_VALUES]=99;
        CHECK(UmiAiDenoisingStochasticStep(values,values,noise,UMI_AI_DENOISING_MAX_VALUES,.25F,.5F,1,result,UMI_AI_DENOISING_MAX_VALUES+1U)==UMI_STATUS_OK);
        for(size_t i=0;i<UMI_AI_DENOISING_MAX_VALUES;++i)CHECK(fabsf(result[i]-.5773502692F)<1e-6F);
        CHECK(result[UMI_AI_DENOISING_MAX_VALUES]==99);
    } else if(!strcmp(mode,"grid")) {
        /* Classic subtractive variance in long double is independent of the
         * production positive-term coefficient expression. */
        for(unsigned ai=1;ai<16;++ai)for(unsigned pi=ai+1;pi<=16;++pi)for(unsigned ei=0;ei<=4;++ei) {
            const float a=(float)ai/16.0F,p=(float)pi/16.0F,e=(float)ei/4.0F;
            long double variance=(long double)e*e*(1.L-p)/(1.L-a)*(1.L-(long double)a/p);
            long double direction=sqrtl(1.L-p-variance);
            CHECK(UmiAiDenoisingStochasticStep(x,eps,z,3,a,p,e,out,5)==UMI_STATUS_OK);
            for(size_t i=0;i<3;++i) {
                long double clean=((long double)x[i]-sqrtl(1.L-a)*eps[i])/sqrtl(a);
                long double expected=sqrtl(p)*clean+direction*eps[i]+sqrtl(variance)*z[i];
                CHECK(fabsl((long double)out[i]-expected)<2e-6L*(1.L+fabsl(expected)));
            }
        }
    } else if(!strcmp(mode,"noise-amplitude")) {
        const float zero[3]={0,0,0},signs[3]={-1,0,1};
        CHECK(UmiAiDenoisingStochasticStep(zero,zero,signs,3,.25F,.5F,.5F,out,5)==UMI_STATUS_OK);
        CHECK(fabsf(out[0]+.2886751346F)<1e-6F && out[1]==0 && fabsf(out[2]-.2886751346F)<1e-6F);
        CHECK(fabsf(out[2]*out[2]-1.0F/12.0F)<1e-6F);
    } else return 2;
    return 0;
}
