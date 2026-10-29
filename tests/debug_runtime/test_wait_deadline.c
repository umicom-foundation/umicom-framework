/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_wait_deadline.c
 *
 * PURPOSE:
 *   Check debugger wait deadlines, output ownership and retained messages with a deterministic clock.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/adapter.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t now_ns, clock_step;
static uint64_t Now(const UmiClock *clock)
{
    (void)clock;
    uint64_t result = now_ns;
    now_ns += clock_step;
    return result;
}
static UmiStatus Sleep(UmiClock *clock, uint64_t milliseconds)
{
    (void)clock;
    now_ns += milliseconds * UINT64_C(1000000);
    return UMI_STATUS_OK;
}
static UmiClock TestClock(void) { return (UmiClock){Now, Now, Sleep, NULL}; }
/* Only the wait's clock is replaced. Linked transport, framing, message
 * decoding, pending correlation and bounded queues are the production services. */
#define umi_clock_system TestClock
#include "../../src/debug_runtime/deadline.h"
#undef umi_clock_system
#define umi_debug_runtime_adapter_wait_response TestWait
#define UmiDebugRuntimeAdapterWaitObserved TestObserved
#include "../../src/debug_runtime/adapter_wait.inc"
#include "../../src/debug_runtime/adapter_observed_wait.inc"
#undef UmiDebugRuntimeAdapterWaitObserved
#undef umi_debug_runtime_adapter_wait_response

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); result=1; goto cleanup; } } while (0)
typedef struct Peer {
    char input[4096];
    size_t size, offset, first_chunk;
    unsigned reads, delay_ms;
    UmiStatus read_status;
} Peer;
static UmiStatus Write(void *instance, const void *bytes, size_t size)
{ (void)instance; (void)bytes; (void)size; return UMI_STATUS_OK; }
static UmiStatus Read(void *instance, void *bytes, size_t capacity, uint32_t timeout, size_t *out)
{
    (void)timeout;
    Peer *p = instance;
    ++p->reads;
    now_ns += (uint64_t)p->delay_ms * UINT64_C(1000000);
    *out = 0U;
    if (p->read_status != UMI_STATUS_OK) return p->read_status;
    if (p->size == p->offset) return UMI_STATUS_NOT_FOUND;
    size_t n = p->size - p->offset;
    if (n > capacity) n = capacity;
    if (p->reads == 1U && p->first_chunk != 0U && n > p->first_chunk) n = p->first_chunk;
    memcpy(bytes, p->input + p->offset, n);
    p->offset += n; *out = n;
    return UMI_STATUS_OK;
}
static UmiStatus Stop(void *instance, uint32_t timeout)
{ (void)instance; (void)timeout; return UMI_STATUS_OK; }
static int Running(void *instance) { (void)instance; return 1; }
static void Destroy(void *instance) { (void)instance; }
static int Push(Peer *p, const char *json)
{
    int n = snprintf(p->input + p->size, sizeof(p->input) - p->size,
        "Content-Length: %zu\r\n\r\n%s", strlen(json), json);
    if (n < 0 || (size_t)n >= sizeof(p->input) - p->size) return 0;
    p->size += (size_t)n;
    return 1;
}
typedef struct Observer {
    UmiDebugRuntimeAdapter *adapter;
    UmiDebugRuntimeEnvelope *nested;
    int use_nested;
    unsigned calls;
} Observer;
static UmiStatus Observe(void *context, const UmiDebugRuntimeEnvelope *event,
    uint32_t remaining, int *consumed)
{
    Observer *o = context;
    ++o->calls; *consumed = 0;
    if (event->kind != UMI_DEBUG_RUNTIME_MESSAGE_EVENT) return UMI_STATUS_INVALID_STATE;
    if (o->use_nested) {
        *consumed = 1;
        UmiStatus status = TestWait(o->adapter, 2U, remaining, o->nested);
        if (status != UMI_STATUS_OK) return status;
    }
    now_ns += UINT64_C(11000000);
    return UMI_STATUS_OK;
}
static const char reply[] =
    "{\"seq\":10,\"type\":\"response\",\"request_seq\":1,\"command\":\"threads\",\"success\":true,\"body\":{}}";
static const char failed_reply[] =
    "{\"seq\":10,\"type\":\"response\",\"request_seq\":1,\"command\":\"threads\",\"success\":false,\"body\":{}}";
static const char other_reply[] =
    "{\"seq\":11,\"type\":\"response\",\"request_seq\":2,\"command\":\"scopes\",\"success\":true,\"body\":{}}";
static const char event[] =
    "{\"seq\":9,\"type\":\"event\",\"event\":\"initialized\",\"body\":{}}";

static int Run(const char *name)
{
    int result = 0;
    UmiDebugRuntimeAdapter *adapter = NULL;
    UmiDebugRuntimeEnvelope *out = malloc(sizeof(*out)), *before = malloc(sizeof(*before)),
        *scratch = malloc(sizeof(*scratch));
    Peer peer = {0};
    Observer observer = {0};
    CHECK(out != NULL && before != NULL && scratch != NULL);
    int observed = strncmp(name, "observed-", 9U) == 0;
    CHECK(observed || strncmp(name, "ordinary-", 9U) == 0);
    const char *mode = name + 9U;
    int seeded = strcmp(mode,"queued")==0 || strcmp(mode,"queued-cross")==0 || strcmp(mode,"zero-queued")==0;
    int nested = strcmp(mode,"observer-nested")==0, delayed = strcmp(mode,"observer-delay")==0;
    int late_event = strcmp(mode,"late-event")==0, late_other = strcmp(mode,"late-unrelated")==0;
    int full_response = strcmp(mode,"response-full")==0, full_event = strcmp(mode,"event-full")==0;
    int remote_failure = strcmp(mode,"remote-error")==0;
    int failed = strcmp(mode,"late-error")==0 || remote_failure;
    int zero = strncmp(mode,"zero",4U)==0;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode,"read-expired")==0 || late_event || late_other || strcmp(mode,"late-error")==0) {
        peer.delay_ms=11U; expected=UMI_STATUS_TIMEOUT;
    } else if (strcmp(mode,"boundary")==0) { peer.delay_ms=10U; expected=UMI_STATUS_TIMEOUT; }
    else if (strcmp(mode,"near")==0) peer.delay_ms=9U;
    else if (strcmp(mode,"empty-expired")==0) { peer.delay_ms=11U; expected=UMI_STATUS_TIMEOUT; }
    else if (strcmp(mode,"partial")==0) { peer.delay_ms=6U; expected=UMI_STATUS_TIMEOUT; }
    else if (strcmp(mode,"queued-cross")==0 || nested || delayed) expected=UMI_STATUS_TIMEOUT;
    else if (full_response || full_event) { peer.delay_ms=11U; expected=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(mode,"read-error")==0) { peer.read_status=UMI_STATUS_IO_ERROR; expected=UMI_STATUS_IO_ERROR; }
    else if (strcmp(mode,"read-unavailable")==0) { peer.read_status=UMI_STATUS_UNAVAILABLE; expected=UMI_STATUS_UNAVAILABLE; }
    else if (remote_failure) expected=UMI_STATUS_UNAVAILABLE;
    else if (strncmp(mode,"invalid-",8U)==0) expected=UMI_STATUS_INVALID_ARGUMENT;
    else CHECK(strcmp(mode,"valid")==0 || seeded || zero);
    CHECK(observed || (!nested && !delayed));
    if (observed && strcmp(mode,"zero")==0) expected=UMI_STATUS_TIMEOUT;
    if (late_event || delayed || nested || full_event) CHECK(Push(&peer,event));
    if (late_other) CHECK(Push(&peer,other_reply));
    if (strcmp(mode,"empty-expired")!=0) CHECK(Push(&peer,failed ? failed_reply : reply));
    if (nested) CHECK(Push(&peer,other_reply));
    if (strcmp(mode,"partial")==0) peer.first_chunk=peer.size/2U;
    UmiDebugRuntimeTransport transport={&peer,Write,Read,Stop,Running,Destroy};
    CHECK(umi_debug_runtime_adapter_create_with_transport("deadline",&transport,&adapter)==UMI_STATUS_OK);
    uint64_t sequence=0U;
    CHECK(umi_debug_runtime_adapter_send_request(adapter,"threads",NULL,"",&sequence)==UMI_STATUS_OK && sequence==1U);
    if (late_other || nested)
        CHECK(umi_debug_runtime_adapter_send_request(adapter,"scopes",NULL,"",&sequence)==UMI_STATUS_OK && sequence==2U);
    if (seeded) {
        CHECK(umi_debug_runtime_adapter_receive(adapter,0U,scratch)==UMI_STATUS_OK);
        CHECK(umi_debug_runtime_response_queue_push(adapter->responses,scratch)==UMI_STATUS_OK);
    }
    if (full_response || full_event) {
        CHECK(umi_debug_runtime_message_parse(full_event ? event : other_reply,scratch)==UMI_STATUS_OK);
        size_t limit=full_event ? UMI_DEBUG_RUNTIME_MAX_EVENTS : UMI_DEBUG_RUNTIME_MAX_DEFERRED_RESPONSES;
        for (size_t i=0U;i<limit;++i) {
            /* Synthetic existing queue entries have distinct identities; the
             * incoming late frame still uses an independently encoded message. */
            scratch->request_sequence=100U+(uint64_t)i;
            CHECK((full_event ? umi_debug_runtime_event_queue_push(adapter->events,scratch) :
                umi_debug_runtime_response_queue_push(adapter->responses,scratch))==UMI_STATUS_OK);
        }
    }
    unsigned initial_reads=peer.reads;
    if (strcmp(mode,"queued-cross")==0) clock_step=UINT64_C(6000000);
    memset(out,0x5a,sizeof(*out)); memcpy(before,out,sizeof(*out));
    observer.adapter=adapter; observer.nested=scratch; observer.use_nested=nested;
    UmiDebugRuntimeAdapter *input=strcmp(mode,"invalid-adapter")==0 ? NULL : adapter;
    uint64_t request=strcmp(mode,"invalid-request")==0 ? 0U : 1U;
    UmiDebugRuntimeEnvelope *destination=strcmp(mode,"invalid-output")==0 ? NULL : out;
    UmiStatus status=observed ?
        TestObserved(input,request,zero?0U:10U,(delayed||nested||late_event)?Observe:NULL,&observer,destination) :
        TestWait(input,request,zero?0U:10U,destination);
    CHECK(status==expected);
    if (expected!=UMI_STATUS_OK && !remote_failure) CHECK(memcmp(out,before,sizeof(*out))==0);
    else CHECK(out->request_sequence==1U && out->success==!remote_failure);
    if (seeded || strncmp(mode,"invalid-",8U)==0 || (observed&&strcmp(mode,"zero")==0))
        CHECK(peer.reads==initial_reads);
    else CHECK(peer.reads-initial_reads==(strcmp(mode,"partial")==0 ? 2U : 1U));
    CHECK(observer.calls==(delayed||nested ? 1U : 0U));
    clock_step=0U; peer.delay_ms=0U; peer.read_status=UMI_STATUS_OK;
    UmiDebugRuntimeAdapterSnapshot snapshot;
    CHECK(umi_debug_runtime_adapter_snapshot(adapter,&snapshot)==UMI_STATUS_OK);
    if (full_response) CHECK(snapshot.queued_responses==UMI_DEBUG_RUNTIME_MAX_DEFERRED_RESPONSES);
    if (full_event) CHECK(snapshot.queued_events==UMI_DEBUG_RUNTIME_MAX_EVENTS);
    if (expected==UMI_STATUS_TIMEOUT && strcmp(mode,"empty-expired")!=0) {
        /* A fresh explicit poll can inspect retained/buffered results. It does
         * not retroactively turn the earlier timed-out operation into a pass. */
        CHECK(TestWait(adapter,1U,0U,scratch)==(failed ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK));
        CHECK(scratch->request_sequence==1U);
        if (late_event || delayed) {
            CHECK(umi_debug_runtime_adapter_next_event(adapter,scratch)==UMI_STATUS_OK);
            CHECK(strcmp(scratch->event,"initialized")==0);
        }
        if (late_other) {
            CHECK(TestObserved(adapter,2U,0U,NULL,NULL,scratch)==UMI_STATUS_OK);
            CHECK(scratch->request_sequence==2U);
        }
        if (nested) CHECK(snapshot.queued_responses==1U && snapshot.queued_events==0U);
    }
cleanup:
    umi_debug_runtime_adapter_destroy(adapter);
    free(out); free(before); free(scratch);
    return result;
}
int main(int argc,char **argv) { return argc==2 ? Run(argv[1]) : 2; }
