/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_canvas/test_image_canvas.c
 *
 * PURPOSE:
 *   Verify exact canvas geometry, byte preservation, borrowed colors and failure-before-publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/image_canvas.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
static const uint8_t Color[3] = {7U, 8U, 9U};
static const uint8_t Pixels[15] = {
    1U,2U,3U, 4U,5U,6U, 201U,202U,203U,
    10U,11U,12U, 13U,14U,15U
};
static UmiAiTrainingRgb8View Source(void)
{
    return (UmiAiTrainingRgb8View){Pixels,sizeof(Pixels),2U,2U,9U};
}
static int Golden(void)
{
    UmiAiTrainingRgb8View source = Source();
    uint8_t out[40]; memset(out, 0xa5, sizeof(out));
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,4U,3U,1U,1U,Color,out,sizeof(out)) == UMI_STATUS_OK);
    for(size_t y=0U;y<3U;++y) for(size_t x=0U;x<4U;++x) {
        const uint8_t *expected = y>=1U && x>=1U && x<3U ?
            Pixels+(y-1U)*9U+(x-1U)*3U : Color;
        CHECK(memcmp(out+(y*4U+x)*3U,expected,3U)==0);
    }
    for(size_t i=36U;i<sizeof(out);++i) CHECK(out[i]==0xa5U);
    CHECK(Pixels[6]==201U && Pixels[14]==15U);
    return 0;
}
static int Identity(void)
{
    UmiAiTrainingRgb8View source = Source();
    uint8_t out[12];
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,2U,2U,0U,0U,Color,out,sizeof(out)) == UMI_STATUS_OK);
    CHECK(memcmp(out,Pixels,6U)==0 && memcmp(out+6U,Pixels+9U,6U)==0);
    return 0;
}
static int Placements(void)
{
    uint8_t pixels[4U*4U*3U], out[6U*6U*3U];
    for(size_t i=0U;i<sizeof(pixels);++i) pixels[i]=(uint8_t)(20U+i);
    /* Enumerate every fitting origin in several shapes. The oracle classifies
     * each destination coordinate, independently of the implementation's row copies. */
    for(size_t h=1U;h<=4U;++h) for(size_t w=1U;w<=4U;++w)
    for(size_t ch=h;ch<=6U;++ch) for(size_t cw=w;cw<=6U;++cw)
    for(size_t top=0U;top<=ch-h;++top) for(size_t left=0U;left<=cw-w;++left) {
        UmiAiTrainingRgb8View source={pixels,w*h*3U,w,h,w*3U};
        memset(out,0xa5,sizeof(out));
        CHECK(UmiAiTrainingImageCanvasRgb8(&source,cw,ch,left,top,Color,out,sizeof(out))==UMI_STATUS_OK);
        for(size_t y=0U;y<ch;++y) for(size_t x=0U;x<cw;++x) {
            bool inside=x>=left && x-left<w && y>=top && y-top<h;
            const uint8_t *expected=inside?pixels+((y-top)*w+x-left)*3U:Color;
            CHECK(memcmp(out+(y*cw+x)*3U,expected,3U)==0);
        }
        for(size_t i=cw*ch*3U;i<sizeof(out);++i) CHECK(out[i]==0xa5U);
    }
    return 0;
}
static int Refuse(const char *mode)
{
    UmiAiTrainingRgb8View source=Source();
    uint8_t out[40], before[40]; memset(out,0xa5,sizeof(out)); memcpy(before,out,sizeof(out));
    size_t width=4U,height=3U,left=1U,top=1U,capacity=sizeof(out);
    UmiStatus expected=UMI_STATUS_INVALID_ARGUMENT;
    if(strcmp(mode,"capacity")==0) {capacity=35U; expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(mode,"short-source")==0) {source.bytes=14U;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(mode,"stride")==0) source.row_stride_bytes=5U;
    else if(strcmp(mode,"overflow")==0) {width=SIZE_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(mode,"oversized-source")==0) {source.width=SIZE_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(strcmp(mode,"origin")==0) left=SIZE_MAX;
    else if(strcmp(mode,"rectangle")==0) top=2U;
    else if(strcmp(mode,"zero")==0) height=0U;
    else return 2;
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,width,height,left,top,Color,out,capacity)==expected);
    CHECK(memcmp(out,before,sizeof(out))==0);
    return 0;
}
static int Arguments(void)
{
    UmiAiTrainingRgb8View source=Source();
    uint8_t out[36]; memset(out,0xa5,sizeof(out));
    CHECK(UmiAiTrainingImageCanvasRgb8(NULL,4U,3U,1U,1U,Color,out,sizeof(out))==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,4U,3U,1U,1U,NULL,out,sizeof(out))==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,4U,3U,1U,1U,Color,NULL,sizeof(out))==UMI_STATUS_INVALID_ARGUMENT);
    source.pixels=NULL;
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,4U,3U,1U,1U,Color,out,sizeof(out))==UMI_STATUS_INVALID_ARGUMENT);
    for(size_t i=0U;i<sizeof(out);++i) CHECK(out[i]==0xa5U);
    return 0;
}
static int BorrowedColor(void)
{
    UmiAiTrainingRgb8View source=Source();
    uint8_t out[36]; memset(out,0xa5,sizeof(out));
    memcpy(out+12U,Color,3U);
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,4U,3U,1U,1U,out+12U,out,sizeof(out))==UMI_STATUS_OK);
    CHECK(memcmp(out,Color,3U)==0 && memcmp(out+33U,Color,3U)==0);
    return 0;
}
static int Crop(void)
{
    UmiAiTrainingRgb8View source=Source(),crop;
    uint8_t out[18];
    CHECK(UmiAiTrainingImageCropRgb8(&source,1U,0U,1U,2U,&crop)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImageCanvasRgb8(&crop,3U,2U,1U,0U,Color,out,sizeof(out))==UMI_STATUS_OK);
    CHECK(memcmp(out+3U,Pixels+3U,3U)==0 && memcmp(out+12U,Pixels+12U,3U)==0);
    CHECK(memcmp(out,Color,3U)==0 && memcmp(out+15U,Color,3U)==0);
    return 0;
}
static int Tensor(void)
{
    const uint8_t pixels[6]={255U,255U,255U,255U,255U,255U},black[3]={0};
    UmiAiTrainingRgb8View source={pixels,sizeof(pixels),2U,1U,6U};
    uint8_t out[36]; float values[36];
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,4U,3U,1U,1U,black,out,sizeof(out))==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImagePackRgb8(out,sizeof(out),4U,3U,12U,
        UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_SIGNED_UNIT,values,36U)==UMI_STATUS_OK);
    for(size_t c=0U;c<3U;++c) for(size_t p=0U;p<12U;++p)
        CHECK(values[c*12U+p]==((p==5U||p==6U)?1.0f:-1.0f));
    return 0;
}
static int Limit(void)
{
    const size_t width=UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U;
    uint8_t *pixels=malloc(width*3U),*out=malloc(width*3U);
    CHECK(pixels!=NULL && out!=NULL);
    memset(pixels,33,width*3U);
    UmiAiTrainingRgb8View source={pixels,width*3U,width,1U,width*3U};
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,width,1U,0U,0U,Color,out,width*3U)==UMI_STATUS_OK);
    CHECK(memcmp(pixels,out,width*3U)==0);
    CHECK(UmiAiTrainingImageCanvasRgb8(&source,width+1U,1U,0U,0U,Color,out,width*3U)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(pixels,out,width*3U)==0);
    free(out); free(pixels); return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    if(strcmp(argv[1],"golden")==0) return Golden();
    if(strcmp(argv[1],"identity")==0) return Identity();
    if(strcmp(argv[1],"placements")==0) return Placements();
    if(strcmp(argv[1],"arguments")==0) return Arguments();
    if(strcmp(argv[1],"borrowed-color")==0) return BorrowedColor();
    if(strcmp(argv[1],"crop")==0) return Crop();
    if(strcmp(argv[1],"tensor")==0) return Tensor();
    if(strcmp(argv[1],"limit")==0) return Limit();
    return Refuse(argv[1]);
}
