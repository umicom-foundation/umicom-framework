/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/training_data/test_jsonl_stream_allocation.c
 *
 * PURPOSE:
 *   Observe the real streaming line owner for bounded allocation, clearing, failure progress and repeated release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_stream.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while(0)
static void *owned;
static size_t attempts,ownedBytes;
static int refuse,observerError;
static void *LineAllocate(size_t bytes)
{
    ++attempts;
    if(bytes!=UMI_AI_TRAINING_PAIR_LINE_BYTES+1U || owned!=NULL) {
        observerError=1;return NULL;
    }
    if(refuse) return NULL;
    owned=malloc(bytes);
    ownedBytes=bytes;
    if(owned!=NULL) memset(owned,0x7b,bytes);
    return owned;
}
static void LineRelease(void *value)
{
    if(value==NULL || value!=owned) { observerError=1;return; }
    /* Observe while ownership is still valid, never after free. Unused bytes
     * were poisoned too, so partial clearing cannot satisfy this check. */
    for(size_t i=0U;i<ownedBytes;++i)
        if(((unsigned char *)value)[i]!=0U) observerError=1;
    free(value);owned=NULL;ownedBytes=0U;
}
/* Compile the canonical implementation with observation only at its own
 * allocation boundary. Platform/parser dependencies remain the real library;
 * their allocations are outside this particular lifetime assertion. */
#define malloc LineAllocate
#define free LineRelease
#include "../../src/ai/training/jsonl_stream.c"
#undef malloc
#undef free
static UmiStatus Count(void *opaque,const UmiAiTrainingPair *pair,size_t line)
{
    size_t *count=opaque;
    if(line!=*count+1U || strcmp(pair->sourceId,"a")!=0) return UMI_STATUS_INVALID_STATE;
    ++*count;
    return UMI_STATUS_OK;
}
static int Run(int argc,char **argv)
{
    CHECK(argc==3);
    const char *mode=argv[1],*file=argv[2];
    size_t count=0U;
    UmiAiTrainingJsonlReport report;
    refuse=1;
    CHECK(UmiAiTrainingJsonlVisitFile(file,NULL,Count,&count,&report)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(attempts==1U && owned==NULL && count==0U && !observerError);
    CHECK(report.acceptedRows==0U && report.stage==UMI_AI_TRAINING_JSONL_INPUT);
    refuse=0;
    UmiStatus expected=strcmp(mode,"valid")==0?UMI_STATUS_OK:
        strcmp(mode,"late-invalid")==0?UMI_STATUS_PARSE_ERROR:
        strcmp(mode,"missing")==0?UMI_STATUS_NOT_FOUND:UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t rows=strcmp(mode,"valid")==0?2U:strcmp(mode,"late-invalid")==0?1U:0U;
    for(size_t repeat=0U;repeat<128U;++repeat) {
        attempts=0U;count=0U;
        CHECK(UmiAiTrainingJsonlVisitFile(file,NULL,Count,&count,&report)==expected);
        CHECK(attempts==1U && owned==NULL && ownedBytes==0U && !observerError);
        CHECK(count==rows && report.acceptedRows==rows);
    }
    return 0;
}
int main(int argc,char **argv)
{
    int result=2;
    UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,Run,&result);
    return status==UMI_STATUS_OK?result:2;
}
