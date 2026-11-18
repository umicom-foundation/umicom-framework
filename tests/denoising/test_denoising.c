/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/denoising/test_denoising.c
 *
 * PURPOSE:
 *   Check reference values, inversion limits and nonfinite or overflowing denoising refusals.
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
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    const float clean[]={0.25F,-0.5F,0.0F},noise[]={-1.0F,0.5F,1.0F};
    float output[]={13.0F,14.0F,15.0F,16.0F},before[4];memcpy(before,output,sizeof(output));
    if(strcmp(mode,"reference")==0) {
        CHECK(UmiAiDenoisingMix(clean,noise,3U,0.5F,output,4U)==UMI_STATUS_OK);
        CHECK(fabsf(output[0]-(-0.530330086F))<0.000001F);
        CHECK(output[1]==0.0F&&fabsf(output[2]-0.707106781F)<0.000001F&&output[3]==16.0F);
    } else if(strcmp(mode,"roundtrip")==0) {
        const float coefficients[]={0.001F,0.1F,0.5F,0.99F,1.0F};
        for(size_t i=0;i<sizeof(coefficients)/sizeof(coefficients[0]);++i){
            float recovered[3];
            CHECK(UmiAiDenoisingMix(clean,noise,3U,coefficients[i],output,4U)==UMI_STATUS_OK);
            CHECK(UmiAiDenoisingRecover(output,noise,3U,coefficients[i],recovered,3U)==UMI_STATUS_OK);
            for(size_t j=0;j<3U;++j)CHECK(fabsf(recovered[j]-clean[j])<0.00001F);
        }
    } else if(strcmp(mode,"endpoints")==0) {
        CHECK(UmiAiDenoisingMix(clean,noise,3U,0.0F,output,4U)==UMI_STATUS_OK);
        CHECK(memcmp(output,noise,sizeof(noise))==0);
        CHECK(UmiAiDenoisingMix(clean,noise,3U,1.0F,output,4U)==UMI_STATUS_OK);
        CHECK(memcmp(output,clean,sizeof(clean))==0);
        memcpy(before,output,sizeof(output));
        CHECK(UmiAiDenoisingRecover(clean,noise,3U,0.0F,output,4U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before,output,sizeof(output))==0);
    } else if(strcmp(mode,"nonfinite")==0) {
        const float bad[]={NAN,INFINITY,-INFINITY};
        for(size_t i=0;i<3U;++i)for(int which=0;which<2;++which) {
            float left[]={1.0F,2.0F,3.0F},right[]={0.0F,0.0F,0.0F};
            (which?right:left)[2]=bad[i];
            CHECK(UmiAiDenoisingMix(left,right,3U,0.5F,output,4U)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(before,output,sizeof(output))==0);
            CHECK(UmiAiDenoisingRecover(left,right,3U,0.5F,output,4U)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(before,output,sizeof(output))==0);
        }
    } else if(strcmp(mode,"overflow")==0) {
        const float left[]={0.0F,FLT_MAX},right[]={0.0F,FLT_MAX};
        CHECK(UmiAiDenoisingMix(left,right,2U,0.5F,output,4U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before,output,sizeof(output))==0);
        const float zero[]={0.0F,0.0F};
        CHECK(UmiAiDenoisingRecover(left,zero,2U,FLT_MIN,output,4U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before,output,sizeof(output))==0);
    } else if(strcmp(mode,"alpha")==0) {
        const float bad[]={-0.1F,1.1F,NAN,INFINITY};
        for(size_t i=0;i<sizeof(bad)/sizeof(bad[0]);++i) {
            CHECK(UmiAiDenoisingMix(clean,noise,3U,bad[i],output,4U)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiAiDenoisingRecover(clean,noise,3U,bad[i],output,4U)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(before,output,sizeof(output))==0);
        }
    } else if(strcmp(mode,"capacity")==0) {
        CHECK(UmiAiDenoisingMix(clean,noise,3U,0.5F,output,2U)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiDenoisingRecover(clean,noise,3U,0.5F,output,2U)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before,output,sizeof(output))==0);
        size_t n=UMI_AI_DENOISING_MAX_VALUES;
        float *left=calloc(n,sizeof(float)),*right=calloc(n,sizeof(float)),*out=malloc(n*sizeof(float));
        CHECK(left&&right&&out);
        CHECK(UmiAiDenoisingMix(left,right,n,0.5F,out,n)==UMI_STATUS_OK);
        for(size_t i=0;i<n;++i)CHECK(out[i]==0.0F);
        free(out);free(right);free(left);
    } else if(strcmp(mode,"arguments")==0) {
        CHECK(UmiAiDenoisingMix(NULL,noise,3U,0.5F,output,4U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDenoisingMix(clean,NULL,3U,0.5F,output,4U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDenoisingMix(clean,noise,3U,0.5F,NULL,4U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDenoisingMix(clean,noise,0U,0.5F,output,4U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDenoisingMix(clean,noise,UMI_AI_DENOISING_MAX_VALUES+1U,0.5F,output,4U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before,output,sizeof(output))==0);
    } else return 2;
    return 0;
}
