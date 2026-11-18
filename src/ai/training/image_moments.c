/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/image_moments.c
 *
 * PURPOSE:
 *   Derive stable channel statistics from admitted exact frequencies before atomic publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_moments.h"

/* With N<=65535 and x<=255, N*sum(x*x) and sum(x)^2 stay below
 * 2^48. Revisit this proof before increasing the shared image profile. */
_Static_assert(UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U<=UINT16_MAX,
    "Review exact moment arithmetic before enlarging the image profile");

UmiStatus UmiAiTrainingImageHistogramMoments(const UmiAiTrainingRgb8Histogram *histogram,
    UmiAiTrainingRgb8Moments *outMoments)
{
    if(histogram==NULL || outMoments==NULL || histogram->pixel_count==0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(histogram->pixel_count>UMI_AI_TRAINING_IMAGE_MAX_VALUES/3U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const uint64_t n=(uint64_t)histogram->pixel_count;
    UmiAiTrainingRgb8Moments candidate={0};
    candidate.pixel_count=histogram->pixel_count;
    for(size_t channel=0U;channel<3U;++channel){
        uint64_t count=0U,sum=0U,squares=0U;
        uint8_t minimum=UINT8_MAX,maximum=0U;
        for(size_t value=0U;value<UMI_AI_TRAINING_RGB8_BINS;++value){
            const uint64_t frequency=histogram->bins[channel][value];
            /* Reject corrupted metadata before accumulating any weighted value.
             * The remaining-count check also bounds every later multiplication. */
            if(frequency>n-count)return UMI_STATUS_INVALID_ARGUMENT;
            count+=frequency;
            const uint64_t code=(uint64_t)value;
            sum+=code*frequency;
            squares+=code*code*frequency;
            if(frequency!=0U){
                if(value<(size_t)minimum)minimum=(uint8_t)value;
                maximum=(uint8_t)value;
            }
        }
        if(count!=n)return UMI_STATUS_INVALID_ARGUMENT;
        /* Exact integer subtraction avoids cancellation between rounded means.
         * Every admitted channel has N values, so Cauchy's inequality makes
         * the numerator nonnegative, including constant/single-pixel cases. */
        const uint64_t numerator=n*squares-sum*sum;
        candidate.mean[channel]=(double)sum/(double)n;
        candidate.population_variance[channel]=(double)numerator/(double)(n*n);
        candidate.minimum[channel]=minimum;
        candidate.maximum[channel]=maximum;
    }
    *outMoments=candidate;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiTrainingImageMeasureRgb8(const UmiAiTrainingRgb8View *source,
    UmiAiTrainingRgb8Moments *outMoments)
{
    if(outMoments==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingRgb8Histogram histogram;
    const UmiStatus status=UmiAiTrainingImageHistogramRgb8(source,&histogram);
    if(status!=UMI_STATUS_OK)return status;
    return UmiAiTrainingImageHistogramMoments(&histogram,outMoments);
}
