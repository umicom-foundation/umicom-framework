/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_histogram/test_image_histogram.c
 *
 * PURPOSE:
 *   Check exact channel frequencies, excluded padding, composition and atomic rejection of invalid views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/image_histogram.h"
#include "umicom/ai/training/image_flip.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static const uint8_t pixels[] = {
    0,10,255, 64,20,255, 64,20,128,
    128,30,128, 255,40,0, 255,40,0
};
static int equal(const UmiAiTrainingRgb8Histogram *a, const UmiAiTrainingRgb8Histogram *b)
{
    return a->pixel_count == b->pixel_count && memcmp(a->bins,b->bins,sizeof a->bins)==0;
}
static int golden(void)
{
    const UmiAiTrainingRgb8View source={pixels,sizeof pixels,3,2,9};
    UmiAiTrainingRgb8Histogram result, expected={0};
    expected.pixel_count=6;
    expected.bins[0][0]=1;expected.bins[0][64]=2;expected.bins[0][128]=1;expected.bins[0][255]=2;
    expected.bins[1][10]=1;expected.bins[1][20]=2;expected.bins[1][30]=1;expected.bins[1][40]=2;
    expected.bins[2][0]=2;expected.bins[2][128]=2;expected.bins[2][255]=2;
    memset(&result,0xa5,sizeof result);
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,&result)==UMI_STATUS_OK);
    CHECK(equal(&result,&expected));
    return 0;
}
static int padding(void)
{
    const uint8_t padded[]={0,10,255,64,20,255,64,20,128, 99,99,99,99,
        128,30,128,255,40,0,255,40,0};
    const UmiAiTrainingRgb8View packed={pixels,sizeof pixels,3,2,9};
    const UmiAiTrainingRgb8View source={padded,sizeof padded,3,2,13};
    UmiAiTrainingRgb8Histogram a,b;
    CHECK(UmiAiTrainingImageHistogramRgb8(&packed,&a)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,&b)==UMI_STATUS_OK);
    CHECK(equal(&a,&b));
    const UmiAiTrainingRgb8View oneRow={pixels,9,3,1,SIZE_MAX};
    CHECK(UmiAiTrainingImageHistogramRgb8(&oneRow,&b)==UMI_STATUS_OK);
    CHECK(b.pixel_count==3 && b.bins[0][0]==1 && b.bins[0][64]==2);
    return 0;
}
static int crop(void)
{
    const UmiAiTrainingRgb8View source={pixels,sizeof pixels,3,2,9};
    UmiAiTrainingRgb8View view;UmiAiTrainingRgb8Histogram result,expected={0};
    CHECK(UmiAiTrainingImageCropRgb8(&source,1,0,2,2,&view)==UMI_STATUS_OK);
    CHECK(view.bytes==15 && view.row_stride_bytes==9);
    CHECK(UmiAiTrainingImageHistogramRgb8(&view,&result)==UMI_STATUS_OK);
    expected.pixel_count=4;expected.bins[0][64]=2;expected.bins[0][255]=2;
    expected.bins[1][20]=2;expected.bins[1][40]=2;
    expected.bins[2][0]=2;expected.bins[2][128]=1;expected.bins[2][255]=1;
    CHECK(equal(&result,&expected));
    return 0;
}
static int flip(void)
{
    const UmiAiTrainingRgb8View source={pixels,sizeof pixels,3,2,9};
    uint8_t transformed[sizeof pixels];
    const UmiAiTrainingRgb8View view={transformed,sizeof transformed,3,2,9};
    UmiAiTrainingRgb8Histogram before,after;
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,&before)==UMI_STATUS_OK);
    for(unsigned flags=0;flags<4;++flags){
        CHECK(UmiAiTrainingImageFlipRgb8(&source,flags,transformed,sizeof transformed)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingImageHistogramRgb8(&view,&after)==UMI_STATUS_OK);
        CHECK(equal(&before,&after));
    }
    return 0;
}
static int constant(void)
{
    const uint8_t white[]={255,255,255};
    const UmiAiTrainingRgb8View source={white,3,1,1,3};
    UmiAiTrainingRgb8Histogram result;
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,&result)==UMI_STATUS_OK);
    CHECK(result.pixel_count==1);
    for(size_t channel=0;channel<3;++channel)for(size_t value=0;value<256;++value)
        CHECK(result.bins[channel][value]==(value==255?1U:0U));
    return 0;
}
static int refusal(void)
{
    UmiAiTrainingRgb8View source={pixels,sizeof pixels,3,2,9};
    UmiAiTrainingRgb8Histogram result;unsigned char before[sizeof result];
    memset(&result,0xa5,sizeof result);memcpy(before,&result,sizeof result);
    CHECK(UmiAiTrainingImageHistogramRgb8(NULL,&result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before,&result,sizeof result)==0);
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    source.bytes=sizeof pixels-1U;
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,&result)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before,&result,sizeof result)==0);
    source.pixels=NULL;
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,&result)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before,&result,sizeof result)==0);
    return 0;
}
static int metadata(void)
{
    const UmiAiTrainingRgb8View cases[]={
        {pixels,sizeof pixels,0,2,9},{pixels,sizeof pixels,3,0,9},{pixels,sizeof pixels,3,2,8},
        {pixels,SIZE_MAX,SIZE_MAX,1,SIZE_MAX},{pixels,SIZE_MAX,1,SIZE_MAX,3},
        {pixels,SIZE_MAX,1,2,SIZE_MAX}
    };
    UmiAiTrainingRgb8Histogram result;unsigned char before[sizeof result];
    memset(&result,0xa5,sizeof result);memcpy(before,&result,sizeof result);
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i){
        const UmiStatus expected=i<3?UMI_STATUS_INVALID_ARGUMENT:UMI_STATUS_CAPACITY_EXCEEDED;
        CHECK(UmiAiTrainingImageHistogramRgb8(&cases[i],&result)==expected);
        CHECK(memcmp(before,&result,sizeof result)==0);
    }
    return 0;
}
static int limits(void)
{
    static uint8_t input[UMI_AI_TRAINING_IMAGE_MAX_VALUES];
    memset(input,255,sizeof input);
    const size_t count=UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U;
    UmiAiTrainingRgb8View source={input,sizeof input,count,1,count*3U};
    UmiAiTrainingRgb8Histogram result;
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,&result)==UMI_STATUS_OK);
    CHECK(result.pixel_count==count);
    for(size_t channel=0;channel<3;++channel)for(size_t value=0;value<256;++value)
        CHECK(result.bins[channel][value]==(value==255?(uint32_t)count:0U));
    unsigned char before[sizeof result];memcpy(before,&result,sizeof result);
    source.width=count+1U;source.row_stride_bytes=source.width*3U;
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,&result)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before,&result,sizeof result)==0);
    return 0;
}
static int conservation(void)
{
    uint8_t input[17*13*3];uint32_t seed=17;
    for(size_t i=0;i<sizeof input;++i){seed=seed*UINT32_C(1664525)+UINT32_C(1013904223);input[i]=(uint8_t)(seed>>24U);}
    for(size_t width=1;width<=17;++width)for(size_t height=1;height<=13;++height){
        const UmiAiTrainingRgb8View source={input,sizeof input,width,height,width*3U};
        UmiAiTrainingRgb8Histogram result;
        CHECK(UmiAiTrainingImageHistogramRgb8(&source,&result)==UMI_STATUS_OK);
        CHECK(result.pixel_count==width*height);
        for(size_t channel=0;channel<3;++channel){
            size_t total=0;uint64_t weighted=0,direct=0;
            for(size_t value=0;value<256;++value){total+=result.bins[channel][value];weighted+=(uint64_t)value*result.bins[channel][value];}
            for(size_t pixel=0;pixel<width*height;++pixel)direct+=input[pixel*3U+channel];
            CHECK(total==width*height && weighted==direct);
        }
    }
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strcmp(argv[1],"golden")==0)return golden();
    if(strcmp(argv[1],"padding")==0)return padding();
    if(strcmp(argv[1],"crop")==0)return crop();
    if(strcmp(argv[1],"flip")==0)return flip();
    if(strcmp(argv[1],"constant")==0)return constant();
    if(strcmp(argv[1],"refusal")==0)return refusal();
    if(strcmp(argv[1],"metadata")==0)return metadata();
    if(strcmp(argv[1],"limits")==0)return limits();
    if(strcmp(argv[1],"conservation")==0)return conservation();
    return 2;
}
