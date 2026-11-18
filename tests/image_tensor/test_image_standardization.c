/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_tensor/test_image_standardization.c
 *
 * PURPOSE:
 *   Verify explicit per-channel preprocessing, canonical layouts and publication-free refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_tensor.h"
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1; } } while (0)
static const UmiAiTrainingImageStandardization identity={{0,0,0},{1,1,1}};
static int Refuse(const UmiAiTrainingImageStandardization *parameters,UmiStatus expected)
{
    const uint8_t pixel[]={0,100,255};float output[6],before[6];
    for(size_t i=0;i<6;++i)output[i]=(float)(99U+i);
    memcpy(before,output,sizeof before);
    CHECK(UmiAiTrainingImagePackRgb8Standardized(pixel,sizeof pixel,1,1,3,
        UMI_AI_TRAINING_IMAGE_HWC,parameters,output,6)==expected);
    CHECK(memcmp(before,output,sizeof before)==0);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *name=argv[1];
    const uint8_t pair[]={0,0,0,255,255,255};
    UmiAiTrainingImageStandardization p={{.5f,0,1},{.5f,1,.25f}};
    float output[UMI_AI_TRAINING_IMAGE_MAX_VALUES];
    for(size_t i=0;i<UMI_AI_TRAINING_IMAGE_MAX_VALUES;++i)output[i]=99.0f;
    if(!strcmp(name,"golden")){
        const float expected[]={-1,0,-4,1,1,0};
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,2,1,6,
            UMI_AI_TRAINING_IMAGE_HWC,&p,output,7)==UMI_STATUS_OK);
        for(size_t i=0;i<6;++i){CHECK(output[i]==expected[i]);}
        CHECK(output[6]==99);
    }else if(!strcmp(name,"chw")){
        const float expected[]={-1,1,0,1,-4,0};
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,2,1,6,
            UMI_AI_TRAINING_IMAGE_CHW,&p,output,7)==UMI_STATUS_OK);
        for(size_t i=0;i<6;++i){CHECK(output[i]==expected[i]);}
        CHECK(output[6]==99);
    }else if(!strcmp(name,"identity")){
        uint8_t input[256U*3U];float unit[256U*3U];
        for(size_t i=0;i<sizeof input;++i)input[i]=(uint8_t)(i%256U);
        for(int layout=0;layout<2;++layout){
            CHECK(UmiAiTrainingImagePackRgb8(input,sizeof input,256,1,sizeof input,
                (UmiAiTrainingImageLayout)layout,UMI_AI_TRAINING_IMAGE_UNIT,unit,768)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingImagePackRgb8Standardized(input,sizeof input,256,1,sizeof input,
                (UmiAiTrainingImageLayout)layout,&identity,output,769)==UMI_STATUS_OK);
            CHECK(!memcmp(unit,output,sizeof unit));CHECK(output[768]==99);
        }
    }else if(!strcmp(name,"all-values-reference")){
        /* Independent grid indexing and explicit rounding to the documented
         * unit float ensure this does not silently become a fused transform. */
        uint8_t input[2U*773U];memset(input,0xcc,sizeof input);
        p.mean[0]=.12345f;p.mean[1]=-.75f;p.mean[2]=2.25f;
        p.scale[0]=.3f;p.scale[1]=3.5f;p.scale[2]=.125f;
        for(size_t y=0;y<2;++y)for(size_t x=0;x<256;++x)for(size_t c=0;c<3;++c)
            input[y*773U+x*3U+c]=(uint8_t)((x+17U*c+29U*y)%256U);
        for(int layout=0;layout<2;++layout){
            CHECK(UmiAiTrainingImagePackRgb8Standardized(input,773U+768U,256,2,773,
                (UmiAiTrainingImageLayout)layout,&p,output,1537)==UMI_STATUS_OK);
            for(size_t c=0;c<3;++c)for(size_t y=0;y<2;++y)for(size_t x=0;x<256;++x){
                const unsigned value=(unsigned)((x+17U*c+29U*y)%256U);
                const float unit=(float)((double)value/255.0);
                const float expected=(float)(((double)unit-(double)p.mean[c])/(double)p.scale[c]);
                const size_t offset=layout==0?(y*256U+x)*3U+c:c*512U+y*256U+x;
                CHECK(output[offset]==expected && isfinite(output[offset]));
            }
            CHECK(output[1536]==99);
        }
    }else if(!strcmp(name,"maximum")){
        uint8_t input[(UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U)*3U];memset(input,255,sizeof input);
        CHECK(UmiAiTrainingImagePackRgb8Standardized(input,sizeof input,sizeof input/3U,1,sizeof input,
            UMI_AI_TRAINING_IMAGE_CHW,&identity,output,UMI_AI_TRAINING_IMAGE_MAX_VALUES)==UMI_STATUS_OK);
        for(size_t i=0;i<sizeof input;++i)CHECK(output[i]==1.0f);
        CHECK(output[sizeof input]==99);
    }else if(!strcmp(name,"invalid-scale")){
        const float invalid[]={0.0f,-0.0f,-1.0f,NAN,INFINITY,-INFINITY};
        for(size_t c=0;c<3;++c)for(size_t i=0;i<sizeof invalid/sizeof invalid[0];++i){
            p=identity;p.scale[c]=invalid[i];CHECK(!Refuse(&p,UMI_STATUS_INVALID_ARGUMENT));}
    }else if(!strcmp(name,"invalid-mean")){
        const float invalid[]={NAN,INFINITY,-INFINITY};
        for(size_t c=0;c<3;++c)for(size_t i=0;i<sizeof invalid/sizeof invalid[0];++i){
            p=identity;p.mean[c]=invalid[i];CHECK(!Refuse(&p,UMI_STATUS_INVALID_ARGUMENT));}
    }else if(!strcmp(name,"endpoint-overflow")){
        p=identity;p.scale[2]=FLT_TRUE_MIN;CHECK(!Refuse(&p,UMI_STATUS_INVALID_ARGUMENT));
        p=identity;p.mean[2]=FLT_MAX;p.scale[2]=.5f;CHECK(!Refuse(&p,UMI_STATUS_INVALID_ARGUMENT));
        p=identity;p.mean[2]=-FLT_MAX;p.scale[2]=.5f;CHECK(!Refuse(&p,UMI_STATUS_INVALID_ARGUMENT));
        /* Even absent bright pixels do not exempt the model's endpoint policy. */
        const uint8_t black[3]={0,0,0};p=identity;p.scale[2]=FLT_TRUE_MIN;
        CHECK(UmiAiTrainingImagePackRgb8Standardized(black,3,1,1,3,
            UMI_AI_TRAINING_IMAGE_HWC,&p,output,3)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(output[0]==99 && output[1]==99 && output[2]==99);
    }else if(!strcmp(name,"extreme-finite")){
        p.mean[0]=FLT_MAX;p.mean[1]=-FLT_MAX;p.mean[2]=0;
        p.scale[0]=FLT_MAX;p.scale[1]=FLT_MAX;p.scale[2]=FLT_MAX;
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,2,1,6,
            UMI_AI_TRAINING_IMAGE_HWC,&p,output,7)==UMI_STATUS_OK);
        CHECK(output[0]==-1 && output[1]==1 && output[2]==0);
        for(size_t i=0;i<6;++i){CHECK(isfinite(output[i]));}
        CHECK(output[6]==99);
    }else if(!strcmp(name,"shape-refusal")){
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,0,1,6,
            UMI_AI_TRAINING_IMAGE_HWC,&identity,output,6)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,SIZE_MAX,1,SIZE_MAX,
            UMI_AI_TRAINING_IMAGE_HWC,&identity,output,6)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,1,2,SIZE_MAX,
            UMI_AI_TRAINING_IMAGE_HWC,&identity,output,6)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,2,1,5,
            UMI_AI_TRAINING_IMAGE_HWC,&identity,output,6)==UMI_STATUS_INVALID_ARGUMENT);
        for(size_t i=0;i<6;++i)CHECK(output[i]==99);
    }else if(!strcmp(name,"span-refusal")){
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,5,2,1,6,
            UMI_AI_TRAINING_IMAGE_HWC,&identity,output,6)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,2,1,6,
            UMI_AI_TRAINING_IMAGE_HWC,&identity,output,5)==UMI_STATUS_CAPACITY_EXCEEDED);
        for(size_t i=0;i<6;++i)CHECK(output[i]==99);
    }else if(!strcmp(name,"arguments")){
        CHECK(!Refuse(NULL,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(UmiAiTrainingImagePackRgb8Standardized(NULL,6,2,1,6,
            UMI_AI_TRAINING_IMAGE_HWC,&identity,output,6)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,2,1,6,
            UMI_AI_TRAINING_IMAGE_HWC,&identity,NULL,6)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingImagePackRgb8Standardized(pair,6,2,1,6,
            (UmiAiTrainingImageLayout)99,&identity,output,6)==UMI_STATUS_INVALID_ARGUMENT);
        for(size_t i=0;i<6;++i)CHECK(output[i]==99);
    }else return 2;
    return 0;
}
