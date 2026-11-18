/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_tensor/test_video_output.c
 *
 * PURPOSE:
 *   Verify whole-clip value admission, explicit layout and refusal-preserving RGB publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/video_tensor.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
static int filled(const uint8_t *bytes, size_t count, uint8_t value)
{
    for (size_t i = 0U; i < count; ++i) if (bytes[i] != value) return 0;
    return 1;
}
static int golden(UmiAiTrainingImageLayout layout, UmiAiTrainingImageRange range)
{
    const float hwc[12] = {1,0,0, 0,1,0, 0,0,1, 1,1,1};
    const float chw[12] = {1,0, 0,1, 0,0, 0,1, 0,1, 1,1};
    const uint8_t expected[2][6] = {{255,0,0,0,255,0},{0,0,255,255,255,255}};
    float values[12]; uint8_t pixels[2][8]; memset(pixels, 91, sizeof pixels);
    const float *source = layout == UMI_AI_TRAINING_IMAGE_HWC ? hwc : chw;
    for (size_t i = 0; i < 12; ++i) values[i] = range == UMI_AI_TRAINING_IMAGE_UNIT ? source[i] : 2.0f*source[i]-1.0f;
    const UmiAiTrainingVideoRgb8Output outputs[2] = {{pixels[0],8,6},{pixels[1],8,6}};
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,12,2,2,1,layout,range,
        UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_OK);
    for(size_t i=0;i<2;++i){CHECK(memcmp(pixels[i],expected[i],6)==0);CHECK(pixels[i][6]==91 && pixels[i][7]==91);}
    return 0;
}
static int clipping(void)
{
    const float values[12]={-FLT_MAX,0.5f,FLT_MAX,-1,0,1, -FLT_MAX,0,FLT_MAX,-1,0.5f,1};
    const uint8_t unit[12]={0,128,255,0,0,255,0,0,255,0,128,255};
    const uint8_t signedUnit[12]={0,191,255,0,128,255,0,128,255,0,191,255};
    uint8_t pixels[12];const UmiAiTrainingVideoRgb8Output outputs[2]={{pixels,6,6},{pixels+6,6,6}};
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,12,2,2,1,UMI_AI_TRAINING_IMAGE_HWC,
        UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE,outputs)==UMI_STATUS_OK);
    CHECK(memcmp(pixels,unit,12)==0);
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,12,2,2,1,UMI_AI_TRAINING_IMAGE_HWC,
        UMI_AI_TRAINING_IMAGE_SIGNED_UNIT,UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE,outputs)==UMI_STATUS_OK);
    CHECK(memcmp(pixels,signedUnit,12)==0);return 0;
}
static int padding(void)
{
    const float values[12]={1,0,0,0,1,0,0,0,1,1,1,1};
    const uint8_t expected[12]={255,0,0,0,255,0,0,0,255,255,255,255};
    uint8_t first[13],second[17];memset(first,91,sizeof first);memset(second,91,sizeof second);
    const UmiAiTrainingVideoRgb8Output outputs[2]={{first,sizeof first,7},{second,sizeof second,10}};
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,12,2,1,2,UMI_AI_TRAINING_IMAGE_HWC,
        UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_OK);
    CHECK(memcmp(first,expected,3)==0 && memcmp(first+7,expected+3,3)==0);
    CHECK(filled(first+3,4,91) && filled(first+10,3,91));
    CHECK(memcmp(second,expected+6,3)==0 && memcmp(second+10,expected+9,3)==0);
    CHECK(filled(second+3,7,91) && filled(second+13,4,91));return 0;
}
static int late_value(int nonfinite)
{
    float values[12]={0};uint8_t pixels[2][8];
    const UmiAiTrainingVideoRgb8Output outputs[2]={{pixels[0],8,6},{pixels[1],8,6}};
    const float bad[]={NAN,INFINITY,-INFINITY,-0.01f,1.01f};
    const size_t begin=nonfinite?0U:3U,end=nonfinite?3U:5U;
    for(size_t i=begin;i<end;++i)for(size_t at=0;at<12;++at){
        memset(pixels,91,sizeof pixels);values[at]=bad[i];
        CHECK(UmiAiTrainingVideoUnpackRgb8(values,12,2,2,1,UMI_AI_TRAINING_IMAGE_HWC,
            UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(filled(&pixels[0][0],sizeof pixels,91));
        if(nonfinite){CHECK(UmiAiTrainingVideoUnpackRgb8(values,12,2,2,1,UMI_AI_TRAINING_IMAGE_HWC,
            UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE,outputs)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(filled(&pixels[0][0],sizeof pixels,91));}
        values[at]=0;
    }return 0;
}
static int late_descriptor(int kind)
{
    const float values[12]={0};uint8_t pixels[2][10];memset(pixels,91,sizeof pixels);
    UmiAiTrainingVideoRgb8Output outputs[2]={{pixels[0],10,5},{pixels[1],10,5}};
    UmiStatus expected=UMI_STATUS_INVALID_ARGUMENT;
    if(kind==0){outputs[1].rgb_capacity_bytes=7;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    if(kind==1)outputs[1].row_stride_bytes=2;
    if(kind==2)outputs[1].rgb=NULL;
    if(kind==3){outputs[1].row_stride_bytes=SIZE_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,12,2,1,2,UMI_AI_TRAINING_IMAGE_HWC,
        UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==expected);
    CHECK(filled(&pixels[0][0],sizeof pixels,91));return 0;
}
static int limits(void)
{
    const float values[3]={0};uint8_t pixels[3]={91,91,91};
    UmiAiTrainingVideoRgb8Output outputs[64];for(size_t i=0;i<64;++i)outputs[i]=(UmiAiTrainingVideoRgb8Output){pixels,SIZE_MAX,16383};
    /* Oversized counts and products refuse before reading claimed input or
     * writing the shared sentinel; success callers require disjoint buffers. */
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,3,65,1,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,SIZE_MAX,5,5461,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,2,1,1,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,3,1,SIZE_MAX,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(filled(pixels,3,91));return 0;
}
static int maximum(void)
{
    static float values[65536];static uint8_t pixels[4][16385];
    UmiAiTrainingVideoRgb8Output outputs[4];for(size_t i=0;i<65536;++i)values[i]=0.5f;
    memset(pixels,91,sizeof pixels);
    for(size_t i=0;i<4;++i)outputs[i]=(UmiAiTrainingVideoRgb8Output){pixels[i],16385,16383};
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,65536,4,5461,1,UMI_AI_TRAINING_IMAGE_HWC,
        UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_OK);
    for(size_t i=0;i<4;++i){CHECK(filled(pixels[i],16383,128));CHECK(filled(pixels[i]+16383,2,91));}
    uint8_t tiny[64][3];UmiAiTrainingVideoRgb8Output many[64];
    for(size_t i=0;i<64;++i)many[i]=(UmiAiTrainingVideoRgb8Output){tiny[i],3,3};
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,192,64,1,1,UMI_AI_TRAINING_IMAGE_HWC,
        UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,many)==UMI_STATUS_OK);
    CHECK(filled(&tiny[0][0],sizeof tiny,128));return 0;
}
static int ignored_tail(void)
{
    const float values[9]={0,0.5f,1,1,0.5f,0,NAN,INFINITY,-INFINITY};uint8_t pixels[8];memset(pixels,91,sizeof pixels);
    const UmiAiTrainingVideoRgb8Output outputs[2]={{pixels,3,3},{pixels+3,5,3}};
    const uint8_t expected[8]={0,128,255,255,128,0,91,91};
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,9,2,1,1,UMI_AI_TRAINING_IMAGE_HWC,
        UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_OK);
    CHECK(memcmp(pixels,expected,8)==0);return 0;
}
static int single_row(void)
{
    const float values[6]={0,0.5f,1,1,0,0};uint8_t pixels[6];
    const UmiAiTrainingVideoRgb8Output outputs[2]={{pixels,3,SIZE_MAX},{pixels+3,3,SIZE_MAX}};
    const uint8_t expected[6]={0,128,255,255,0,0};
    CHECK(UmiAiTrainingVideoUnpackRgb8(values,6,2,1,1,UMI_AI_TRAINING_IMAGE_HWC,
        UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_OK);
    CHECK(memcmp(pixels,expected,6)==0);return 0;
}
static int round_trip(void)
{
    uint8_t source[2][768],pixels[2][770];float values[1536];
    UmiAiTrainingVideoFrame inputs[2];UmiAiTrainingVideoRgb8Output outputs[2];
    for(size_t f=0;f<2;++f){for(size_t i=0;i<768;++i)source[f][i]=(uint8_t)((i+f*37U)%256U);
        inputs[f]=(UmiAiTrainingVideoFrame){source[f],768,768};outputs[f]=(UmiAiTrainingVideoRgb8Output){pixels[f],770,768};}
    for(int l=0;l<2;++l)for(int r=0;r<2;++r){memset(pixels,91,sizeof pixels);
        CHECK(UmiAiTrainingVideoPackRgb8(inputs,2,256,1,(UmiAiTrainingImageLayout)l,(UmiAiTrainingImageRange)r,values,1536)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingVideoUnpackRgb8(values,1536,2,256,1,(UmiAiTrainingImageLayout)l,(UmiAiTrainingImageRange)r,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,outputs)==UMI_STATUS_OK);
        for(size_t f=0;f<2;++f){CHECK(memcmp(source[f],pixels[f],768)==0);CHECK(pixels[f][768]==91 && pixels[f][769]==91);}
    }return 0;
}
static int value_admission(void)
{
    float values[4]={0,0.5f,1,NAN};float before[4];memcpy(before,values,sizeof before);
    CHECK(UmiAiTrainingImageValidateValues(values,3,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImageValidateValues(values,4,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(values,before,sizeof values)==0);
    CHECK(UmiAiTrainingImageValidateValues(values,0,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageValidateValues(NULL,3,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageValidateValues(values,SIZE_MAX,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingImageValidateValues(values,3,(UmiAiTrainingImageRange)9,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageValidateValues(values,3,UMI_AI_TRAINING_IMAGE_UNIT,(UmiAiTrainingImageQuantization)9)==UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
static int arguments(void)
{
    const float values[3]={0};uint8_t pixels[3]={91,91,91};const UmiAiTrainingVideoRgb8Output frame={pixels,3,3};
#define BAD(v,n,w,h,l,r,q,f) CHECK(UmiAiTrainingVideoUnpackRgb8(v,3,n,w,h,l,r,q,f)==UMI_STATUS_INVALID_ARGUMENT)
    BAD(NULL,1,1,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,&frame);
    BAD(values,1,1,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,NULL);
    BAD(values,0,1,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,&frame);
    BAD(values,1,0,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,&frame);
    BAD(values,1,1,0,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,&frame);
    BAD(values,1,1,1,(UmiAiTrainingImageLayout)9,UMI_AI_TRAINING_IMAGE_UNIT,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,&frame);
    BAD(values,1,1,1,UMI_AI_TRAINING_IMAGE_HWC,(UmiAiTrainingImageRange)9,UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE,&frame);
    BAD(values,1,1,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,(UmiAiTrainingImageQuantization)9,&frame);
#undef BAD
    CHECK(filled(pixels,3,91));return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strcmp(argv[1],"thwc")==0)return golden(UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT);
    if(strcmp(argv[1],"tchw")==0)return golden(UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_UNIT);
    if(strcmp(argv[1],"signed")==0)return golden(UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_SIGNED_UNIT);
    if(strcmp(argv[1],"clipping")==0)return clipping();
    if(strcmp(argv[1],"padding")==0)return padding();
    if(strcmp(argv[1],"late-nonfinite")==0)return late_value(1);
    if(strcmp(argv[1],"late-outside")==0)return late_value(0);
    if(strcmp(argv[1],"late-capacity")==0)return late_descriptor(0);
    if(strcmp(argv[1],"late-stride")==0)return late_descriptor(1);
    if(strcmp(argv[1],"late-null")==0)return late_descriptor(2);
    if(strcmp(argv[1],"late-overflow")==0)return late_descriptor(3);
    if(strcmp(argv[1],"limits")==0)return limits();
    if(strcmp(argv[1],"maximum")==0)return maximum();
    if(strcmp(argv[1],"ignored-tail")==0)return ignored_tail();
    if(strcmp(argv[1],"single-row")==0)return single_row();
    if(strcmp(argv[1],"round-trip")==0)return round_trip();
    if(strcmp(argv[1],"value-admission")==0)return value_admission();
    if(strcmp(argv[1],"arguments")==0)return arguments();
    return 2;
}
