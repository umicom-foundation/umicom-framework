/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_flip/test_image_flip.c
 *
 * PURPOSE:
 *   Verify explicit RGB mirrors, borrowed crop composition, capacities and transactional refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_flip.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} } while(0)
static const uint8_t grid[]={1,2,3, 4,5,6, 7,8,9, 10,11,12, 13,14,15, 16,17,18};
static const uint8_t horizontal[]={7,8,9, 4,5,6, 1,2,3, 16,17,18, 13,14,15, 10,11,12};
static const uint8_t vertical[]={10,11,12, 13,14,15, 16,17,18, 1,2,3, 4,5,6, 7,8,9};
static const uint8_t both[]={16,17,18, 13,14,15, 10,11,12, 7,8,9, 4,5,6, 1,2,3};
static int golden(void)
{
    const UmiAiTrainingRgb8View source={grid,sizeof grid,3,2,9};
    const uint8_t *expected[]={grid,horizontal,vertical,both};uint8_t output[23];
    for(unsigned flags=0;flags<4;++flags) {
        memset(output,0xcc,sizeof output);
        CHECK(UmiAiTrainingImageFlipRgb8(&source,flags,output,sizeof output)==UMI_STATUS_OK);
        CHECK(memcmp(output,expected[flags],18)==0);
        for(size_t i=18;i<sizeof output;++i)CHECK(output[i]==0xcc);
    }
    return 0;
}
static int padding(void)
{
    const uint8_t padded[]={1,2,3,4,5,6,7,8,9, 99,98,97,96,95, 10,11,12,13,14,15,16,17,18};
    const UmiAiTrainingRgb8View source={padded,sizeof padded,3,2,14};uint8_t output[18];
    CHECK(UmiAiTrainingImageFlipRgb8(&source,3,output,sizeof output)==UMI_STATUS_OK);
    CHECK(memcmp(output,both,18)==0);
    /* A one-row view does not need final padding, even with a huge stride. */
    const UmiAiTrainingRgb8View row={grid,9,3,1,SIZE_MAX};
    CHECK(UmiAiTrainingImageFlipRgb8(&row,3,output,sizeof output)==UMI_STATUS_OK);
    CHECK(memcmp(output,horizontal,9)==0);
    return 0;
}
static int crop(void)
{
    const UmiAiTrainingRgb8View source={grid,sizeof grid,3,2,9};UmiAiTrainingRgb8View view={0};
    uint8_t output[12];const uint8_t expected[]={16,17,18,13,14,15,7,8,9,4,5,6};
    CHECK(UmiAiTrainingImageCropRgb8(&source,1,0,2,2,&view)==UMI_STATUS_OK);
    CHECK(view.bytes==15 && view.row_stride_bytes==9);
    CHECK(UmiAiTrainingImageFlipRgb8(&view,3,output,sizeof output)==UMI_STATUS_OK);
    CHECK(memcmp(output,expected,12)==0);
    return 0;
}
static int tensor(void)
{
    const uint8_t pixels[]={255,0,0, 0,255,0, 0,0,255, 255,255,255};
    const UmiAiTrainingRgb8View source={pixels,sizeof pixels,2,2,6};uint8_t flipped[12];float tensor[12];
    const float expected[]={0,1,1,0, 1,0,1,0, 0,0,1,1};
    CHECK(UmiAiTrainingImageFlipRgb8(&source,UMI_AI_TRAINING_FLIP_HORIZONTAL,flipped,sizeof flipped)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImagePackRgb8(flipped,sizeof flipped,2,2,6,UMI_AI_TRAINING_IMAGE_CHW,
        UMI_AI_TRAINING_IMAGE_UNIT,tensor,12)==UMI_STATUS_OK);
    CHECK(memcmp(tensor,expected,sizeof expected)==0);
    return 0;
}
static int refusal(void)
{
    UmiAiTrainingRgb8View source={grid,sizeof grid,3,2,9};uint8_t output[20],before[20];
    memset(output,0xcc,sizeof output);memcpy(before,output,sizeof output);
    CHECK(UmiAiTrainingImageFlipRgb8(NULL,0,output,sizeof output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageFlipRgb8(&source,0,NULL,sizeof output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageFlipRgb8(&source,4,output,sizeof output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageFlipRgb8(&source,~0U,output,sizeof output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageFlipRgb8(&source,0,output,17)==UMI_STATUS_CAPACITY_EXCEEDED);
    source.bytes=17;
    CHECK(UmiAiTrainingImageFlipRgb8(&source,0,output,sizeof output)==UMI_STATUS_CAPACITY_EXCEEDED);
    source.pixels=NULL;
    CHECK(UmiAiTrainingImageFlipRgb8(&source,0,output,sizeof output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(output,before,sizeof output)==0);
    return 0;
}
static int metadata(void)
{
    uint8_t output[20],before[20];memset(output,0xcc,sizeof output);memcpy(before,output,sizeof output);
    const UmiAiTrainingRgb8View cases[]={
        {grid,sizeof grid,0,2,9},{grid,sizeof grid,3,0,9},{grid,sizeof grid,3,2,8},
        {grid,SIZE_MAX,SIZE_MAX,1,SIZE_MAX},{grid,SIZE_MAX,1,SIZE_MAX,3},{grid,SIZE_MAX,1,2,SIZE_MAX}
    };
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i) {
        const UmiStatus expected=i<3?UMI_STATUS_INVALID_ARGUMENT:UMI_STATUS_CAPACITY_EXCEEDED;
        CHECK(UmiAiTrainingImageFlipRgb8(&cases[i],0,output,sizeof output)==expected);
        CHECK(memcmp(output,before,sizeof output)==0);
    }
    return 0;
}
static int limits(void)
{
    static uint8_t input[UMI_AI_TRAINING_IMAGE_MAX_VALUES],output[UMI_AI_TRAINING_IMAGE_MAX_VALUES+3];
    for(size_t i=0;i<sizeof input;++i)input[i]=(uint8_t)(i%251U);
    const size_t width=UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U;
    UmiAiTrainingRgb8View source={input,sizeof input,width,1,width*3};
    memset(output,0xcc,sizeof output);
    CHECK(UmiAiTrainingImageFlipRgb8(&source,1,output,sizeof output)==UMI_STATUS_OK);
    CHECK(memcmp(output,input+(width-1U)*3U,3)==0 && memcmp(output+(width-1U)*3U,input,3)==0);
    CHECK(output[width*3U]==0xcc);
    source.width=width+1U;source.row_stride_bytes=source.width*3U;
    const uint8_t first=output[0],last=output[width*3U-1U];
    CHECK(UmiAiTrainingImageFlipRgb8(&source,1,output,sizeof output)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(output[0]==first && output[width*3U-1U]==last && output[width*3U]==0xcc);
    return 0;
}
static int involution(void)
{
    uint8_t input[13*11*3],flipped[sizeof input],restored[sizeof input];
    for(size_t i=0;i<sizeof input;++i)input[i]=(uint8_t)(i%251U);
    for(size_t width=1;width<=13;++width)for(size_t height=1;height<=11;++height) {
        const size_t bytes=width*height*3U;
        const UmiAiTrainingRgb8View source={input,bytes,width,height,width*3U};
        const UmiAiTrainingRgb8View intermediate={flipped,bytes,width,height,width*3U};
        for(unsigned flags=0;flags<4;++flags) {
            CHECK(UmiAiTrainingImageFlipRgb8(&source,flags,flipped,sizeof flipped)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingImageFlipRgb8(&intermediate,flags,restored,sizeof restored)==UMI_STATUS_OK);
            CHECK(memcmp(input,restored,bytes)==0);
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
    if(strcmp(argv[1],"tensor")==0)return tensor();
    if(strcmp(argv[1],"refusal")==0)return refusal();
    if(strcmp(argv[1],"metadata")==0)return metadata();
    if(strcmp(argv[1],"limits")==0)return limits();
    if(strcmp(argv[1],"involution")==0)return involution();
    return 2;
}
