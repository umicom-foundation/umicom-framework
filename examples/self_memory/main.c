/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/self_memory/main.c
 *
 * PURPOSE:
 *   Demonstrate native self-process observations before allocation, during use and after release without assuming memory returns immediately.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/desktop_system/self_memory.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
static void metric(const char *label,unsigned int known,unsigned int bit,uint64_t bytes)
{
    if((known & bit)!=0U) printf("  %s: %" PRIu64 " bytes\n",label,bytes);
    else printf("  %s: unknown\n",label);
}
static UmiStatus observe(const char *stage,UmiSelfMemorySample *sample)
{
    UmiStatus status=UmiDesktopSelfMemoryCapture(sample);
    if(status!=UMI_STATUS_OK) { fprintf(stderr,"%s capture failed: %d\n",stage,(int)status);return status; }
    printf("%s | process %" PRIu64 " | time %" PRIu64 " ms\n",stage,sample->pid,sample->captured_milliseconds);
    metric("resident",sample->known_metrics,UMI_SELF_MEMORY_RESIDENT,sample->resident_bytes);
    metric("lifetime peak resident",sample->known_metrics,UMI_SELF_MEMORY_PEAK_RESIDENT,sample->peak_resident_bytes);
    metric("private committed",sample->known_metrics,UMI_SELF_MEMORY_PRIVATE_COMMITTED,sample->private_committed_bytes);
    return UMI_STATUS_OK;
}
int main(void)
{
    UmiSelfMemorySample before,during,after;
    if(observe("Before allocation",&before)!=UMI_STATUS_OK) return 1;
    const size_t size=1024U*1024U;
    unsigned char *owned=malloc(size);
    if(owned==NULL) { fputs("Allocation refused\n",stderr);return 1; }
    /* Touch each byte so the example actually uses its allocation. Volatile
     * accesses keep this teaching workload observable under optimization. */
    volatile unsigned char *touch=owned;
    for(size_t i=0U;i<size;++i) touch[i]=(unsigned char)(i & 255U);
    UmiStatus status=observe("During allocation",&during);
    free(owned);owned=NULL;
    if(status!=UMI_STATUS_OK) return 1;
    if(observe("After release",&after)!=UMI_STATUS_OK) return 1;
    UmiSelfMemoryDifference difference;
    status=UmiDesktopSelfMemoryCompare(&before,&after,&difference);
    if(status!=UMI_STATUS_OK) { fprintf(stderr,"Comparison failed: %d\n",(int)status);return 1; }
    printf("Resident movement after release: direction %d, %" PRIu64 " bytes\n",
        difference.resident.direction,difference.resident.bytes);
    puts("Allocator caching and OS accounting can retain memory; this is not a leak verdict.");
    return fflush(stdout)==0 && !ferror(stdout)?0:1;
}
