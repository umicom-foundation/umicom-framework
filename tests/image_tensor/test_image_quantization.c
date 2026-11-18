/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_tensor/test_image_quantization.c
 *
 * PURPOSE:
 *   Check explicit tensor-to-pixel policy, exact byte recovery and transactional refusal without a model.
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
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"image quantization check: %d\n",__LINE__); return 1; } } while (0)
static uint8_t largeRgb[UMI_AI_TRAINING_IMAGE_MAX_VALUES+8U];
static float largeValues[UMI_AI_TRAINING_IMAGE_MAX_VALUES+1U];
static int Refused(const float *values, size_t count, size_t width, size_t height,
    size_t stride, UmiAiTrainingImageLayout layout, UmiAiTrainingImageRange range,
    UmiAiTrainingImageQuantization policy, size_t bytes, UmiStatus expected)
{
    uint8_t output[32], before[32];
    memset(output,0xA5,sizeof output);
    memcpy(before,output,sizeof before);
    return UmiAiTrainingImageUnpackRgb8(values,count,width,height,stride,layout,range,
        policy,output,bytes)==expected && memcmp(output,before,sizeof output)==0;
}
int main(int argc, char **argv)
{
    CHECK(argc==2);
    const char *mode=argv[1];
    const float rgb[12]={1,0,0, 0,1,0, 0,0,1, 1,1,1};
    const uint8_t expected[12]={255,0,0, 0,255,0, 0,0,255, 255,255,255};
    uint8_t output[32];
    memset(output,0xA5,sizeof output);
    if (!strcmp(mode,"unit")) {
        CHECK(UmiAiTrainingImageUnpackRgb8(rgb,12,2,2,6,0,0,0,output,sizeof output)==UMI_STATUS_OK);
        CHECK(memcmp(output,expected,sizeof expected)==0);
        for(size_t i=12;i<sizeof output;++i)CHECK(output[i]==0xA5);
    } else if (!strcmp(mode,"signed")) {
        const float input[6]={-1,0,1, -0.5F,0.5F,-0.0F};
        const uint8_t wanted[6]={0,128,255,64,191,128};
        CHECK(UmiAiTrainingImageUnpackRgb8(input,6,2,1,6,0,
            UMI_AI_TRAINING_IMAGE_SIGNED_UNIT,0,output,sizeof output)==UMI_STATUS_OK);
        CHECK(memcmp(output,wanted,sizeof wanted)==0);
    } else if (!strcmp(mode,"chw")) {
        const float planes[12]={1,0,0,1, 0,1,0,1, 0,0,1,1};
        CHECK(UmiAiTrainingImageUnpackRgb8(planes,12,2,2,6,
            UMI_AI_TRAINING_IMAGE_CHW,0,0,output,sizeof output)==UMI_STATUS_OK);
        CHECK(memcmp(output,expected,sizeof expected)==0);
    } else if (!strcmp(mode,"halfway")) {
        const float values[6]={nextafterf(0.5F,0.0F),0.5F,nextafterf(0.5F,1.0F),
            0.25F,0.75F,-0.0F};
        const uint8_t wanted[6]={127,128,128,64,191,0};
        CHECK(UmiAiTrainingImageUnpackRgb8(values,6,2,1,6,0,0,0,output,sizeof output)==UMI_STATUS_OK);
        CHECK(memcmp(output,wanted,sizeof wanted)==0);
    } else if (!strcmp(mode,"clipping")) {
        const float values[6]={-FLT_MAX,0.5F,FLT_MAX,-2,1,-0.0F};
        const uint8_t unit[6]={0,128,255,0,255,0}, signedUnit[6]={0,191,255,0,255,128};
        CHECK(UmiAiTrainingImageUnpackRgb8(values,6,2,1,6,0,0,
            UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE,output,sizeof output)==UMI_STATUS_OK);
        CHECK(memcmp(output,unit,sizeof unit)==0);
        CHECK(UmiAiTrainingImageUnpackRgb8(values,6,2,1,6,0,
            UMI_AI_TRAINING_IMAGE_SIGNED_UNIT,UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE,
            output,sizeof output)==UMI_STATUS_OK);
        CHECK(memcmp(output,signedUnit,sizeof signedUnit)==0);
    } else if (!strcmp(mode,"outside")) {
        float values[12];
        memcpy(values,rgb,sizeof values);
        const float invalid[4]={nextafterf(0.0F,-1.0F),nextafterf(1.0F,2.0F),-FLT_MAX,FLT_MAX};
        for(size_t i=0;i<4;++i) {
            values[11]=invalid[i];
            CHECK(Refused(values,12,2,2,6,0,0,0,32,UMI_STATUS_INVALID_ARGUMENT));
        }
        values[11]=nextafterf(-1.0F,-2.0F);
        CHECK(Refused(values,12,2,2,6,0,UMI_AI_TRAINING_IMAGE_SIGNED_UNIT,0,32,UMI_STATUS_INVALID_ARGUMENT));
    } else if (!strcmp(mode,"nonfinite")) {
        float values[12];
        memcpy(values,rgb,sizeof values);
        const float bad[3]={NAN,INFINITY,-INFINITY};
        for(size_t i=0;i<3;++i)for(int policy=0;policy<2;++policy)for(int range=0;range<2;++range) {
            values[11]=bad[i];
            CHECK(Refused(values,12,2,2,8,UMI_AI_TRAINING_IMAGE_CHW,
                (UmiAiTrainingImageRange)range,(UmiAiTrainingImageQuantization)policy,32,UMI_STATUS_INVALID_ARGUMENT));
        }
    } else if (!strcmp(mode,"padding")) {
        CHECK(UmiAiTrainingImageUnpackRgb8(rgb,12,2,2,8,0,0,0,output,14)==UMI_STATUS_OK);
        CHECK(memcmp(output,expected,6)==0 && memcmp(output+8,expected+6,6)==0);
        CHECK(output[6]==0xA5 && output[7]==0xA5);
        for(size_t i=14;i<sizeof output;++i)CHECK(output[i]==0xA5);
    } else if (!strcmp(mode,"single-row")) {
        CHECK(UmiAiTrainingImageUnpackRgb8(rgb,3,1,1,SIZE_MAX,0,0,0,output,3)==UMI_STATUS_OK);
        CHECK(memcmp(output,expected,3)==0 && output[3]==0xA5);
    } else if (!strcmp(mode,"input-tail")) {
        const float values[6]={0,0.5F,1,NAN,INFINITY,-INFINITY};
        CHECK(UmiAiTrainingImageUnpackRgb8(values,6,1,1,3,0,0,0,output,3)==UMI_STATUS_OK);
        CHECK(output[0]==0 && output[1]==128 && output[2]==255 && output[3]==0xA5);
    } else if (!strcmp(mode,"all-bytes")) {
        uint8_t source[768], recovered[768];
        float values[768];
        for(size_t i=0;i<256;++i) {
            source[i*3U]=(uint8_t)i;
            source[i*3U+1U]=(uint8_t)(255U-i);
            source[i*3U+2U]=(uint8_t)((i*31U)%256U);
        }
        for(int layout=0;layout<2;++layout)for(int range=0;range<2;++range) {
            CHECK(UmiAiTrainingImagePackRgb8(source,sizeof source,16,16,48,
                (UmiAiTrainingImageLayout)layout,(UmiAiTrainingImageRange)range,values,768)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingImageUnpackRgb8(values,768,16,16,48,
                (UmiAiTrainingImageLayout)layout,(UmiAiTrainingImageRange)range,0,recovered,sizeof recovered)==UMI_STATUS_OK);
            CHECK(memcmp(source,recovered,sizeof source)==0);
        }
    } else if (!strcmp(mode,"maximum")) {
        const size_t count=(UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U)*3U;
        for(size_t i=0;i<count;++i)largeValues[i]=1.0F;
        memset(largeRgb,0xA5,sizeof largeRgb);
        CHECK(UmiAiTrainingImageUnpackRgb8(largeValues,count,count/3U,1,count,1,0,0,
            largeRgb,count)==UMI_STATUS_OK);
        for(size_t i=0;i<count;++i)CHECK(largeRgb[i]==255);
        for(size_t i=count;i<sizeof largeRgb;++i)CHECK(largeRgb[i]==0xA5);
    } else if (!strcmp(mode,"shape-refusal")) {
        CHECK(Refused(rgb,12,0,2,6,0,0,0,32,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,12,2,0,6,0,0,0,32,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,12,SIZE_MAX,1,SIZE_MAX,0,0,0,32,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,12,1,SIZE_MAX,3,0,0,0,32,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,12,UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U+1U,1,SIZE_MAX,0,0,0,32,UMI_STATUS_CAPACITY_EXCEEDED));
    } else if (!strcmp(mode,"span-refusal")) {
        CHECK(Refused(rgb,11,2,2,6,0,0,0,32,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,12,2,2,6,0,0,0,11,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,12,2,2,5,0,0,0,32,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,12,1,2,SIZE_MAX,0,0,0,32,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,12,1,3,SIZE_MAX/2U,0,0,0,32,UMI_STATUS_CAPACITY_EXCEEDED));
    } else if (!strcmp(mode,"arguments")) {
        CHECK(Refused(NULL,12,2,2,6,0,0,0,32,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,12,2,2,6,(UmiAiTrainingImageLayout)-1,0,0,32,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,12,2,2,6,0,(UmiAiTrainingImageRange)2,0,32,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,12,2,2,6,0,0,(UmiAiTrainingImageQuantization)2,32,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(UmiAiTrainingImageUnpackRgb8(rgb,12,2,2,6,0,0,0,NULL,12)==UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
