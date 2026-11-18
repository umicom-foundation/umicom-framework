/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/denoising/test_guidance.c
 *
 * PURPOSE:
 *   Verify epsilon guidance endpoints, complete admission, float bounds and composition with native reconstruction.
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
static int Refused(const float *a, const float *b, size_t count, float scale,
    size_t capacity, UmiStatus expected)
{
    float output[5] = {91,92,93,94,95}, before[5];
    memcpy(before,output,sizeof output);
    CHECK(UmiAiDenoisingGuideNoise(a,b,count,scale,output,capacity)==expected);
    CHECK(memcmp(output,before,sizeof output)==0);
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);
    const char *mode=argv[1];
    const float a[3]={1,-2,0.5F},b[3]={3,2,-0.5F};
    float output[5]={91,92,93,94,95};
    if(!strcmp(mode,"reference")) {
        CHECK(UmiAiDenoisingGuideNoise(a,b,3U,2.0F,output,5U)==UMI_STATUS_OK);
        CHECK(output[0]==5 && output[1]==6 && output[2]==-1.5F && output[3]==94 && output[4]==95);
        CHECK(UmiAiDenoisingGuideNoise(a,b,3U,0.5F,output,3U)==UMI_STATUS_OK);
        CHECK(output[0]==2 && output[1]==0 && output[2]==0);
    } else if(!strcmp(mode,"endpoints")) {
        const float large[3]={FLT_MAX,-FLT_MAX,-0.0F},small[3]={1e-30F,-1e-30F,0.0F};
        CHECK(UmiAiDenoisingGuideNoise(large,small,3U,0.0F,output,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(output,large,sizeof large));
        CHECK(UmiAiDenoisingGuideNoise(large,small,3U,1.0F,output,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(output,small,sizeof small));
        CHECK(UmiAiDenoisingGuideNoise(small,large,3U,1.0F,output,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(output,large,sizeof large));
    } else if(!strcmp(mode,"same-predictions")) {
        CHECK(UmiAiDenoisingGuideNoise(a,a,3U,FLT_MAX,output,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(output,a,sizeof a));
    } else if(!strcmp(mode,"nonfinite")) {
        const float bads[3]={NAN,INFINITY,-INFINITY};
        for(size_t i=0U;i<3U;++i) {
            float bad[3]={1,2,bads[i]};
            CHECK(!Refused(a,bad,3U,0.0F,5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(bad,b,3U,1.0F,5U,UMI_STATUS_INVALID_ARGUMENT));
            CHECK(!Refused(a,bad,3U,2.0F,5U,UMI_STATUS_INVALID_ARGUMENT));
        }
    } else if(!strcmp(mode,"scale")) {
        const float bads[4]={-0.1F,NAN,INFINITY,-INFINITY};
        for(size_t i=0U;i<4U;++i)CHECK(!Refused(a,b,3U,bads[i],5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(UmiAiDenoisingGuideNoise(a,b,3U,-0.0F,output,3U)==UMI_STATUS_OK);
        CHECK(!memcmp(output,a,sizeof a));
    } else if(!strcmp(mode,"overflow")) {
        const float left[3]={1,2,-FLT_MAX},right[3]={2,3,FLT_MAX};
        CHECK(!Refused(left,right,3U,2.0F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(right,left,3U,2.0F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(a,b,3U,FLT_MAX,5U,UMI_STATUS_INVALID_ARGUMENT));
    } else if(!strcmp(mode,"arguments")) {
        CHECK(!Refused(NULL,b,3U,1.0F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(a,NULL,3U,1.0F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(a,b,0U,1.0F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(a,b,UMI_AI_DENOISING_MAX_VALUES+1U,1.0F,5U,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!Refused(a,b,3U,1.0F,2U,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(UmiAiDenoisingGuideNoise(a,b,3U,1.0F,NULL,3U)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(mode,"maximum")) {
        static float left[UMI_AI_DENOISING_MAX_VALUES],right[UMI_AI_DENOISING_MAX_VALUES];
        static float out[UMI_AI_DENOISING_MAX_VALUES+1U];
        for(size_t i=0U;i<UMI_AI_DENOISING_MAX_VALUES;++i){left[i]=1;right[i]=3;}
        out[UMI_AI_DENOISING_MAX_VALUES]=99;
        CHECK(UmiAiDenoisingGuideNoise(left,right,UMI_AI_DENOISING_MAX_VALUES,2.0F,out,UMI_AI_DENOISING_MAX_VALUES+1U)==UMI_STATUS_OK);
        for(size_t i=0U;i<UMI_AI_DENOISING_MAX_VALUES;++i)CHECK(out[i]==5);
        CHECK(out[UMI_AI_DENOISING_MAX_VALUES]==99);
    } else if(!strcmp(mode,"recovery")) {
        const float clean[3]={0.1F,0.2F,-0.3F},noise[3]={1,-1,0.5F};
        float noisy[3],guided[3],recovered[3];
        CHECK(UmiAiDenoisingMix(clean,noise,3U,0.25F,noisy,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingGuideNoise(a,noise,3U,1.0F,guided,3U)==UMI_STATUS_OK);
        CHECK(UmiAiDenoisingRecover(noisy,guided,3U,0.25F,recovered,3U)==UMI_STATUS_OK);
        for(size_t i=0U;i<3U;++i)CHECK(fabsf(recovered[i]-clean[i])<2e-6F);
    } else return 2;
    return 0;
}
