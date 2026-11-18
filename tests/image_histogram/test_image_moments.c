/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/image_histogram/test_image_moments.c
 *
 * PURPOSE:
 *   Check exact channel moments, independent pixel references and atomic malformed-input refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_moments.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}}while(0)
static int close_value(double actual,double expected){return fabs(actual-expected)<1e-9;}
static int compare_reference(const UmiAiTrainingRgb8View *view)
{
    UmiAiTrainingRgb8Moments result;
    CHECK(UmiAiTrainingImageMeasureRgb8(view,&result)==UMI_STATUS_OK);
    const size_t n=view->width*view->height;
    CHECK(result.pixel_count==n);
    /* Independent two-pass pixel oracle: no histogram or integer-square formula. */
    for(size_t c=0U;c<3U;++c){
        double sum=0.0;uint8_t minimum=255U,maximum=0U;
        for(size_t y=0U;y<view->height;++y)for(size_t x=0U;x<view->width;++x){
            uint8_t value=view->pixels[y*view->row_stride_bytes+x*3U+c];
            sum+=(double)value;if(value<minimum)minimum=value;if(value>maximum)maximum=value;
        }
        const double mean=sum/(double)n;double centered=0.0;
        for(size_t y=0U;y<view->height;++y)for(size_t x=0U;x<view->width;++x){
            const double delta=(double)view->pixels[y*view->row_stride_bytes+x*3U+c]-mean;
            centered+=delta*delta;
        }
        CHECK(close_value(result.mean[c],mean));
        CHECK(close_value(result.population_variance[c],centered/(double)n));
        CHECK(result.minimum[c]==minimum && result.maximum[c]==maximum);
    }
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);
    UmiAiTrainingRgb8Moments result,before;memset(&result,0x5a,sizeof(result));before=result;
    UmiAiTrainingRgb8Histogram histogram={0};
    if(!strcmp(argv[1],"golden")){
        const uint8_t pixels[]={0U,10U,255U,255U,30U,255U};
        const UmiAiTrainingRgb8View view={pixels,sizeof(pixels),2U,1U,6U};
        CHECK(UmiAiTrainingImageMeasureRgb8(&view,&result)==UMI_STATUS_OK);
        CHECK(result.pixel_count==2U && result.mean[0]==127.5 && result.population_variance[0]==16256.25);
        CHECK(result.mean[1]==20.0 && result.population_variance[1]==100.0);
        CHECK(result.mean[2]==255.0 && result.population_variance[2]==0.0);
        CHECK(result.minimum[0]==0U && result.maximum[0]==255U);
        return compare_reference(&view);
    }else if(!strcmp(argv[1],"constant")){
        for(size_t value=0U;value<256U;++value){
            memset(&histogram,0,sizeof(histogram));histogram.pixel_count=7U;
            for(size_t c=0U;c<3U;++c)histogram.bins[c][value]=7U;
            CHECK(UmiAiTrainingImageHistogramMoments(&histogram,&result)==UMI_STATUS_OK);
            for(size_t c=0U;c<3U;++c)CHECK(result.mean[c]==(double)value && result.population_variance[c]==0.0 && result.minimum[c]==value && result.maximum[c]==value);
        }return 0;
    }else if(!strcmp(argv[1],"single")){
        const uint8_t pixels[]={3U,127U,251U};const UmiAiTrainingRgb8View view={pixels,sizeof(pixels),1U,1U,3U};
        return compare_reference(&view);
    }else if(!strcmp(argv[1],"padded-reference")){
        uint8_t pixels[9U*56U];memset(pixels,255,sizeof(pixels));
        for(size_t y=0U;y<9U;++y)for(size_t x=0U;x<17U*3U;++x)pixels[y*56U+x]=(uint8_t)((y*97U+x*73U+x/11U)%256U);
        const UmiAiTrainingRgb8View view={pixels,8U*56U+51U,17U,9U,56U};
        return compare_reference(&view);
    }else if(!strcmp(argv[1],"crop")){
        const uint8_t pixels[]={255,255,255,10,20,30,20,40,60,255,255,255};
        const UmiAiTrainingRgb8View full={pixels,sizeof(pixels),4U,1U,12U};UmiAiTrainingRgb8View crop;
        CHECK(UmiAiTrainingImageCropRgb8(&full,1U,0U,2U,1U,&crop)==UMI_STATUS_OK);
        CHECK(UmiAiTrainingImageMeasureRgb8(&crop,&result)==UMI_STATUS_OK);
        CHECK(result.pixel_count==2U && result.mean[0]==15.0 && result.population_variance[2]==225.0);
        return compare_reference(&crop);
    }else if(!strcmp(argv[1],"maximum-count")){
        const size_t n=UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U;histogram.pixel_count=n;
        for(size_t c=0U;c<3U;++c){histogram.bins[c][0]=(uint32_t)(n-1U);histogram.bins[c][255]=1U;}
        CHECK(UmiAiTrainingImageHistogramMoments(&histogram,&result)==UMI_STATUS_OK);
        CHECK(close_value(result.mean[0],255.0/(double)n));
        CHECK(close_value(result.population_variance[0],65025.0*(double)(n-1U)/((double)n*(double)n)));
        for(size_t c=0U;c<3U;++c){histogram.bins[c][0]=0U;histogram.bins[c][255]=(uint32_t)n;}
        CHECK(UmiAiTrainingImageHistogramMoments(&histogram,&result)==UMI_STATUS_OK && result.population_variance[2]==0.0);
        return 0;
    }else if(!strcmp(argv[1],"zero-count")){
        CHECK(UmiAiTrainingImageHistogramMoments(&histogram,&result)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(!strcmp(argv[1],"over-count")){
        histogram.pixel_count=UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U+1U;
        CHECK(UmiAiTrainingImageHistogramMoments(&histogram,&result)==UMI_STATUS_CAPACITY_EXCEEDED);
        histogram.pixel_count=SIZE_MAX;
        CHECK(UmiAiTrainingImageHistogramMoments(&histogram,&result)==UMI_STATUS_CAPACITY_EXCEEDED);
    }else if(!strcmp(argv[1],"late-invalid-bins")){
        histogram.pixel_count=2U;histogram.bins[0][1]=2U;histogram.bins[1][2]=2U;histogram.bins[2][3]=1U;
        CHECK(UmiAiTrainingImageHistogramMoments(&histogram,&result)==UMI_STATUS_INVALID_ARGUMENT);
        histogram.bins[2][3]=2U;histogram.bins[2][255]=1U;
        CHECK(UmiAiTrainingImageHistogramMoments(&histogram,&result)==UMI_STATUS_INVALID_ARGUMENT);
        histogram.bins[2][255]=UINT32_MAX;
        CHECK(UmiAiTrainingImageHistogramMoments(&histogram,&result)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(!strcmp(argv[1],"view-refusal")){
        const uint8_t pixels[]={1,2,3};UmiAiTrainingRgb8View view={pixels,2U,1U,1U,3U};
        CHECK(UmiAiTrainingImageMeasureRgb8(&view,&result)==UMI_STATUS_CAPACITY_EXCEEDED);
        view.bytes=sizeof(pixels);view.row_stride_bytes=2U;
        CHECK(UmiAiTrainingImageMeasureRgb8(&view,&result)!=UMI_STATUS_OK);
        view.row_stride_bytes=3U;view.width=SIZE_MAX;
        CHECK(UmiAiTrainingImageMeasureRgb8(&view,&result)!=UMI_STATUS_OK);
    }else if(!strcmp(argv[1],"arguments")){
        CHECK(UmiAiTrainingImageHistogramMoments(NULL,&result)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingImageHistogramMoments(&histogram,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingImageMeasureRgb8(NULL,&result)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingImageMeasureRgb8(NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else return 2;
    CHECK(memcmp(&result,&before,sizeof(result))==0);
    return 0;
}
