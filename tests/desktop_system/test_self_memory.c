/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_system/test_self_memory.c
 *
 * PURPOSE:
 *   Exercise transactional sampling, unavailable metrics and identity-bound unsigned memory differences without OS observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/desktop_system/self_memory.h"
static UmiStatus fixture_status=UMI_STATUS_OK;
static UmiSelfMemorySample fixture;
static unsigned reads;
static UmiStatus fake_read(UmiSelfMemorySample *out) { ++reads;*out=fixture;return fixture_status; }
#define UmiDesktopSelfMemoryReadNative fake_read
#define UmiDesktopSelfMemoryCapture UmiTestSelfMemoryCapture
#define UmiDesktopSelfMemoryCompare UmiTestSelfMemoryCompare
#include "../../src/desktop_system/self_memory.c"
#undef UmiDesktopSelfMemoryCompare
#undef UmiDesktopSelfMemoryCapture
#undef UmiDesktopSelfMemoryReadNative
#define MODE(name) (strcmp(argv[1],name)==0)
int main(int argc,char **argv)
{
    assert(argc==2);
    UmiSelfMemorySample before={UMI_SELF_MEMORY_WINDOWS,UMI_SELF_MEMORY_ALL,17U,23U,100U,40U,80U,60U};
    UmiSelfMemorySample after=before;after.captured_milliseconds=120U;
    if(strncmp(argv[1],"capture",7U)==0) {
        fixture=before;UmiSelfMemorySample out,saved;memset(&out,0x5a,sizeof(out));memcpy(&saved,&out,sizeof(saved));
        UmiStatus wanted=UMI_STATUS_OK;UmiSelfMemorySample *destination=&out;
        if(MODE("capture-error")) wanted=fixture_status=UMI_STATUS_IO_ERROR;
        else if(MODE("capture-invalid")) { fixture.pid=0U;wanted=UMI_STATUS_INVALID_STATE; }
        else if(MODE("capture-null")) { destination=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
        else assert(MODE("capture"));
        assert(UmiTestSelfMemoryCapture(destination)==wanted);
        assert(reads==(MODE("capture-null")?0U:1U));
        if(wanted!=UMI_STATUS_OK) assert(memcmp(&out,&saved,sizeof(out))==0);
        else assert(out.pid==17U && out.known_metrics==UMI_SELF_MEMORY_ALL && out.resident_bytes==40U);
        return 0;
    }
    const UmiSelfMemorySample *first=&before,*second=&after;
    UmiSelfMemoryDifference out,saved;memset(&out,0x5a,sizeof(out));memcpy(&saved,&out,sizeof(saved));
    UmiSelfMemoryDifference *destination=&out;UmiStatus wanted=UMI_STATUS_OK;
    if(MODE("growth")) { after.resident_bytes=55U;after.private_committed_bytes=90U;after.peak_resident_bytes=100U; }
    else if(MODE("shrink")) { after.resident_bytes=20U;after.private_committed_bytes=30U; }
    else if(MODE("unsigned-range")) { before.resident_bytes=0U;after.resident_bytes=UINT64_MAX;before.private_committed_bytes=UINT64_MAX;after.private_committed_bytes=0U; }
    else if(MODE("partial")) { after.known_metrics=UMI_SELF_MEMORY_RESIDENT;after.peak_resident_bytes=0U;after.private_committed_bytes=0U; }
    else if(MODE("no-common")) { before.known_metrics=UMI_SELF_MEMORY_RESIDENT;before.peak_resident_bytes=before.private_committed_bytes=0U;after.known_metrics=UMI_SELF_MEMORY_PRIVATE_COMMITTED;after.resident_bytes=after.peak_resident_bytes=0U;wanted=UMI_STATUS_UNAVAILABLE; }
    else if(MODE("pid-change")) { ++after.pid;wanted=UMI_STATUS_INVALID_STATE; }
    else if(MODE("creation-change")) { ++after.creation_ticks;wanted=UMI_STATUS_INVALID_STATE; }
    else if(MODE("provider-change")) { after.provider=UMI_SELF_MEMORY_LINUX;wanted=UMI_STATUS_INVALID_STATE; }
    else if(MODE("time-reversed")) { after.captured_milliseconds=99U;wanted=UMI_STATUS_INVALID_STATE; }
    else if(MODE("peak-reversed")) { after.peak_resident_bytes=79U;wanted=UMI_STATUS_INVALID_STATE; }
    else if(MODE("null-before")) { first=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("null-after")) { second=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("null-output")) { destination=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("zero-pid")) { after.pid=0U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("unknown-provider")) { after.provider=(UmiSelfMemoryProvider)99;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("unknown-bits")) { after.known_metrics=8U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("empty-metrics")) { after.known_metrics=0U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("unknown-resident-value")) { after.known_metrics &= ~UMI_SELF_MEMORY_RESIDENT;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("unknown-peak-value")) { after.known_metrics &= ~UMI_SELF_MEMORY_PEAK_RESIDENT;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("unknown-private-value")) { after.known_metrics &= ~UMI_SELF_MEMORY_PRIVATE_COMMITTED;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("zero-known-values")) { before.resident_bytes=before.private_committed_bytes=before.peak_resident_bytes=0U;after=before; }
    else assert(MODE("equal"));
    assert(UmiTestSelfMemoryCompare(first,second,destination)==wanted);
    if(wanted!=UMI_STATUS_OK) { assert(memcmp(&out,&saved,sizeof(out))==0);return 0; }
    assert(out.elapsed_milliseconds==(MODE("zero-known-values")?0U:20U));
    if(MODE("growth")) assert(out.resident.direction==1 && out.resident.bytes==15U && out.private_committed.bytes==30U && out.peak_resident.bytes==20U);
    else if(MODE("shrink")) assert(out.resident.direction==-1 && out.resident.bytes==20U && out.private_committed.direction==-1 && out.private_committed.bytes==30U);
    else if(MODE("unsigned-range")) assert(out.resident.direction==1 && out.resident.bytes==UINT64_MAX && out.private_committed.direction==-1 && out.private_committed.bytes==UINT64_MAX);
    else assert(out.resident.direction==0 && out.resident.bytes==0U);
    assert(out.common_metrics==(MODE("partial")?UMI_SELF_MEMORY_RESIDENT:UMI_SELF_MEMORY_ALL));
    return 0;
}
