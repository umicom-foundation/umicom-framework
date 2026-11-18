/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_tensor/test_image_tensor.c
 *
 * PURPOSE:
 *   Check RGB layout/range semantics, padded storage and transactional refusal against explicit image values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_tensor.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "image tensor check: %d\n", __LINE__); return 1; } } while (0)
static uint8_t largeRgb[UMI_AI_TRAINING_IMAGE_MAX_VALUES];
static float largeOut[UMI_AI_TRAINING_IMAGE_MAX_VALUES + 1U];
static int Near(float actual, double expected)
{
    return fabs((double)actual - expected) < 0.0000001;
}
static int Refused(const uint8_t *rgb, size_t bytes, size_t width, size_t height,
    size_t stride, UmiAiTrainingImageLayout layout, UmiAiTrainingImageRange range,
    size_t capacity, UmiStatus status)
{
    float out[16], before[16];
    memset(out, 0xA7, sizeof out);
    memcpy(before, out, sizeof before);
    return UmiAiTrainingImagePackRgb8(rgb, bytes, width, height, stride,
        layout, range, out, capacity) == status && memcmp(out, before, sizeof out) == 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const uint8_t rgb[] = {255,0,0, 0,255,0, 0,0,255, 255,255,255};
    float out[16];
    for (size_t i=0; i<16; ++i) out[i]=19.0F;
    if (!strcmp(mode, "hwc")) {
        const float expected[] = {1,0,0, 0,1,0, 0,0,1, 1,1,1};
        CHECK(UmiAiTrainingImagePackRgb8(rgb, sizeof rgb, 2, 2, 6,
            UMI_AI_TRAINING_IMAGE_HWC, UMI_AI_TRAINING_IMAGE_UNIT, out, 16) == UMI_STATUS_OK);
        CHECK(memcmp(out, expected, sizeof expected) == 0);
        for (size_t i=12; i<16; ++i) CHECK(out[i] == 19.0F);
    } else if (!strcmp(mode, "chw")) {
        const float expected[] = {1,0,0,1, 0,1,0,1, 0,0,1,1};
        CHECK(UmiAiTrainingImagePackRgb8(rgb, sizeof rgb, 2, 2, 6,
            UMI_AI_TRAINING_IMAGE_CHW, UMI_AI_TRAINING_IMAGE_UNIT, out, 16) == UMI_STATUS_OK);
        CHECK(memcmp(out, expected, sizeof expected) == 0);
        CHECK(out[12] == 19.0F);
    } else if (!strcmp(mode, "signed")) {
        const uint8_t samples[] = {0,127,128, 255,64,192};
        CHECK(UmiAiTrainingImagePackRgb8(samples, sizeof samples, 2, 1, 6,
            UMI_AI_TRAINING_IMAGE_HWC, UMI_AI_TRAINING_IMAGE_SIGNED_UNIT, out, 16) == UMI_STATUS_OK);
        CHECK(out[0] == -1.0F && out[3] == 1.0F);
        CHECK(Near(out[1], -1.0/255.0) && Near(out[2], 1.0/255.0));
        CHECK(Near(out[4], -127.0/255.0) && Near(out[5], 129.0/255.0));
    } else if (!strcmp(mode, "padding")) {
        /* Only the gap between rows exists; final-row padding is not required. */
        uint8_t padded[] = {255,0,0, 0,255,0, 0xA5,0x5A, 0,0,255, 255,255,255};
        uint8_t before[sizeof padded];
        memcpy(before, padded, sizeof before);
        const float expected[] = {1,0,0,1, 0,1,0,1, 0,0,1,1};
        CHECK(UmiAiTrainingImagePackRgb8(padded, sizeof padded, 2, 2, 8,
            UMI_AI_TRAINING_IMAGE_CHW, UMI_AI_TRAINING_IMAGE_UNIT, out, 16) == UMI_STATUS_OK);
        CHECK(memcmp(out, expected, sizeof expected) == 0);
        CHECK(memcmp(padded, before, sizeof padded) == 0 && out[12] == 19.0F);
    } else if (!strcmp(mode, "single-row")) {
        /* A one-row image never advances to a second row, even with this stride. */
        CHECK(UmiAiTrainingImagePackRgb8(rgb, 3, 1, 1, SIZE_MAX,
            UMI_AI_TRAINING_IMAGE_HWC, UMI_AI_TRAINING_IMAGE_UNIT, out, 3) == UMI_STATUS_OK);
        CHECK(out[0] == 1.0F && out[1] == 0.0F && out[2] == 0.0F && out[3] == 19.0F);
    } else if (!strcmp(mode, "reference")) {
        uint8_t padded[600];
        float hwc[600], chw[600];
        for (size_t width=1; width<=13; ++width) for (size_t height=1; height<=11; ++height) {
            const size_t stride=width*3U+5U, bytes=(height-1U)*stride+width*3U;
            for (size_t i=0; i<bytes; ++i) padded[i]=(uint8_t)((i*29U+width*7U+height)%256U);
            CHECK(UmiAiTrainingImagePackRgb8(padded,bytes,width,height,stride,
                UMI_AI_TRAINING_IMAGE_HWC,UMI_AI_TRAINING_IMAGE_UNIT,hwc,600)==UMI_STATUS_OK);
            CHECK(UmiAiTrainingImagePackRgb8(padded,bytes,width,height,stride,
                UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_SIGNED_UNIT,chw,600)==UMI_STATUS_OK);
            size_t tensorIndex=0;
            for (size_t y=0;y<height;++y) {
                for(size_t byte=0;byte<width*3U;++byte) {
                    CHECK(Near(hwc[tensorIndex],(double)padded[y*stride+byte]/255.0));
                    ++tensorIndex;
                }
            }
            for (size_t channel=0;channel<3;++channel) for(size_t pixel=0;pixel<width*height;++pixel)
                CHECK(Near(chw[channel*width*height+pixel],2.0*(double)hwc[3U*pixel+channel]-1.0));
        }
    } else if (!strcmp(mode, "maximum")) {
        const size_t width=UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U, count=width*3U;
        memset(largeRgb,255,sizeof largeRgb);
        for(size_t i=0;i<UMI_AI_TRAINING_IMAGE_MAX_VALUES+1U;++i)largeOut[i]=19.0F;
        CHECK(UmiAiTrainingImagePackRgb8(largeRgb,count,width,1,count,
            UMI_AI_TRAINING_IMAGE_CHW,UMI_AI_TRAINING_IMAGE_UNIT,largeOut,count)==UMI_STATUS_OK);
        for(size_t i=0;i<count;++i)CHECK(largeOut[i]==1.0F);
        CHECK(largeOut[count]==19.0F);
    } else if (!strcmp(mode, "shape-refusal")) {
        CHECK(Refused(rgb,sizeof rgb,0,2,6,0,0,16,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,sizeof rgb,2,0,6,0,0,16,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,sizeof rgb,SIZE_MAX,1,SIZE_MAX,0,0,16,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,sizeof rgb,1,SIZE_MAX,3,0,0,16,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,sizeof rgb,UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U+1U,1,SIZE_MAX,0,0,16,UMI_STATUS_CAPACITY_EXCEEDED));
    } else if (!strcmp(mode, "span-refusal")) {
        CHECK(Refused(rgb,sizeof rgb,2,2,5,0,0,16,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,11,2,2,6,0,0,16,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,sizeof rgb,2,2,6,0,0,11,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,0,1,1,3,0,0,16,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,sizeof rgb,1,2,SIZE_MAX,0,0,16,UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(Refused(rgb,sizeof rgb,1,3,SIZE_MAX/2U,0,0,16,UMI_STATUS_CAPACITY_EXCEEDED));
    } else if (!strcmp(mode, "arguments")) {
        CHECK(Refused(NULL,sizeof rgb,2,2,6,0,0,16,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,sizeof rgb,2,2,6,(UmiAiTrainingImageLayout)2,0,16,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(Refused(rgb,sizeof rgb,2,2,6,0,(UmiAiTrainingImageRange)-1,16,UMI_STATUS_INVALID_ARGUMENT));
        CHECK(UmiAiTrainingImagePackRgb8(rgb,sizeof rgb,2,2,6,0,0,NULL,16)==UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
