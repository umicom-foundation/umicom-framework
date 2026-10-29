/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/debug_reply_queue/main.c
 *
 * PURPOSE:
 *   Demonstrate debugger response correlation, event retention and explicit error inspection through an installed SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int Expect(UmiStatus actual, UmiStatus wanted, const char *step)
{
    if (actual == wanted) return 1;
    fprintf(stderr,"%s: expected %d, received %d\n",step,(int)wanted,(int)actual);
    return 0;
}
static UmiStatus Supply(UmiDebugRuntimeMemoryTransport *memory, const char *json)
{
    char frame[1024];
    int n=snprintf(frame,sizeof(frame),"Content-Length: %zu\r\n\r\n%s",strlen(json),json);
    if (n<0 || (size_t)n>=sizeof(frame)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_debug_runtime_memory_transport_push_read(memory,frame,(size_t)n);
}
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); goto done; } } while (0)

int main(int argc,char **argv)
{
    if (argc>2) return 2;
    const char *mode=argc==1 ? "interleaved" : argv[1];
    int interleaved=strcmp(mode,"interleaved")==0, remote_error=strcmp(mode,"remote-error")==0;
    if (!interleaved && !remote_error && strcmp(mode,"ordered")!=0) return 2;
    UmiDebugRuntimeMemoryTransport *memory=NULL;
    UmiDebugRuntimeTransport transport={0};
    UmiDebugRuntimeAdapter *adapter=NULL;
    UmiDebugRuntimeEnvelope *candidate=calloc(1U,sizeof(*candidate)), *before=malloc(sizeof(*before));
    int result=1;
    REQUIRE(candidate!=NULL && before!=NULL);
    REQUIRE(Expect(umi_debug_runtime_memory_transport_create(&memory,&transport),UMI_STATUS_OK,"create memory transport"));
    REQUIRE(Expect(umi_debug_runtime_adapter_create_with_transport("learning-debugger",&transport,&adapter),UMI_STATUS_OK,"create adapter"));
    REQUIRE(transport.instance==NULL);
    /* The adapter owns the transport. The borrowed memory pointer only supplies
     * local replay bytes; this example never starts a debugger or another process. */
    uint64_t first=0U,second=0U;
    REQUIRE(Expect(umi_debug_runtime_adapter_send_request(adapter,"threads",NULL,"",&first),UMI_STATUS_OK,"send first request"));
    REQUIRE(Expect(umi_debug_runtime_adapter_send_request(adapter,"scopes","{\"frameId\":1}","",&second),UMI_STATUS_OK,"send second request"));
    REQUIRE(first==1U && second==2U);
    const char *first_reply=remote_error ?
        "{\"seq\":5,\"type\":\"response\",\"request_seq\":1,\"command\":\"threads\",\"success\":false,\"message\":\"Replay refusal\",\"body\":{}}" :
        "{\"seq\":5,\"type\":\"response\",\"request_seq\":1,\"command\":\"threads\",\"success\":true,\"body\":{\"threads\":[]}}";
    const char *second_reply=
        "{\"seq\":4,\"type\":\"response\",\"request_seq\":2,\"command\":\"scopes\",\"success\":true,\"body\":{\"scopes\":[]}}";
    if (interleaved) {
        REQUIRE(Expect(Supply(memory,"{\"seq\":1,\"type\":\"event\",\"event\":\"initialized\",\"body\":{}}"),UMI_STATUS_OK,"supply first event"));
        REQUIRE(Expect(Supply(memory,second_reply),UMI_STATUS_OK,"supply second response first"));
        REQUIRE(Expect(Supply(memory,"{\"seq\":3,\"type\":\"event\",\"event\":\"output\",\"body\":{\"output\":\"Replay output\"}}"),UMI_STATUS_OK,"supply second event"));
    }
    REQUIRE(Expect(Supply(memory,first_reply),UMI_STATUS_OK,"supply first response"));
    if (!interleaved) REQUIRE(Expect(Supply(memory,second_reply),UMI_STATUS_OK,"supply second response"));
    /* Ordinary zero-time waiting polls available replay frames without sleeping.
     * Observed waiting has a different zero-time contract: queued replies only. */
    REQUIRE(Expect(umi_debug_runtime_adapter_wait_response(adapter,first,0U,candidate),
        remote_error ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK,"wait for first request"));
    REQUIRE(candidate->kind==UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE &&
        candidate->request_sequence==first && candidate->success==!remote_error &&
        strcmp(candidate->command,"threads")==0);
    puts(remote_error ? "The matching remote refusal was received and inspected." :
        "The first request received its own response.");
    UmiDebugRuntimeAdapterSnapshot state;
    REQUIRE(Expect(umi_debug_runtime_adapter_snapshot(adapter,&state),UMI_STATUS_OK,"inspect queues"));
    REQUIRE(state.queued_responses==(interleaved ? 1U : 0U) && state.queued_events==(interleaved ? 2U : 0U));
    memset(candidate,0,sizeof(*candidate));
    REQUIRE(Expect(umi_debug_runtime_adapter_wait_response(adapter,second,0U,candidate),UMI_STATUS_OK,"wait for second request"));
    REQUIRE(candidate->request_sequence==second && strcmp(candidate->command,"scopes")==0);
    if (interleaved) {
        REQUIRE(Expect(umi_debug_runtime_adapter_next_event(adapter,candidate),UMI_STATUS_OK,"take first event"));
        REQUIRE(strcmp(candidate->event,"initialized")==0);
        REQUIRE(Expect(umi_debug_runtime_adapter_next_event(adapter,candidate),UMI_STATUS_OK,"take second event"));
        REQUIRE(strcmp(candidate->event,"output")==0);
        puts("Both events remained available in their original order.");
    }
    REQUIRE(Expect(umi_debug_runtime_adapter_next_event(adapter,candidate),UMI_STATUS_NOT_FOUND,"confirm events drained"));
    memcpy(before,candidate,sizeof(*before));
    REQUIRE(Expect(umi_debug_runtime_adapter_wait_response(adapter,99U,0U,candidate),UMI_STATUS_TIMEOUT,"poll absent response"));
    REQUIRE(memcmp(before,candidate,sizeof(*before))==0);
    REQUIRE(Expect(umi_debug_runtime_adapter_snapshot(adapter,&state),UMI_STATUS_OK,"inspect drained state"));
    REQUIRE(state.queued_responses==0U && state.queued_events==0U && state.pending_requests==0U);
    puts("An empty poll preserved the caller's previous output.");
    result=0;
done:
    umi_debug_runtime_adapter_destroy(adapter);
    if (transport.instance!=NULL) transport.destroy(transport.instance);
    free(candidate);free(before);
    return result;
}
