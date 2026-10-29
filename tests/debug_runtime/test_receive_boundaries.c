/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_receive_boundaries.c
 *
 * PURPOSE:
 *   Verify debugger transport bounds, retained pending requests and atomic receive output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/adapter.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#expression); exit(1); \
} } while (0)

typedef struct Fixture {
    char input[4096];
    size_t size,offset,fragment;
    unsigned reads,destroys;
    int oversize,empty,fail;
} Fixture;
static UmiStatus Write(void *context,const void *bytes,size_t count)
{ (void)context;(void)bytes;(void)count;return UMI_STATUS_OK; }
static UmiStatus Read(void *context,void *bytes,size_t capacity,uint32_t timeout,size_t *out)
{
    Fixture *fixture=context;
    (void)timeout;++fixture->reads;*out=0U;
    if(fixture->fail) return UMI_STATUS_IO_ERROR;
    if(fixture->oversize) { *out=capacity+1U;return UMI_STATUS_OK; }
    if(fixture->empty) return UMI_STATUS_OK;
    if(fixture->offset==fixture->size) return UMI_STATUS_NOT_FOUND;
    size_t count=fixture->size-fixture->offset;
    if(count>capacity) count=capacity;
    if(fixture->fragment!=0U && count>fixture->fragment) count=fixture->fragment;
    memcpy(bytes,fixture->input+fixture->offset,count);
    fixture->offset+=count;*out=count;
    return UMI_STATUS_OK;
}
static UmiStatus Stop(void *context,uint32_t timeout)
{ (void)context;(void)timeout;return UMI_STATUS_OK; }
static int Running(void *context) { (void)context;return 1; }
static void Destroy(void *context) { ++((Fixture *)context)->destroys; }
static void Push(Fixture *fixture,const char *body)
{
    size_t count=0U;
    CHECK(umi_language_runtime_frame_encode(body,fixture->input+fixture->size,
        sizeof(fixture->input)-fixture->size,&count)==UMI_STATUS_OK);
    fixture->size+=count;
}
static UmiDebugRuntimeAdapterSnapshot Snapshot(UmiDebugRuntimeAdapter *adapter)
{
    UmiDebugRuntimeAdapterSnapshot snapshot;
    CHECK(umi_debug_runtime_adapter_snapshot(adapter,&snapshot)==UMI_STATUS_OK);
    return snapshot;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    const char *mode=argv[1];
    const char *good="{\"seq\":10,\"type\":\"response\",\"request_seq\":1,"
        "\"success\":true,\"command\":\"threads\",\"body\":{\"threads\":[]}}";
    Fixture fixture={0};
    UmiDebugRuntimeTransport transport={&fixture,Write,Read,Stop,Running,Destroy};
    UmiDebugRuntimeAdapter *adapter=NULL;
    CHECK(umi_debug_runtime_adapter_create_with_transport("receive",&transport,&adapter)==UMI_STATUS_OK);
    CHECK(transport.instance==NULL);
    uint64_t sequence=0U;
    CHECK(umi_debug_runtime_adapter_send_request(adapter,"threads",NULL,"",&sequence)==UMI_STATUS_OK);
    CHECK(sequence==1U);
    UmiDebugRuntimeEnvelope *out=malloc(sizeof(*out)),*sentinel=malloc(sizeof(*sentinel));
    CHECK(out!=NULL && sentinel!=NULL);
    memset(sentinel,0x47,sizeof(*sentinel));*out=*sentinel;
    UmiDebugRuntimeAdapterSnapshot before=Snapshot(adapter);
    if(strcmp(mode,"oversize")==0 || strcmp(mode,"read-error")==0 || strcmp(mode,"empty")==0) {
        fixture.oversize=strcmp(mode,"oversize")==0;
        fixture.fail=strcmp(mode,"read-error")==0;
        fixture.empty=strcmp(mode,"empty")==0;
        UmiStatus expected=fixture.oversize?UMI_STATUS_INVALID_STATE:
            fixture.fail?UMI_STATUS_IO_ERROR:UMI_STATUS_NOT_FOUND;
        CHECK(umi_debug_runtime_adapter_receive(adapter,0U,out)==expected);
        CHECK(memcmp(out,sentinel,sizeof(*out))==0);
        UmiDebugRuntimeAdapterSnapshot after=Snapshot(adapter);
        CHECK(after.bytes_received==before.bytes_received);
        CHECK(after.messages_received==before.messages_received && after.revision==before.revision);
        CHECK(after.pending_requests==before.pending_requests);
        fixture.oversize=fixture.fail=fixture.empty=0;
        Push(&fixture,good);
        CHECK(umi_debug_runtime_adapter_receive(adapter,0U,out)==UMI_STATUS_OK);
        CHECK(Snapshot(adapter).pending_requests==0U);
    } else if(strcmp(mode,"malformed-json")==0) {
        Push(&fixture,"{\"seq\":10,\"type\":\"response\",\"request_seq\":\"wrong\"}");
        CHECK(umi_debug_runtime_adapter_receive(adapter,0U,out)==UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(out,sentinel,sizeof(*out))==0);
        CHECK(Snapshot(adapter).pending_requests==1U && Snapshot(adapter).messages_received==0U);
        /* A rejected JSON document consumed one complete frame only. A later
         * valid response still correlates to the original pending request. */
        Push(&fixture,good);
        CHECK(umi_debug_runtime_adapter_receive(adapter,0U,out)==UMI_STATUS_OK);
        CHECK(out->request_sequence==1U && Snapshot(adapter).pending_requests==0U);
    } else if(strcmp(mode,"fragmented")==0) {
        Push(&fixture,good);fixture.fragment=1U;
        for(size_t i=0U;i<fixture.size;++i) {
            UmiStatus result=umi_debug_runtime_adapter_receive(adapter,0U,out);
            if(i+1U<fixture.size) {
                CHECK(result==UMI_STATUS_NOT_FOUND);
                CHECK(memcmp(out,sentinel,sizeof(*out))==0);
                CHECK(Snapshot(adapter).pending_requests==1U);
            } else CHECK(result==UMI_STATUS_OK);
        }
        CHECK(out->request_sequence==1U && Snapshot(adapter).messages_received==1U);
    } else if(strcmp(mode,"buffered-pair")==0 || strcmp(mode,"buffered-error")==0) {
        Push(&fixture,good);
        if(strcmp(mode,"buffered-pair")==0)
            Push(&fixture,"{\"seq\":11,\"type\":\"event\",\"event\":\"initialized\"}");
        else {
            const char *bad="Content-Length: -1\r\n\r\nx";
            CHECK(strlen(bad)<sizeof(fixture.input)-fixture.size);
            memcpy(fixture.input+fixture.size,bad,strlen(bad));fixture.size+=strlen(bad);
        }
        CHECK(umi_debug_runtime_adapter_receive(adapter,0U,out)==UMI_STATUS_OK);
        CHECK(out->request_sequence==1U);
        unsigned reads=fixture.reads;
        if(strcmp(mode,"buffered-pair")==0) {
            CHECK(umi_debug_runtime_adapter_receive(adapter,0U,out)==UMI_STATUS_OK);
            CHECK(out->kind==UMI_DEBUG_RUNTIME_MESSAGE_EVENT && strcmp(out->event,"initialized")==0);
        } else {
            *out=*sentinel;
            CHECK(umi_debug_runtime_adapter_receive(adapter,500U,out)==UMI_STATUS_PARSE_ERROR);
            CHECK(memcmp(out,sentinel,sizeof(*out))==0);
            CHECK(umi_debug_runtime_adapter_receive(adapter,500U,out)==UMI_STATUS_PARSE_ERROR);
        }
        CHECK(fixture.reads==reads);
    } else if(strcmp(mode,"arguments")==0) {
        CHECK(umi_debug_runtime_adapter_receive(NULL,0U,out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_debug_runtime_adapter_receive(adapter,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(fixture.reads==0U && memcmp(out,sentinel,sizeof(*out))==0);
    } else { free(out);free(sentinel);umi_debug_runtime_adapter_destroy(adapter);return 2; }
    free(out);free(sentinel);
    umi_debug_runtime_adapter_destroy(adapter);
    CHECK(fixture.destroys==1U);
    return 0;
}
