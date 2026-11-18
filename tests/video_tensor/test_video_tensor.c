/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_tensor/test_video_tensor.c
 *
 * PURPOSE:
 *   Verify bounded frame selection, explicit tensor order and refusal-preserving video packing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/video_tensor.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
static int same(const float *a, const float *b, size_t count)
{
    for (size_t i=0U;i<count;++i) {
        const float d=a[i]-b[i];
        if (!(d > -0.000001f && d < 0.000001f)) return 0;
    }
    return 1;
}
static int describe(void)
{
    UmiAiTrainingImageSpan span={71U,73U};
    CHECK(UmiAiTrainingImageDescribeRgb8(2U,2U,8U,&span)==UMI_STATUS_OK);
    CHECK(span.required_bytes==14U && span.value_count==12U);
    CHECK(UmiAiTrainingImageDescribeRgb8(1U,1U,SIZE_MAX,&span)==UMI_STATUS_OK);
    CHECK(span.required_bytes==3U && span.value_count==3U);
    const size_t bad[][3]={{0,1,3},{1,0,3},{2,2,5},{SIZE_MAX,1,3},{1,SIZE_MAX,3},{1,2,SIZE_MAX}};
    for(size_t i=0U;i<sizeof bad/sizeof bad[0];++i){
        span=(UmiAiTrainingImageSpan){71U,73U};
        CHECK(UmiAiTrainingImageDescribeRgb8(bad[i][0],bad[i][1],bad[i][2],&span)!=UMI_STATUS_OK);
        CHECK(span.required_bytes==71U && span.value_count==73U);
    }
    CHECK(UmiAiTrainingImageDescribeRgb8(1,1,3,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    for(size_t w=1U;w<=31U;++w)for(size_t h=1U;h<=17U;++h){
        CHECK(UmiAiTrainingImageDescribeRgb8(w,h,w*3U+2U,&span)==UMI_STATUS_OK);
        CHECK(span.value_count==w*h*3U && span.required_bytes==(h-1U)*(w*3U+2U)+w*3U);
    }
    return 0;
}
static int selection(void)
{
    size_t out[8];for(size_t i=0;i<8;++i)out[i]=91U;
    const size_t expected[]={2U,5U,8U,11U};
    CHECK(UmiAiTrainingVideoSelectFrames(12,2,3,4,out,8)==UMI_STATUS_OK);
    CHECK(memcmp(out,expected,sizeof expected)==0 && out[4]==91U && out[7]==91U);
    CHECK(UmiAiTrainingVideoSelectFrames(1,0,SIZE_MAX,1,out,8)==UMI_STATUS_OK && out[0]==0U);
    return 0;
}
static int selection_grid(void)
{
    size_t out[UMI_AI_TRAINING_VIDEO_MAX_FRAMES];
    for(size_t total=1;total<=32;++total)for(size_t first=0;first<total;++first)
    for(size_t step=1;step<=7;++step)for(size_t count=1;count<=8;++count){
        for(size_t i=0;i<UMI_AI_TRAINING_VIDEO_MAX_FRAMES;++i)out[i]=91U;
        size_t expected[8],used=0,at=first;
        while(used<count && at<total){expected[used++]=at;at+=step;}
        UmiStatus status=UmiAiTrainingVideoSelectFrames(total,first,step,count,out,UMI_AI_TRAINING_VIDEO_MAX_FRAMES);
        if(used==count){CHECK(status==UMI_STATUS_OK);CHECK(memcmp(out,expected,count*sizeof out[0])==0);}
        else {CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);for(size_t i=0;i<count;++i)CHECK(out[i]==91U);}
        CHECK(out[count]==91U);
    }
    return 0;
}
static int selection_limits(void)
{
    size_t out[64];for(size_t i=0;i<64;++i)out[i]=91U;
    CHECK(UmiAiTrainingVideoSelectFrames(SIZE_MAX,SIZE_MAX-3U,2U,2U,out,64)==UMI_STATUS_OK);
    CHECK(out[0]==SIZE_MAX-3U && out[1]==SIZE_MAX-1U);
    CHECK(UmiAiTrainingVideoSelectFrames(64,0,1,64,out,64)==UMI_STATUS_OK);
    for(size_t i=0;i<64;++i)CHECK(out[i]==i);
    const size_t bad[][5]={{0,0,1,1,64},{2,2,1,1,64},{2,0,0,1,64},{2,0,1,0,64},
        {100,0,1,65,64},{5,0,1,5,4},{5,0,2,4,64},{SIZE_MAX,SIZE_MAX-3U,2,3,64},
        {SIZE_MAX,0,SIZE_MAX,2,64}};
    for(size_t n=0;n<sizeof bad/sizeof bad[0];++n){
        for(size_t i=0;i<64;++i)out[i]=91U;
        CHECK(UmiAiTrainingVideoSelectFrames(bad[n][0],bad[n][1],bad[n][2],bad[n][3],out,bad[n][4])!=UMI_STATUS_OK);
        for(size_t i=0;i<64;++i)CHECK(out[i]==91U);
    }
    CHECK(UmiAiTrainingVideoSelectFrames(1,0,1,1,NULL,1)==UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
static const uint8_t redGreen[]={255,0,0,0,255,0};
static const uint8_t blueWhite[]={0,0,255,255,255,255};
static int layout(UmiAiTrainingImageLayout order, UmiAiTrainingImageRange range)
{
    const UmiAiTrainingVideoFrame frames[]={{redGreen,6,6},{blueWhite,6,6}};
    const float hwc[]={1,0,0,0,1,0, 0,0,1,1,1,1};
    const float chw[]={1,0,0,1,0,0, 0,1,0,1,1,1};
    float expected[12],out[15];
    for(size_t i=0;i<12;++i){float x=(order==UMI_AI_TRAINING_IMAGE_HWC?hwc:chw)[i];expected[i]=range==UMI_AI_TRAINING_IMAGE_UNIT?x:2*x-1;}
    for(size_t i=0;i<15;++i)out[i]=19.0f;
    CHECK(UmiAiTrainingVideoPackRgb8(frames,2,2,1,order,range,out,15)==UMI_STATUS_OK);
    CHECK(same(out,expected,12) && out[12]==19.0f && out[14]==19.0f);
    CHECK(memcmp(redGreen,(uint8_t[]){255,0,0,0,255,0},6)==0);
    return 0;
}
static int padding(void)
{
    const uint8_t a[]={255,0,0,0,255,0,99,88,0,0,255,255,255,255};
    const uint8_t b[]={0,0,255,255,255,255,77,66,55,255,0,0,0,255,0};
    const UmiAiTrainingVideoFrame frames[]={{a,sizeof a,8},{b,sizeof b,9}};
    const float expected[]={1,0,0,0,1,0,0,0,1,1,1,1, 0,0,1,1,1,1,1,0,0,0,1,0};
    float out[24];
    CHECK(UmiAiTrainingVideoPackRgb8(frames,2,2,2,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,out,24)==UMI_STATUS_OK);
    CHECK(same(out,expected,24));return 0;
}
static int late_refusal(void)
{
    float out[16],before[16];for(size_t i=0;i<16;++i)before[i]=17.0f+(float)i;
    UmiAiTrainingVideoFrame frames[]={{redGreen,6,6},{blueWhite,6,6}};
    for(unsigned mode=0;mode<5;++mode){
        frames[1]=(UmiAiTrainingVideoFrame){blueWhite,6,6};
        if(mode==0U)frames[1].rgb=NULL;
        if(mode==1U)frames[1].rgb_bytes=5U;
        if(mode==2U)frames[1].row_stride_bytes=5U;
        if(mode==3U)frames[1].rgb_bytes=0U;
        memcpy(out,before,sizeof out);
        CHECK(UmiAiTrainingVideoPackRgb8(frames,2,2,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,out,mode==4U?11U:16U)!=UMI_STATUS_OK);
        CHECK(memcmp(out,before,sizeof out)==0);
    }
    return 0;
}
static int maximum(void)
{
    static uint8_t pixels[16383];static float out[65537];
    for(size_t i=0;i<sizeof pixels;++i)pixels[i]=255U;
    for(size_t i=0;i<sizeof out/sizeof out[0];++i)out[i]=17.0f;
    UmiAiTrainingVideoFrame frames[64];
    for(size_t i=0;i<64;++i)frames[i]=(UmiAiTrainingVideoFrame){pixels,sizeof pixels,sizeof pixels};
    CHECK(UmiAiTrainingVideoPackRgb8(frames,4,5461,1,UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_UNIT,out,65537)==UMI_STATUS_OK);
    for(size_t i=0;i<65532;++i)CHECK(out[i]==1.0f);
    CHECK(out[65532]==17.0f && out[65536]==17.0f);
    for(size_t i=0;i<sizeof out/sizeof out[0];++i)out[i]=17.0f;
    CHECK(UmiAiTrainingVideoPackRgb8(frames,5,5461,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,out,65537)==UMI_STATUS_CAPACITY_EXCEEDED);
    for(size_t i=0;i<sizeof out/sizeof out[0];++i)CHECK(out[i]==17.0f);
    CHECK(UmiAiTrainingVideoPackRgb8(frames,64,1,1,UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,out,65537)==UMI_STATUS_OK);
    for(size_t i=0;i<192;++i)CHECK(out[i]==1.0f);
    CHECK(out[192]==17.0f);return 0;
}
static int arguments(void)
{
    const UmiAiTrainingVideoFrame frame={redGreen,6,6};float out[8];
    for(size_t i=0;i<8;++i)out[i]=17.0f;
#define REJECT(f,n,w,h,l,r,c) do { CHECK(UmiAiTrainingVideoPackRgb8(f,n,w,h,l,r,out,c)!=UMI_STATUS_OK);for(size_t j=0;j<8;++j)CHECK(out[j]==17.0f); } while(0)
    REJECT(NULL,1,2,1,0,0,8);REJECT(&frame,0,2,1,0,0,8);REJECT(&frame,65,2,1,0,0,8);
    REJECT(&frame,1,0,1,0,0,8);REJECT(&frame,1,2,0,0,0,8);REJECT(&frame,1,SIZE_MAX,1,0,0,8);
    REJECT(&frame,1,2,1,(UmiAiTrainingImageLayout)9,0,8);
    REJECT(&frame,1,2,1,0,(UmiAiTrainingImageRange)9,8);
    REJECT(&frame,1,2,1,0,0,5);
#undef REJECT
    CHECK(UmiAiTrainingVideoPackRgb8(&frame,1,2,1,0,0,NULL,6)==UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);
    if(!strcmp(argv[1],"describe"))return describe();
    if(!strcmp(argv[1],"selection"))return selection();
    if(!strcmp(argv[1],"selection-grid"))return selection_grid();
    if(!strcmp(argv[1],"selection-limits"))return selection_limits();
    if(!strcmp(argv[1],"thwc"))return layout(UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT);
    if(!strcmp(argv[1],"tchw"))return layout(UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_UNIT);
    if(!strcmp(argv[1],"signed"))return layout(UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_SIGNED_UNIT);
    if(!strcmp(argv[1],"padding"))return padding();
    if(!strcmp(argv[1],"late-refusal"))return late_refusal();
    if(!strcmp(argv[1],"maximum"))return maximum();
    if(!strcmp(argv[1],"arguments"))return arguments();
    return 2;
}
