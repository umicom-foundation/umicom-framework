/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/video_timing.c
 *
 * PURPOSE:
 *   Admit complete ordered time spans before publishing hold-sampled frame indices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/video_timing.h"

UmiStatus UmiAiTrainingVideoSelectAtOrBefore(const uint64_t *sourceTimes,
    size_t sourceCount,const uint64_t *queryTimes,size_t queryCount,
    size_t *outIndices,size_t indexCapacity)
{
    if(sourceTimes==NULL || queryTimes==NULL || outIndices==NULL ||
        sourceCount==0U || queryCount==0U) return UMI_STATUS_INVALID_ARGUMENT;
    if(sourceCount>UMI_AI_TRAINING_VIDEO_MAX_TIMESTAMPS ||
        queryCount>UMI_AI_TRAINING_VIDEO_MAX_FRAMES || indexCapacity<queryCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Full admission makes a bad late timestamp transactional without copying
     * a tensor or even allocating a second index array. Stable borrows matter. */
    for(size_t i=1U;i<sourceCount;++i)
        if(sourceTimes[i]<=sourceTimes[i-1U]) return UMI_STATUS_INVALID_ARGUMENT;
    for(size_t i=0U;i<queryCount;++i) {
        if(queryTimes[i]<sourceTimes[0] || queryTimes[i]>sourceTimes[sourceCount-1U] ||
            (i>0U && queryTimes[i]<queryTimes[i-1U])) return UMI_STATUS_INVALID_ARGUMENT;
    }
    size_t frame=0U;
    for(size_t i=0U;i<queryCount;++i) {
        /* Comparing absolute unsigned timestamps avoids overflow in deltas,
         * midpoint arithmetic and floating-point conversions near UINT64_MAX. */
        while(frame+1U<sourceCount && sourceTimes[frame+1U]<=queryTimes[i]) ++frame;
        outIndices[i]=frame;
    }
    return UMI_STATUS_OK;
}
