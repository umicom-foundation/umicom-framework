/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_resize/test_bilinear.c
 *
 * PURPOSE:
 *   Verify exact bilinear sampling, independent center geometry and unchanged outputs on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_resize.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); return 1; } } while (0)

static int golden(void)
{
    const uint8_t pixels[] = {0,10,20, 100,110,120, 200,210,220, 240,250,255};
    const UmiAiTrainingRgb8View source = {pixels,sizeof pixels,2,2,6};
    uint8_t out[28]; memset(out,0xa5,sizeof out);
    const uint8_t expected[] = {0,10,20, 50,60,70, 100,110,120,
        100,110,120, 135,145,154, 170,180,188,
        200,210,220, 220,230,238, 240,250,255};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,3,3,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,expected,sizeof expected)==0 && out[27]==0xa5);
    return 0;
}
static int rounding(void)
{
    /* 0,0,0,1 averages to 0.25. Rounding each row first would incorrectly
     * produce 1 after the second axis. The public policy rounds only once. */
    const uint8_t pixels[]={0,0,0, 0,0,0, 0,0,0, 1,2,3};
    const UmiAiTrainingRgb8View source={pixels,sizeof pixels,2,2,6};
    uint8_t out[3]; const uint8_t expected[]={0,1,1};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,expected,3)==0);
    const uint8_t pair[]={0,0,254, 1,255,255};
    const UmiAiTrainingRgb8View row={pair,sizeof pair,2,1,6};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&row,1,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(out[0]==1 && out[1]==128 && out[2]==255);
    return 0;
}
static int identity(void)
{
    uint8_t pixels[105],out[107];
    for(size_t i=0;i<sizeof pixels;++i) pixels[i]=(uint8_t)((i*37U)%256U);
    memset(out,0x73,sizeof out);
    const UmiAiTrainingRgb8View source={pixels,sizeof pixels,7,5,21};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,7,5,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(pixels,out,sizeof pixels)==0 && out[105]==0x73 && out[106]==0x73);
    return 0;
}
static int degenerate(void)
{
    const uint8_t pixel[]={17,91,233}; uint8_t out[75];
    const UmiAiTrainingRgb8View one={pixel,sizeof pixel,1,1,SIZE_MAX};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&one,5,5,out,sizeof out)==UMI_STATUS_OK);
    for(size_t i=0;i<25;++i) CHECK(memcmp(out+i*3,pixel,3)==0);
    const uint8_t row[]={0,0,0, 100,100,100};
    const UmiAiTrainingRgb8View horizontal={row,sizeof row,2,1,SIZE_MAX};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&horizontal,4,3,out,sizeof out)==UMI_STATUS_OK);
    for(size_t y=0;y<3;++y) for(size_t x=0;x<4;++x)
        for(size_t c=0;c<3;++c) CHECK(out[(y*4+x)*3+c]==((uint8_t[]){0,25,75,100})[x]);
    const UmiAiTrainingRgb8View vertical={row,sizeof row,1,2,3};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&vertical,3,4,out,sizeof out)==UMI_STATUS_OK);
    for(size_t y=0;y<4;++y) for(size_t x=0;x<3;++x)
        for(size_t c=0;c<3;++c) CHECK(out[(y*3+x)*3+c]==((uint8_t[]){0,25,75,100})[y]);
    return 0;
}
static int padding(void)
{
    const uint8_t padded[]={0,10,20, 100,110,120, 255,255,255,255,255,
        200,210,220, 240,250,255};
    const UmiAiTrainingRgb8View source={padded,sizeof padded,2,2,11};
    uint8_t out[3];
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(out[0]==135 && out[1]==145 && out[2]==154);
    return 0;
}
static int crop(void)
{
    const uint8_t pixels[]={255,255,255, 0,10,20, 100,110,120, 255,255,255,
        255,255,255, 200,210,220, 240,250,255, 255,255,255};
    const UmiAiTrainingRgb8View source={pixels,sizeof pixels,4,2,12};
    UmiAiTrainingRgb8View cropped; uint8_t out[3];
    CHECK(UmiAiTrainingImageCropRgb8(&source,1,0,2,2,&cropped)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&cropped,1,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(out[0]==135 && out[1]==145 && out[2]==154);
    return 0;
}
static int refusal(void)
{
    const uint8_t pixel[]={1,2,3}; uint8_t out[20],before[20];
    memset(out,0xa3,sizeof out); memcpy(before,out,sizeof out);
    UmiAiTrainingRgb8View source={pixel,sizeof pixel,1,1,3};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(NULL,1,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,NULL,3)==UMI_STATUS_INVALID_ARGUMENT);
    source.pixels=NULL;
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    source.pixels=pixel; source.bytes=2;
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    source.bytes=3;
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,2)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,0,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,0,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(out,before,sizeof out)==0);
    return 0;
}
static int metadata(void)
{
    const uint8_t pixel[]={1,2,3}; uint8_t out[20],before[20];
    memset(out,0xa3,sizeof out); memcpy(before,out,sizeof out);
    UmiAiTrainingRgb8View source={pixel,sizeof pixel,0,1,3};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    source.width=1;source.height=0;
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    source.height=1;source.row_stride_bytes=2;
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_INVALID_ARGUMENT);
    source.height=2;source.row_stride_bytes=SIZE_MAX;
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    source.height=1;source.width=SIZE_MAX;
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    source.width=1;source.row_stride_bytes=3;
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,SIZE_MAX,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,1,SIZE_MAX,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(out,before,sizeof out)==0);
    return 0;
}
static int limits(void)
{
    enum { P=UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U,N=P*3U };
    uint8_t pixels[N],out[N+2];
    for(size_t i=0;i<N;++i)pixels[i]=(uint8_t)(i%251U);
    memset(out,0x43,sizeof out);
    const UmiAiTrainingRgb8View row={pixels,sizeof pixels,P,1,N};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&row,P,1,out,sizeof out)==UMI_STATUS_OK);
    CHECK(memcmp(out,pixels,N)==0 && out[N]==0x43 && out[N+1]==0x43);
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&row,1,P,out,sizeof out)==UMI_STATUS_OK);
    for(size_t i=0;i<P;++i)CHECK(memcmp(out+i*3,pixels+(P/2U)*3,3)==0);
    const UmiAiTrainingRgb8View column={pixels,sizeof pixels,1,P,3};
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&column,P,1,out,sizeof out)==UMI_STATUS_OK);
    for(size_t i=0;i<P;++i)CHECK(memcmp(out+i*3,pixels+(P/2U)*3,3)==0);
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&row,P+1U,1,out,sizeof out)==UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
/* Independent oracle expresses a continuous coordinate and sums a separable
 * tent kernel over every source pixel. It does not copy the integer remainder
 * algorithm. The tiny epsilon only resolves floating representation around
 * exact half-byte ties for these bounded, small test dimensions. */
static double coordinate(size_t d,size_t source,size_t target)
{
    double position=((double)d+0.5)*(double)source/(double)target-0.5;
    if(position<0.0)position=0.0;
    if(position>(double)(source-1U))position=(double)(source-1U);
    return position;
}
static int geometry(void)
{
    uint8_t pixels[147],out[147];
    for(size_t sh=1;sh<=7;++sh)for(size_t sw=1;sw<=7;++sw){
        for(size_t i=0;i<sw*sh*3;++i)pixels[i]=(uint8_t)((i*47U+3U)%256U);
        const UmiAiTrainingRgb8View source={pixels,sw*sh*3,sw,sh,sw*3};
        for(size_t dh=1;dh<=7;++dh)for(size_t dw=1;dw<=7;++dw){
            CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,dw,dh,out,sizeof out)==UMI_STATUS_OK);
            for(size_t y=0;y<dh;++y)for(size_t x=0;x<dw;++x)for(size_t c=0;c<3;++c){
                const double px=coordinate(x,sw,dw),py=coordinate(y,sh,dh);
                double sum=0.0;
                for(size_t sy=0;sy<sh;++sy)for(size_t sx=0;sx<sw;++sx){
                    const double wx=fmax(0.0,1.0-fabs(px-(double)sx));
                    const double wy=fmax(0.0,1.0-fabs(py-(double)sy));
                    sum+=wx*wy*(double)pixels[(sy*sw+sx)*3+c];
                }
                CHECK(out[(y*dw+x)*3+c]==(uint8_t)floor(sum+0.5+1e-9));
            }
        }
    }
    return 0;
}
static int constant(void)
{
    uint8_t pixels[60],out[147];
    for(size_t value=0;value<=255;++value){
        memset(pixels,(int)value,sizeof pixels);
        const UmiAiTrainingRgb8View source={pixels,sizeof pixels,4,5,12};
        CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,7,7,out,sizeof out)==UMI_STATUS_OK);
        for(size_t i=0;i<sizeof out;++i)CHECK(out[i]==value);
    }
    return 0;
}
static int information_loss(void)
{
    const uint8_t stripe[]={0,0,0, 255,255,255, 0,0,0, 255,255,255, 0,0,0, 255,255,255};
    const UmiAiTrainingRgb8View source={stripe,sizeof stripe,6,1,18};
    uint8_t out[6];
    CHECK(UmiAiTrainingImageResizeBilinearRgb8(&source,2,1,out,sizeof out)==UMI_STATUS_OK);
    /* The centers land on exact source pixels 1 and 4. Bilinear has no widened
     * downsampling footprint, so it does not average either three-pixel area. */
    CHECK(out[0]==255 && out[3]==0);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const struct {const char *name;int (*run)(void);} cases[]={
        {"golden",golden},{"rounding",rounding},{"identity",identity},
        {"degenerate",degenerate},{"padding",padding},{"crop",crop},
        {"refusal",refusal},{"metadata",metadata},{"limits",limits},
        {"geometry",geometry},{"constant",constant},{"information-loss",information_loss}};
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)
        if(strcmp(argv[1],cases[i].name)==0)return cases[i].run();
    return 2;
}
