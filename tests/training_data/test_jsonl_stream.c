/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_stream.c
 *
 * PURPOSE:
 *   Compare real streamed record delivery with memory traversal and independent progress, path and cancellation expectations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_stream.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/native_arguments.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while(0)
#define ROW "{\"source_id\":\"a\",\"prompt\":\"p\",\"completion\":\"c\"}"
typedef struct Seen {
    size_t calls,reject;
    uint64_t hash;
    int wrong;
    UmiCancellationToken *cancel;
} Seen;
static UmiStatus Visit(void *opaque,const UmiAiTrainingPair *pair,size_t line)
{
    Seen *seen=opaque;
    ++seen->calls;
    if(line!=seen->calls || strcmp(pair->sourceId,"a")!=0 || strcmp(pair->completion,"c")!=0)
        seen->wrong=1;
    const char *fields[]={pair->sourceId,pair->prompt,pair->completion};
    for(size_t f=0U;f<3U;++f) {
        for(const unsigned char *p=(const unsigned char *)fields[f];*p!=0U;++p)
            seen->hash=(seen->hash^*p)*UINT64_C(1099511628211);
        seen->hash=(seen->hash^UINT64_C(255))*UINT64_C(1099511628211);
    }
    if(seen->cancel!=NULL) umi_cancellation_token_request(seen->cancel);
    return seen->calls==seen->reject ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}
/* Text-mode fixture generators may expand LF on Windows. Decode a bounded
 * ASCII hex transport through the real binary output owner, then let CMake
 * verify its hash before any semantic check. These files are test fixtures. */
static int HexNibble(unsigned char value)
{
    if(value>='0' && value<='9') return value-'0';
    if(value>='a' && value<='f') return value-'a'+10;
    return -1;
}
static int PrepareFixture(const char *hexPath,const char *outputPath)
{
    unsigned char *hex=NULL;
    size_t length=0U;
    CHECK(UmiInputFileRead(hexPath,1024U*1024U,&hex,&length)==UMI_STATUS_OK);
    int valid=length%2U==0U;
    for(size_t i=0U;i<length && valid;++i) valid=HexNibble(hex[i])>=0;
    if(!valid) { UmiInputFileFree(hex); return 1; }
    UmiOutputFile *file=NULL;
    UmiStatus status=UmiOutputFileCreate(outputPath,&file);
    unsigned char buffer[1024];
    for(size_t i=0U;i<length && status==UMI_STATUS_OK;) {
        size_t count=0U;
        while(i<length && count<sizeof(buffer)) {
            buffer[count++]=(unsigned char)(HexNibble(hex[i])*16+HexNibble(hex[i+1U]));
            i+=2U;
        }
        status=UmiOutputFileWrite(file,buffer,count);
    }
    UmiInputFileFree(hex);
    if(status==UMI_STATUS_OK) status=UmiOutputFileClose(file);
    UmiOutputFileDestroy(file);
    CHECK(status==UMI_STATUS_OK);
    return 0;
}
static int Run(int argc,char **argv)
{
    if(argc==4 && strcmp(argv[1],"prepare")==0) return PrepareFixture(argv[2],argv[3]);
    CHECK(argc==3);
    const char *mode=argv[1],*file=argv[2];
    Seen streamed={0};
    UmiAiTrainingJsonlReport report;
    if(strcmp(mode,"arguments")==0) {
        memset(&report,0x5a,sizeof(report));
        CHECK(UmiAiTrainingJsonlVisitFile(NULL,NULL,Visit,&streamed,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.acceptedRows==0U && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
        CHECK(UmiAiTrainingJsonlVisitFile(file,NULL,NULL,&streamed,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiTrainingJsonlVisitFile(file,NULL,Visit,&streamed,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(streamed.calls==0U);
        return 0;
    }
    if(strncmp(mode,"cancel-",7U)==0) {
        UmiCancellationToken *token=NULL;
        CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        int before=strcmp(mode,"cancel-before")==0;
        int final=strcmp(mode,"cancel-final")==0;
        if(before) umi_cancellation_token_request(token);
        else streamed.cancel=token;
        CHECK(UmiAiTrainingJsonlVisitFile(file,token,Visit,&streamed,&report)==UMI_STATUS_CANCELLED);
        CHECK(streamed.calls==(before?0U:1U));
        CHECK(report.acceptedRows==streamed.calls);
        CHECK(report.consumedBytes==(before?0U:strlen(ROW)+1U));
        CHECK(report.failedLine==((before||final)?0U:2U));
        CHECK(report.stage==((before||final)?UMI_AI_TRAINING_JSONL_INPUT:UMI_AI_TRAINING_JSONL_RECORD));
        umi_cancellation_token_destroy(token);
        return 0;
    }
    if(strcmp(mode,"reject-second")==0) streamed.reject=2U;
    UmiStatus status=UmiAiTrainingJsonlVisitFile(file,NULL,Visit,&streamed,&report);
    CHECK(!streamed.wrong);
    if(strcmp(mode,"missing")==0 || strcmp(mode,"relative")==0 ||
        strcmp(mode,"directory")==0 || strcmp(mode,"oversize")==0) {
        CHECK(status!=UMI_STATUS_OK && streamed.calls==0U);
        CHECK(report.acceptedRows==0U && report.consumedBytes==0U && report.failedLine==0U);
        CHECK(report.stage==UMI_AI_TRAINING_JSONL_INPUT);
        if(strcmp(mode,"missing")==0) CHECK(status==UMI_STATUS_NOT_FOUND);
        if(strcmp(mode,"relative")==0) CHECK(status==UMI_STATUS_INVALID_ARGUMENT);
        if(strcmp(mode,"oversize")==0) CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
        return 0;
    }
    /* The memory reference receives exactly the same bytes. Independent checks
     * below still constrain success, failure and known callback counts. */
    unsigned char *bytes=NULL;
    size_t length=0U;
    CHECK(UmiInputFileRead(file,UMI_AI_TRAINING_JSONL_BYTES,&bytes,&length)==UMI_STATUS_OK);
    Seen memory={.reject=streamed.reject};
    UmiAiTrainingJsonlReport reference;
    UmiStatus expected=UmiAiTrainingJsonlVisit(bytes,length,NULL,Visit,&memory,&reference);
    UmiInputFileFree(bytes);
    CHECK(status==expected && streamed.calls==memory.calls && streamed.hash==memory.hash);
    CHECK(report.acceptedRows==reference.acceptedRows && report.consumedBytes==reference.consumedBytes);
    CHECK(report.failedLine==reference.failedLine && report.stage==reference.stage);
    if(strcmp(mode,"valid")==0 || strcmp(mode,"unicode")==0 || strcmp(mode,"crlf")==0 ||
        strcmp(mode,"last-line")==0 || strcmp(mode,"final-cr")==0) {
        CHECK(status==UMI_STATUS_OK && streamed.calls==2U && report.consumedBytes==length);
    } else if(strncmp(mode,"boundary-",9U)==0) {
        CHECK(status==UMI_STATUS_OK && streamed.calls==(strcmp(mode,"boundary-crlf")==0?2U:3U));
        CHECK(report.consumedBytes==length);
    } else if(strcmp(mode,"line-max")==0 || strcmp(mode,"line-max-crlf")==0 || strcmp(mode,"line-max-cr")==0) {
        CHECK(status==UMI_STATUS_OK && streamed.calls==1U && report.consumedBytes==length);
    } else if(strcmp(mode,"rows-max")==0) {
        CHECK(status==UMI_STATUS_OK && streamed.calls==4096U);
    } else if(strcmp(mode,"rows-over")==0) {
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED && report.acceptedRows==4096U && report.failedLine==4097U);
    } else if(strcmp(mode,"reject-second")==0) {
        CHECK(status==UMI_STATUS_BUSY && streamed.calls==2U && report.acceptedRows==1U);
        CHECK(report.stage==UMI_AI_TRAINING_JSONL_VISITOR && report.failedLine==2U);
    } else if(strcmp(mode,"late-invalid")==0 || strcmp(mode,"blank")==0) {
        CHECK(status==UMI_STATUS_PARSE_ERROR && streamed.calls==1U && report.failedLine==2U);
        CHECK(report.consumedBytes==strlen(ROW)+1U);
    } else if(strcmp(mode,"line-over")==0 || strcmp(mode,"double-cr-over")==0) {
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED && streamed.calls==0U);
    } else if(strcmp(mode,"empty")==0) {
        CHECK(status==UMI_STATUS_INVALID_ARGUMENT && streamed.calls==0U);
    } else {
        CHECK(status!=UMI_STATUS_OK && streamed.calls==0U);
    }
    return 0;
}
int main(int argc,char **argv)
{
    int result=2;
    UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,Run,&result);
    return status==UMI_STATUS_OK?result:2;
}
