/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/self_memory.c
 *
 * PURPOSE:
 *   Publish complete native memory observations and compare only compatible process identities and known metrics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "self_memory_internal.h"
#include <stddef.h>
static int self_memory_valid(const UmiSelfMemorySample *sample)
{
    return sample!=NULL && sample->pid!=0U &&
        (sample->provider==UMI_SELF_MEMORY_WINDOWS || sample->provider==UMI_SELF_MEMORY_LINUX) &&
        sample->known_metrics!=0U && (sample->known_metrics & ~UMI_SELF_MEMORY_ALL)==0U &&
        ((sample->known_metrics & UMI_SELF_MEMORY_RESIDENT)!=0U || sample->resident_bytes==0U) &&
        ((sample->known_metrics & UMI_SELF_MEMORY_PEAK_RESIDENT)!=0U || sample->peak_resident_bytes==0U) &&
        ((sample->known_metrics & UMI_SELF_MEMORY_PRIVATE_COMMITTED)!=0U || sample->private_committed_bytes==0U);
}
UmiStatus UmiDesktopSelfMemoryCapture(UmiSelfMemorySample *out_sample)
{
    if(out_sample==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSelfMemorySample candidate={0};
    UmiStatus status=UmiDesktopSelfMemoryReadNative(&candidate);
    if(status!=UMI_STATUS_OK) return status;
    if(!self_memory_valid(&candidate)) return UMI_STATUS_INVALID_STATE;
    *out_sample=candidate;return UMI_STATUS_OK;
}
static UmiMemoryMovement memory_movement(uint64_t before,uint64_t after)
{
    UmiMemoryMovement result={0};
    if(after>before) { result.bytes=after-before;result.direction=1; }
    else if(after<before) { result.bytes=before-after;result.direction=-1; }
    return result;
}
UmiStatus UmiDesktopSelfMemoryCompare(const UmiSelfMemorySample *before,
    const UmiSelfMemorySample *after,UmiSelfMemoryDifference *out_difference)
{
    if(out_difference==NULL || !self_memory_valid(before) || !self_memory_valid(after))
        return UMI_STATUS_INVALID_ARGUMENT;
    if(before->provider!=after->provider || before->pid!=after->pid ||
       before->creation_ticks!=after->creation_ticks ||
       after->captured_milliseconds<before->captured_milliseconds)
        return UMI_STATUS_INVALID_STATE;
    UmiSelfMemoryDifference candidate={0};
    candidate.common_metrics=before->known_metrics & after->known_metrics;
    if(candidate.common_metrics==0U) return UMI_STATUS_UNAVAILABLE;
    candidate.elapsed_milliseconds=after->captured_milliseconds-before->captured_milliseconds;
    if((candidate.common_metrics & UMI_SELF_MEMORY_RESIDENT)!=0U)
        candidate.resident=memory_movement(before->resident_bytes,after->resident_bytes);
    if((candidate.common_metrics & UMI_SELF_MEMORY_PRIVATE_COMMITTED)!=0U)
        candidate.private_committed=memory_movement(before->private_committed_bytes,after->private_committed_bytes);
    if((candidate.common_metrics & UMI_SELF_MEMORY_PEAK_RESIDENT)!=0U) {
        if(after->peak_resident_bytes<before->peak_resident_bytes) return UMI_STATUS_INVALID_STATE;
        candidate.peak_resident=memory_movement(before->peak_resident_bytes,after->peak_resident_bytes);
    }
    *out_difference=candidate;return UMI_STATUS_OK;
}
