/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/denoising/test_velocity.c
 *
 * PURPOSE:
 *   Verify diffusion velocity rotations, independent identities, endpoint semantics and transactional bounds.
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
typedef UmiStatus (*Conversion)(const float *,const float *,size_t,float,float *,size_t);
static const Conversion conversions[2]={UmiAiDenoisingVelocityToNoise,UmiAiDenoisingVelocityToClean};
static int Refused(Conversion convert,const float *noisy,const float *velocity,
    size_t count,float alpha,size_t capacity,UmiStatus status)
{
    float result[5]={91,92,93,94,95},before[5];
    memcpy(before,result,sizeof result);
    CHECK(convert(noisy,velocity,count,alpha,result,capacity)==status);
    CHECK(!memcmp(result,before,sizeof result));
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    const float x[3]={1,2,-1},v[3]={.5F,-1,.25F};
    float noise[5]={91,92,93,94,95},clean[5]={81,82,83,84,85};
    if(!strcmp(mode,"reference")) {
        const float expectedNoise[3]={1.116025404F,1.232050808F,-.741025404F};
        const float expectedClean[3]={.066987298F,1.866025404F,-.716506351F};
        CHECK(UmiAiDenoisingVelocityToNoise(x,v,3U,.25F,noise,5U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingVelocityToClean(x,v,3U,.25F,clean,5U)==UMI_STATUS_OK);
        for(size_t i=0U;i<3U;++i)CHECK(fabsf(noise[i]-expectedNoise[i])<1e-6F && fabsf(clean[i]-expectedClean[i])<1e-6F);
        CHECK(noise[3]==94 && noise[4]==95 && clean[3]==84 && clean[4]==85);
    } else if(!strcmp(mode,"roundtrip")) {
        const float original[3]={.2F,-.8F,1.2F},eps[3]={1,-.5F,.25F};
        const float points[7]={0,FLT_MIN,.001F,.25F,.75F,.999F,1};
        for(size_t t=0U;t<7U;++t) {
            float noisy[3],velocity[3];
            long double a=sqrtl((long double)points[t]),s=sqrtl(1.L-points[t]);
            for(size_t i=0U;i<3U;++i) {
                noisy[i]=(float)(a*original[i]+s*eps[i]);
                velocity[i]=(float)(a*eps[i]-s*original[i]);
            }
            CHECK(UmiAiDenoisingVelocityToNoise(noisy,velocity,3U,points[t],noise,3U)==UMI_STATUS_OK);
            CHECK(UmiAiDenoisingVelocityToClean(noisy,velocity,3U,points[t],clean,3U)==UMI_STATUS_OK);
            for(size_t i=0U;i<3U;++i)CHECK(fabsf(noise[i]-eps[i])<2e-6F && fabsf(clean[i]-original[i])<2e-6F);
        }
    } else if(!strcmp(mode,"endpoints")) {
        const float noisy[3]={FLT_MAX,-FLT_MAX,-0.0F},velocity[3]={-FLT_MAX,FLT_MAX,-0.0F};
        float negative[3];for(size_t i=0U;i<3U;++i)negative[i]=-velocity[i];
        CHECK(UmiAiDenoisingVelocityToNoise(noisy,velocity,3U,0,noise,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(noise,noisy,sizeof noisy));
        CHECK(UmiAiDenoisingVelocityToClean(noisy,velocity,3U,0,clean,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(clean,negative,sizeof negative));
        CHECK(UmiAiDenoisingVelocityToNoise(noisy,velocity,3U,1,noise,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(noise,velocity,sizeof velocity));
        CHECK(UmiAiDenoisingVelocityToClean(noisy,velocity,3U,1,clean,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(clean,noisy,sizeof noisy));
    } else if(!strcmp(mode,"nonfinite")) {
        const float bad[3]={NAN,INFINITY,-INFINITY},points[3]={0,.5F,1};
        for(size_t f=0U;f<2U;++f)for(size_t k=0U;k<3U;++k)for(size_t t=0U;t<3U;++t) {
            const float values[3]={1,2,bad[k]};
            CHECK(!Refused(conversions[f],values,v,3U,points[t],5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(conversions[f],x,values,3U,points[t],5U,UMI_STATUS_INVALID_ARGUMENT));
        }
    } else if(!strcmp(mode,"overflow")) {
        const float big[3]={1,2,FLT_MAX},negative[3]={1,2,-FLT_MAX};
        CHECK(!Refused(conversions[0],big,big,3U,.5F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(conversions[0],negative,negative,3U,.5F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(conversions[1],big,negative,3U,.5F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(conversions[1],negative,big,3U,.5F,5U,UMI_STATUS_INVALID_ARGUMENT));
    } else if(!strcmp(mode,"coefficients")) {
        const float bad[5]={-.001F,1.001F,NAN,INFINITY,-INFINITY};
        for(size_t f=0U;f<2U;++f)for(size_t k=0U;k<5U;++k)
            CHECK(!Refused(conversions[f],x,v,3U,bad[k],5U,UMI_STATUS_INVALID_ARGUMENT));
    } else if(!strcmp(mode,"arguments")) {
        for(size_t f=0U;f<2U;++f) {
            CHECK(!Refused(conversions[f],NULL,v,3U,.5F,5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(conversions[f],x,NULL,3U,.5F,5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(conversions[f],x,v,0U,.5F,5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(conversions[f],x,v,UMI_AI_DENOISING_MAX_VALUES+1U,.5F,5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(conversions[f],x,v,3U,.5F,2U,UMI_STATUS_CAPACITY_EXCEEDED));
            CHECK(conversions[f](x,v,3U,.5F,NULL,3U)==UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if(!strcmp(mode,"maximum")) {
        static float noisy[UMI_AI_DENOISING_MAX_VALUES],velocity[UMI_AI_DENOISING_MAX_VALUES];
        static float result[UMI_AI_DENOISING_MAX_VALUES+1U];
        for(size_t i=0U;i<UMI_AI_DENOISING_MAX_VALUES;++i){noisy[i]=1;velocity[i]=0;}
        result[UMI_AI_DENOISING_MAX_VALUES]=99;
        for(size_t f=0U;f<2U;++f) {
            CHECK(conversions[f](noisy,velocity,UMI_AI_DENOISING_MAX_VALUES,.5F,result,UMI_AI_DENOISING_MAX_VALUES+1U)==UMI_STATUS_OK);
            for(size_t i=0U;i<UMI_AI_DENOISING_MAX_VALUES;++i)CHECK(fabsf(result[i]-.707106781F)<1e-6F);
            CHECK(result[UMI_AI_DENOISING_MAX_VALUES]==99);
        }
    } else if(!strcmp(mode,"norm")) {
        /* Orthogonal rotation conserves squared length independently of the
         * implementation's two formulas. This catches coefficient/sign swaps. */
        for(unsigned t=0U;t<=100U;++t) {
            float alpha=(float)t/100.0F;
            CHECK(UmiAiDenoisingVelocityToNoise(x,v,3U,alpha,noise,3U)==UMI_STATUS_OK);
            CHECK(UmiAiDenoisingVelocityToClean(x,v,3U,alpha,clean,3U)==UMI_STATUS_OK);
            for(size_t i=0U;i<3U;++i) {
                double before=(double)x[i]*x[i]+(double)v[i]*v[i];
                double after=(double)clean[i]*clean[i]+(double)noise[i]*noise[i];
                CHECK(fabs(before-after)<2e-6*(1+before));
            }
        }
    } else if(!strcmp(mode,"composition")) {
        float recovered[3],step[3];
        CHECK(UmiAiDenoisingVelocityToNoise(x,v,3U,.25F,noise,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingVelocityToClean(x,v,3U,.25F,clean,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingRecover(x,noise,3U,.25F,recovered,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingDeterministicStep(x,noise,3U,.25F,1,step,3U)==UMI_STATUS_OK);
        for(size_t i=0U;i<3U;++i)CHECK(fabsf(clean[i]-recovered[i])<2e-6F && fabsf(clean[i]-step[i])<2e-6F);
    } else if(!strcmp(mode,"guidance")) {
        const float other[3]={-.2F,.8F,-.1F};
        float epsOther[3],guidedEps[3],guidedVelocity[3],converted[3];
        const float scale=2;
        for(size_t i=0U;i<3U;++i)guidedVelocity[i]=v[i]+scale*(other[i]-v[i]);
        CHECK(UmiAiDenoisingVelocityToNoise(x,v,3U,.25F,noise,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingVelocityToNoise(x,other,3U,.25F,epsOther,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingGuideNoise(noise,epsOther,3U,scale,guidedEps,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingVelocityToNoise(x,guidedVelocity,3U,.25F,converted,3U)==UMI_STATUS_OK);
        for(size_t i=0U;i<3U;++i)CHECK(fabsf(guidedEps[i]-converted[i])<2e-6F);
    } else if(!strcmp(mode,"input-lifetime")) {
        for(size_t f=0U;f<2U;++f) {
            float input[3]={1,2,-1},before[3],saved[3];memcpy(before,input,sizeof input);
            CHECK(conversions[f](input,input,3U,.25F,noise,3U)==UMI_STATUS_OK);
            CHECK(!memcmp(input,before,sizeof input));memcpy(saved,noise,sizeof saved);
            memset(input,0,sizeof input);CHECK(!memcmp(noise,saved,sizeof saved));
        }
    } else return 2;
    return 0;
}
