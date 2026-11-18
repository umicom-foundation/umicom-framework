/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_resize/test_image_resize.c
 *
 * PURPOSE:
 *   Verify center mapping with an independent distance oracle, bounded storage and preserved refusal outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_resize.h"
#include "umicom/ai/training/image_histogram.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)

static const uint8_t grid[] = {
    10,11,12, 20,21,22, 30,31,32, 40,41,42,
    50,51,52, 60,61,62, 70,71,72, 80,81,82
};
static int golden(void)
{
    const UmiAiTrainingRgb8View source={grid,sizeof grid,4,2,12};
    uint8_t out[27]; memset(out,0xa5,sizeof out);
    const uint8_t reduced[]={60,61,62,80,81,82};
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,2,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,reduced,sizeof reduced)==0);
    for(size_t i=sizeof reduced;i<sizeof out;++i) CHECK(out[i]==0xa5);
    const uint8_t two[]={1,2,3,9,8,7};
    const UmiAiTrainingRgb8View pair={two,sizeof two,2,1,6};
    const uint8_t expanded[]={1,2,3,1,2,3,9,8,7,9,8,7};
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&pair,4,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,expanded,sizeof expanded)==0);
    /* An exact center tie for 2 -> 1 deliberately selects the second pixel. */
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&pair,1,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,two+3,3)==0);
    const uint8_t three[]={1,2,3,4,5,6,7,8,9};
    const UmiAiTrainingRgb8View odd={three,sizeof three,3,1,9};
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&odd,2,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,three,3)==0 && memcmp(out+3,three+6,3)==0);
    return 0;
}
static int identity(void)
{
    const UmiAiTrainingRgb8View source={grid,sizeof grid,4,2,12};
    uint8_t out[sizeof grid+1]; memset(out,0x93,sizeof out);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,4,2,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,grid,sizeof grid)==0 && out[sizeof grid]==0x93);
    const UmiAiTrainingRgb8View one={grid,3,1,1,SIZE_MAX};
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&one,3,3,out,sizeof grid)==UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
static int padding(void)
{
    uint8_t padded[29], out[6]; memset(padded,0xff,sizeof padded);
    memcpy(padded,grid,12); memcpy(padded+17,grid+12,12);
    const UmiAiTrainingRgb8View source={padded,sizeof padded,4,2,17};
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,2,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,grid+15,3)==0 && memcmp(out+3,grid+21,3)==0);
    const UmiAiTrainingRgb8View row={grid,12,4,1,SIZE_MAX};
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&row,2,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,grid+3,3)==0 && memcmp(out+3,grid+9,3)==0);
    return 0;
}
static int crop(void)
{
    const UmiAiTrainingRgb8View source={grid,sizeof grid,4,2,12};
    UmiAiTrainingRgb8View view;
    uint8_t out[12];
    CHECK(UmiAiTrainingImageCropRgb8(&source,1,0,2,2,&view)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&view,1,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,grid+18,3)==0);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&view,4,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,grid+15,3)==0 && memcmp(out+3,grid+15,3)==0);
    CHECK(memcmp(out+6,grid+18,3)==0 && memcmp(out+9,grid+18,3)==0);
    return 0;
}
static int tensor(void)
{
    const UmiAiTrainingRgb8View source={grid,sizeof grid,4,2,12};
    uint8_t out[6]; float values[6];
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,2,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImagePackRgb8(out,sizeof out,2,1,6,
        UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_UNIT,values,6)==UMI_STATUS_OK);
    const uint8_t expected[]={60,80,61,81,62,82};
    for(size_t i=0;i<6;++i) CHECK(values[i]==(float)((double)expected[i]/255.0));
    return 0;
}
static int refusal(void)
{
    UmiAiTrainingRgb8View source={grid,sizeof grid,4,2,12};
    uint8_t out[30], before[30]; memset(out,0x72,sizeof out);memcpy(before,out,sizeof out);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(NULL,2,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,2,1,NULL,6)==UMI_STATUS_INVALID_ARGUMENT);
    source.pixels=NULL;
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,2,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    source.pixels=grid;source.bytes=sizeof grid-1;
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,2,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    source.bytes=sizeof grid;
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,2,1,out,5)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,0,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,1,0,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(out,before,sizeof out)==0);
    return 0;
}
static int metadata(void)
{
    UmiAiTrainingRgb8View source={grid,sizeof grid,4,2,12};
    uint8_t out[24], before[24];memset(out,0xa9,sizeof out);memcpy(before,out,sizeof out);
    source.width=0;
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    source.width=4;source.row_stride_bytes=11;
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    source.row_stride_bytes=SIZE_MAX;
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    source.row_stride_bytes=12;source.width=SIZE_MAX;
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    source.width=4;
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,SIZE_MAX,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,1,SIZE_MAX,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(out,before,sizeof out)==0);
    return 0;
}
static int limits(void)
{
    enum { P=UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U, N=P*3U };
    uint8_t input[N], out[N+2];
    for(size_t i=0;i<N;++i) input[i]=(uint8_t)(i%251U);
    memset(out,0x31,sizeof out);
    const UmiAiTrainingRgb8View row={input,sizeof input,P,1,N};
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&row,P,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(input,out,N)==0 && out[N]==0x31 && out[N+1]==0x31);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&row,1,P,out,sizeof out)==UMI_STATUS_OK);
    for(size_t i=0;i<P;++i) CHECK(memcmp(out+i*3,input+(P/2)*3,3)==0);
    const UmiAiTrainingRgb8View column={input,sizeof input,1,P,3};
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&column,P,1,out,sizeof out)==UMI_STATUS_OK);
    for(size_t i=0;i<P;++i) CHECK(memcmp(out+i*3,input+(P/2)*3,3)==0);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&row,P+1U,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
/* Independent geometric oracle: compare distances to every source center,
 * retaining the higher coordinate on an exact tie. Do not reuse the production
 * floor formula, so an alignment or rounding change becomes observable. */
static size_t closest(size_t d,size_t source,size_t target)
{
    int64_t best=INT64_MAX;size_t chosen=0;
    for(size_t s=0;s<source;++s){
        const int64_t delta=(int64_t)((2U*s+1U)*target)-(int64_t)((2U*d+1U)*source);
        const int64_t distance=delta<0?-delta:delta;
        if(distance<=best){best=distance;chosen=s;}
    }
    return chosen;
}
static int geometry(void)
{
    uint8_t input[147],out[147];
    for(size_t h=1;h<=7;++h) for(size_t w=1;w<=7;++w){
        for(size_t y=0;y<h;++y)for(size_t x=0;x<w;++x)for(size_t c=0;c<3;++c)
            input[(y*w+x)*3+c]=(uint8_t)(y*23+x*3+c);
        const UmiAiTrainingRgb8View source={input,w*h*3,w,h,w*3};
        for(size_t dh=1;dh<=7;++dh)for(size_t dw=1;dw<=7;++dw){
            CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,dw,dh,out,sizeof out)==UMI_STATUS_OK);
            for(size_t y=0;y<dh;++y)for(size_t x=0;x<dw;++x){
                const size_t sy=closest(y,h,dh),sx=closest(x,w,dw);
                CHECK(memcmp(out+(y*dw+x)*3,input+(sy*w+sx)*3,3)==0);
            }
        }
    }
    return 0;
}
static int information_loss(void)
{
    const uint8_t stripe[]={0,0,0,255,255,255,0,0,0,255,255,255};
    const UmiAiTrainingRgb8View source={stripe,sizeof stripe,4,1,12};
    uint8_t reduced[6],restored[12];
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&source,2,1,reduced,sizeof reduced)==UMI_STATUS_OK);
    const UmiAiTrainingRgb8View small={reduced,sizeof reduced,2,1,6};
    UmiAiTrainingRgb8Histogram before,after;
    CHECK(UmiAiTrainingImageHistogramRgb8(&source,&before)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImageHistogramRgb8(&small,&after)==UMI_STATUS_OK);
    CHECK(before.bins[0][0]==2 && before.bins[0][255]==2);
    CHECK(after.bins[0][0]==0 && after.bins[0][255]==2);
    CHECK(UmiAiTrainingImageResizeNearestRgb8(&small,4,1,restored,sizeof restored)==UMI_STATUS_OK);
    CHECK(memcmp(restored,stripe,sizeof stripe)!=0);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const struct {const char *name;int (*run)(void);} cases[]={
        {"golden",golden},{"identity",identity},{"padding",padding},{"crop",crop},
        {"tensor",tensor},{"refusal",refusal},{"metadata",metadata},{"limits",limits},
        {"geometry",geometry},{"information-loss",information_loss}};
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)
        if(strcmp(argv[1],cases[i].name)==0)return cases[i].run();
    return 2;
}
