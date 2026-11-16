/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_system/test_self_memory_report.c
 *
 * PURPOSE:
 *   Verify lossless memory JSON, staged output preservation and capture/write failures without sampling a host process.
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
#include <stdio.h>
#include <string.h>
#include "umicom/desktop_system/self_memory_report.h"
static UmiSelfMemorySample fixture;
static UmiStatus capture_status=UMI_STATUS_OK;
static unsigned captures,writes,flushes;
static int short_write,bad_flush,bad_stream;
static char delivered[UMI_SELF_MEMORY_REPORT_CAPACITY];
static UmiStatus capture(UmiSelfMemorySample *out) { ++captures;*out=fixture;return capture_status; }
static size_t write_bytes(const void *bytes,size_t size,size_t count,FILE *stream)
{
    assert(stream==stderr && size==1U && count<sizeof(delivered));++writes;
    memcpy(delivered,bytes,count);delivered[count]='\0';return short_write?count-1U:count;
}
static int flush_bytes(FILE *stream) { assert(stream==stderr);++flushes;return bad_flush?-1:0; }
static int stream_error(FILE *stream) { assert(stream==stderr);return bad_stream; }
#define UmiDesktopSelfMemoryCapture capture
#define UmiDesktopSelfMemorySampleJson UmiTestMemorySampleJson
#define UmiDesktopSelfMemoryReport UmiTestMemoryReport
#define fwrite write_bytes
#define fflush flush_bytes
#define ferror stream_error
#include "../../src/desktop_system/self_memory_report.c"
#undef ferror
#undef fflush
#undef fwrite
#undef UmiDesktopSelfMemoryReport
#undef UmiDesktopSelfMemorySampleJson
#undef UmiDesktopSelfMemoryCapture
#define MODE(name) (strcmp(argv[1],name)==0)
int main(int argc,char **argv)
{
    assert(argc==2);
    fixture=(UmiSelfMemorySample){UMI_SELF_MEMORY_WINDOWS,UMI_SELF_MEMORY_ALL,17U,23U,100U,40U,80U,60U};
    if(strncmp(argv[1],"report-",7U)==0) {
        const char *stage="loaded";FILE *stream=stderr;UmiStatus wanted=UMI_STATUS_OK;
        if(MODE("report-capture-failure")) capture_status=wanted=UMI_STATUS_IO_ERROR;
        else if(MODE("report-invalid-sample")) { fixture.pid=0U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
        else if(MODE("report-short-write")) { short_write=1;wanted=UMI_STATUS_IO_ERROR; }
        else if(MODE("report-flush-failure")) { bad_flush=1;wanted=UMI_STATUS_IO_ERROR; }
        else if(MODE("report-stream-error")) { bad_stream=1;wanted=UMI_STATUS_IO_ERROR; }
        else if(MODE("report-null-stream")) { stream=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
        else if(MODE("report-invalid-stage")) { stage="bad\nstage";wanted=UMI_STATUS_INVALID_ARGUMENT; }
        else assert(MODE("report-valid"));
        assert(UmiTestMemoryReport(stage,stream)==wanted);
        int rejected=MODE("report-null-stream") || MODE("report-invalid-stage");
        assert(captures==(rejected?0U:1U));
        int no_output=rejected || MODE("report-capture-failure") || MODE("report-invalid-sample");
        assert(writes==(no_output?0U:1U));
        assert(flushes==((no_output || short_write)?0U:1U));
        if(wanted==UMI_STATUS_OK) assert(strstr(delivered,"\"stage\":\"loaded\"")!=NULL);
        return 0;
    }
    char out[UMI_SELF_MEMORY_REPORT_CAPACITY],saved[sizeof(out)],stage_text[65];
    memset(out,'x',sizeof(out));memcpy(saved,out,sizeof(out));
    size_t written=37U,capacity=sizeof(out);size_t *written_out=&written;char *destination=out;
    const UmiSelfMemorySample *sample=&fixture;const char *stage="after-release";
    UmiStatus wanted=UMI_STATUS_OK;
    if(MODE("maximum")) { fixture.pid=fixture.creation_ticks=fixture.captured_milliseconds=UINT64_MAX;fixture.resident_bytes=fixture.peak_resident_bytes=fixture.private_committed_bytes=UINT64_MAX;memset(stage_text,'a',63U);stage_text[63]='\0';stage=stage_text; }
    else if(MODE("unknown")) { fixture.provider=UMI_SELF_MEMORY_LINUX;fixture.known_metrics=UMI_SELF_MEMORY_RESIDENT;fixture.resident_bytes=0U;fixture.peak_resident_bytes=fixture.private_committed_bytes=0U; }
    else if(MODE("small")) { capacity=8U;wanted=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(MODE("zero-capacity")) { capacity=0U;wanted=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(MODE("null-sample")) { sample=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("null-stage")) { stage=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("empty-stage")) { stage="";wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("long-stage")) { memset(stage_text,'a',64U);stage_text[64]='\0';stage=stage_text;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("quote-stage")) { stage="bad\"stage";wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("newline-stage")) { stage="bad\nstage";wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("utf8-stage")) { stage="\xc3\xa9";wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("null-buffer")) { destination=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("null-written")) { written_out=NULL;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("invalid-sample")) { fixture.known_metrics=0U;wanted=UMI_STATUS_INVALID_ARGUMENT; }
    else if(MODE("exact-fit") || MODE("no-terminator")) {
        char temporary[sizeof(out)];size_t count=0U;
        assert(UmiTestMemorySampleJson(sample,stage,temporary,sizeof(temporary),&count)==UMI_STATUS_OK);
        capacity=count+(MODE("exact-fit")?1U:0U);
        if(MODE("no-terminator")) wanted=UMI_STATUS_CAPACITY_EXCEEDED;
    } else assert(MODE("valid"));
    assert(UmiTestMemorySampleJson(sample,stage,destination,capacity,written_out)==wanted);
    assert(captures==0U);
    if(wanted!=UMI_STATUS_OK) { assert(memcmp(out,saved,sizeof(out))==0);assert(written==37U);return 0; }
    assert(written==strlen(out) && out[written-1U]=='\n');
    assert(strstr(out,"\"scope\":\"self\"")!=NULL);
    if(MODE("maximum")) assert(strstr(out,"\"resident_bytes\":\"18446744073709551615\"")!=NULL);
    else if(MODE("unknown")) { assert(strstr(out,"\"resident_bytes\":\"0\"")!=NULL);assert(strstr(out,"\"private_committed_bytes\":null")!=NULL);assert(strstr(out,"\"peak_resident_bytes\":null")!=NULL); }
    else assert(strcmp(out,"{\"version\":1,\"scope\":\"self\",\"stage\":\"after-release\",\"provider\":\"windows\",\"pid\":\"17\",\"creation_ticks\":\"23\",\"captured_milliseconds\":\"100\",\"known_metrics\":7,\"resident_bytes\":\"40\",\"peak_resident_bytes\":\"80\",\"private_committed_bytes\":\"60\"}\n")==0);
    return 0;
}
