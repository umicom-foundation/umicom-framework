/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_view/test_image_view.c
 *
 * PURPOSE:
 *   Verify borrowed crop spans, lifetime-neutral composition and atomic refusals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_view.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "line %d: %s\n", __LINE__, #c); return 1; } } while (0)
static const uint8_t palette[] = {
    255,0,0, 0,255,0, 0,0,255, 19,20,21,
    0,255,255, 255,0,255, 255,255,0
};
static UmiAiTrainingRgb8View original(void)
{
    const UmiAiTrainingRgb8View view = {palette, sizeof palette, 3U, 2U, 12U};
    return view;
}
static int equal(UmiAiTrainingRgb8View a, UmiAiTrainingRgb8View b)
{
    return a.pixels == b.pixels && a.bytes == b.bytes && a.width == b.width &&
        a.height == b.height && a.row_stride_bytes == b.row_stride_bytes;
}
static int refuses(const UmiAiTrainingRgb8View *source, size_t x, size_t y,
    size_t w, size_t h, UmiStatus wanted)
{
    UmiAiTrainingRgb8View out = {palette + 3U, 17U, 19U, 23U, 29U};
    const UmiAiTrainingRgb8View before = out;
    CHECK(UmiAiTrainingImageCropRgb8(source,x,y,w,h,&out) == wanted);
    CHECK(equal(out,before));
    return 0;
}
static int identity(void)
{
    UmiAiTrainingRgb8View src = original(), out = {0};
    CHECK(UmiAiTrainingImageCropRgb8(&src,0,0,3,2,&out) == UMI_STATUS_OK);
    CHECK(equal(out,src));
    CHECK(out.bytes == 21U); /* Final-row padding is deliberately absent. */
    return 0;
}
static int grid(void)
{
    uint8_t pixels[8U * 26U];
    float values[8U * 8U * 3U + 1U];
    for(size_t h=1U;h<=8U;++h) for(size_t w=1U;w<=8U;++w) {
        const size_t stride=w*3U+2U;
        memset(pixels,239,sizeof pixels);
        for(size_t y=0;y<h;++y) for(size_t x=0;x<w;++x)
            for(size_t c=0;c<3U;++c)
                pixels[y*stride+x*3U+c]=(uint8_t)(y*24U+x*3U+c);
        const UmiAiTrainingRgb8View src={pixels,(h-1U)*stride+w*3U,w,h,stride};
        for(size_t y=0;y<h;++y) for(size_t x=0;x<w;++x)
            for(size_t ch=1U;ch<=h-y;++ch) for(size_t cw=1U;cw<=w-x;++cw) {
                UmiAiTrainingRgb8View out={0};
                CHECK(UmiAiTrainingImageCropRgb8(&src,x,y,cw,ch,&out)==UMI_STATUS_OK);
                CHECK(out.pixels==pixels+y*stride+x*3U);
                CHECK(out.width==cw && out.height==ch && out.row_stride_bytes==stride);
                for(size_t i=0;i<sizeof values/sizeof values[0];++i) values[i]=-9.0f;
                CHECK(UmiAiTrainingImagePackRgb8(out.pixels,out.bytes,cw,ch,stride,
                    UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,values,
                    sizeof values/sizeof values[0])==UMI_STATUS_OK);
                size_t index=0U;
                /* Independent coordinate oracle: padding is never an image value. */
                for(size_t cy=y;cy<y+ch;++cy) for(size_t cx=x;cx<x+cw;++cx)
                    for(size_t c=0;c<3U;++c)
                        CHECK(values[index++]==(float)((double)(cy*24U+cx*3U+c)/255.0));
                CHECK(values[index]==-9.0f);
            }
    }
    return 0;
}
static int padding(void)
{
    UmiAiTrainingRgb8View src=original(),out={0};
    CHECK(UmiAiTrainingImageCropRgb8(&src,1,0,2,2,&out)==UMI_STATUS_OK);
    CHECK(out.pixels==palette+3U && out.bytes==18U && out.row_stride_bytes==12U);
    CHECK(UmiAiTrainingImageCropRgb8(&src,2,1,1,1,&out)==UMI_STATUS_OK);
    CHECK(out.pixels==palette+18U && out.bytes==3U && out.row_stride_bytes==12U);
    return 0;
}
static int packing(void)
{
    const float hwc[]={0,1,0, 0,0,1, 1,0,1, 1,1,0};
    const float chw[]={0,0,1,1, 1,0,0,1, 0,1,1,0};
    UmiAiTrainingRgb8View src=original(),crop={0};
    float values[13];
    CHECK(UmiAiTrainingImageCropRgb8(&src,1,0,2,2,&crop)==UMI_STATUS_OK);
    values[12]=73.0f;
    CHECK(UmiAiTrainingImagePackRgb8(crop.pixels,crop.bytes,2,2,12,
        UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,values,13)==UMI_STATUS_OK);
    CHECK(memcmp(values,hwc,sizeof hwc)==0 && values[12]==73.0f);
    CHECK(UmiAiTrainingImagePackRgb8(crop.pixels,crop.bytes,2,2,12,
        UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_UNIT,values,13)==UMI_STATUS_OK);
    CHECK(memcmp(values,chw,sizeof chw)==0 && values[12]==73.0f);
    CHECK(equal(src,original()));
    return 0;
}
static int inplace(void)
{
    UmiAiTrainingRgb8View src=original();
    CHECK(UmiAiTrainingImageCropRgb8(&src,1,0,2,2,&src)==UMI_STATUS_OK);
    CHECK(src.pixels==palette+3U && src.bytes==18U && src.width==2U);
    const UmiAiTrainingRgb8View before=src;
    CHECK(UmiAiTrainingImageCropRgb8(&src,2,0,1,1,&src)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(equal(src,before));
    return 0;
}
static int nested(void)
{
    UmiAiTrainingRgb8View src=original(),first={0},second={0},direct={0};
    CHECK(UmiAiTrainingImageCropRgb8(&src,1,0,2,2,&first)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImageCropRgb8(&first,1,1,1,1,&second)==UMI_STATUS_OK);
    CHECK(UmiAiTrainingImageCropRgb8(&src,2,1,1,1,&direct)==UMI_STATUS_OK);
    CHECK(equal(second,direct));
    return 0;
}
static int nulls(void)
{
    UmiAiTrainingRgb8View src=original();
    CHECK(refuses(NULL,0,0,1,1,UMI_STATUS_INVALID_ARGUMENT)==0);
    CHECK(UmiAiTrainingImageCropRgb8(&src,0,0,1,1,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    src.pixels=NULL;
    CHECK(refuses(&src,0,0,1,1,UMI_STATUS_INVALID_ARGUMENT)==0);
    return 0;
}
static int rectangles(void)
{
    const UmiAiTrainingRgb8View src=original();
    const size_t cases[][4]={{0,0,0,1},{0,0,1,0},{3,0,1,1},{0,2,1,1},
        {2,0,2,1},{0,1,1,2},{SIZE_MAX,0,1,1},{0,SIZE_MAX,1,1},
        {1,0,SIZE_MAX,1},{0,1,1,SIZE_MAX}};
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)
        CHECK(refuses(&src,cases[i][0],cases[i][1],cases[i][2],cases[i][3],
            UMI_STATUS_INVALID_ARGUMENT)==0);
    return 0;
}
static int metadata(void)
{
    UmiAiTrainingRgb8View src=original();
    src.width=0;
    CHECK(refuses(&src,0,0,1,1,UMI_STATUS_INVALID_ARGUMENT)==0);
    src=original(); src.height=0;
    CHECK(refuses(&src,0,0,1,1,UMI_STATUS_INVALID_ARGUMENT)==0);
    src=original(); src.row_stride_bytes=8;
    CHECK(refuses(&src,0,0,1,1,UMI_STATUS_INVALID_ARGUMENT)==0);
    src=original(); src.row_stride_bytes=SIZE_MAX;
    CHECK(refuses(&src,0,0,1,1,UMI_STATUS_CAPACITY_EXCEEDED)==0);
    return 0;
}
static int short_input(void)
{
    UmiAiTrainingRgb8View src=original();
    for(size_t n=0;n<sizeof palette;++n) {
        src.bytes=n;
        /* A small crop cannot admit a source whose final row is missing. */
        CHECK(refuses(&src,0,0,1,1,UMI_STATUS_CAPACITY_EXCEEDED)==0);
    }
    return 0;
}
static int limits(void)
{
    uint8_t pixels[16383];
    const UmiAiTrainingRgb8View src={pixels,sizeof pixels,5461U,1U,sizeof pixels};
    UmiAiTrainingRgb8View out={0};
    CHECK(UmiAiTrainingImageCropRgb8(&src,5460U,0,1,1,&out)==UMI_STATUS_OK);
    CHECK(out.pixels==pixels+16380U && out.bytes==3U);
    UmiAiTrainingRgb8View bad=src; bad.width=5462U;
    CHECK(refuses(&bad,0,0,1,1,UMI_STATUS_CAPACITY_EXCEEDED)==0);
    bad=src; bad.height=SIZE_MAX;
    CHECK(refuses(&bad,0,0,1,1,UMI_STATUS_CAPACITY_EXCEEDED)==0);
    return 0;
}
static int single_row(void)
{
    const UmiAiTrainingRgb8View src={palette,9U,3U,1U,SIZE_MAX};
    UmiAiTrainingRgb8View out={0};
    CHECK(UmiAiTrainingImageCropRgb8(&src,2,0,1,1,&out)==UMI_STATUS_OK);
    CHECK(out.pixels==palette+6U && out.bytes==3U && out.row_stride_bytes==SIZE_MAX);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    const struct {const char *name; int (*run)(void);} cases[]={
        {"identity",identity},{"grid",grid},{"padding",padding},{"packing",packing},
        {"inplace",inplace},{"nested",nested},{"nulls",nulls},{"rectangles",rectangles},
        {"metadata",metadata},{"short",short_input},{"limits",limits},{"single-row",single_row}};
    for(size_t i=0;i<sizeof cases/sizeof cases[0];++i)
        if(strcmp(argv[1],cases[i].name)==0) return cases[i].run();
    return 2;
}
