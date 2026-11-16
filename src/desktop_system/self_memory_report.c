/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/self_memory_report.c
 *
 * PURPOSE:
 *   Keep memory evidence serialization and capture in Framework while application clients supply only stage and output policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/desktop_system/self_memory_report.h"
#include <inttypes.h>
#include <string.h>
static int memory_stage_valid(const char *stage)
{
    if(stage==NULL) return 0;
    for(size_t i=0U;i<=UMI_SELF_MEMORY_STAGE_MAX_BYTES;++i) {
        unsigned char c=(unsigned char)stage[i];
        if(c==0U) return i!=0U;
        if(!((c>='a' && c<='z') || (c>='A' && c<='Z') ||
             (c>='0' && c<='9') || c=='.' || c=='_' || c=='-')) return 0;
    }
    return 0;
}
static int memory_metric_text(char out[24],unsigned known,unsigned bit,uint64_t value)
{
    int count=(known & bit)!=0U ? snprintf(out,24U,"\"%" PRIu64 "\"",value)
                              : snprintf(out,24U,"null");
    return count>=0 && count<24;
}
UmiStatus UmiDesktopSelfMemorySampleJson(const UmiSelfMemorySample *sample,
    const char *stage,char *buffer,size_t capacity,size_t *written)
{
    if(buffer==NULL || written==NULL || !memory_stage_valid(stage)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSelfMemoryDifference identity;
    /* Reuse the canonical known-bit and identity contract instead of creating
     * a second set of validation rules in a serializer or application. */
    UmiStatus status=UmiDesktopSelfMemoryCompare(sample,sample,&identity);
    if(status!=UMI_STATUS_OK) return status;
    char resident[24],peak[24],committed[24];
    if(!memory_metric_text(resident,sample->known_metrics,UMI_SELF_MEMORY_RESIDENT,sample->resident_bytes) ||
       !memory_metric_text(peak,sample->known_metrics,UMI_SELF_MEMORY_PEAK_RESIDENT,sample->peak_resident_bytes) ||
       !memory_metric_text(committed,sample->known_metrics,UMI_SELF_MEMORY_PRIVATE_COMMITTED,sample->private_committed_bytes))
        return UMI_STATUS_INTERNAL_ERROR;
    char candidate[UMI_SELF_MEMORY_REPORT_CAPACITY];
    int count=snprintf(candidate,sizeof(candidate),
        "{\"version\":1,\"scope\":\"self\",\"stage\":\"%s\",\"provider\":\"%s\","
        "\"pid\":\"%" PRIu64 "\",\"creation_ticks\":\"%" PRIu64 "\","
        "\"captured_milliseconds\":\"%" PRIu64 "\",\"known_metrics\":%u,"
        "\"resident_bytes\":%s,\"peak_resident_bytes\":%s,\"private_committed_bytes\":%s}\n",
        stage,sample->provider==UMI_SELF_MEMORY_WINDOWS?"windows":"linux",
        sample->pid,sample->creation_ticks,sample->captured_milliseconds,sample->known_metrics,
        resident,peak,committed);
    if(count<0 || (size_t)count>=sizeof(candidate)) return UMI_STATUS_INTERNAL_ERROR;
    if(capacity<=(size_t)count) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(buffer,candidate,(size_t)count+1U);*written=(size_t)count;return UMI_STATUS_OK;
}
UmiStatus UmiDesktopSelfMemoryReport(const char *stage,FILE *stream)
{
    if(stream==NULL || !memory_stage_valid(stage)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSelfMemorySample sample;
    UmiStatus status=UmiDesktopSelfMemoryCapture(&sample);
    if(status!=UMI_STATUS_OK) return status;
    char json[UMI_SELF_MEMORY_REPORT_CAPACITY];size_t written=0U;
    status=UmiDesktopSelfMemorySampleJson(&sample,stage,json,sizeof(json),&written);
    if(status!=UMI_STATUS_OK) return status;
    if(fwrite(json,1U,written,stream)!=written || fflush(stream)!=0 || ferror(stream))
        return UMI_STATUS_IO_ERROR;
    return UMI_STATUS_OK;
}
