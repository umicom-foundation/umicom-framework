/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_reply_deadline.c
 *
 * PURPOSE:
 *   Verify lifecycle and source-query deadline boundaries using an injected deterministic clock.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/language_runtime/server_manager.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t now_ns;
static uint64_t Now(const UmiClock *clock) { (void)clock; return now_ns; }
static UmiStatus Sleep(UmiClock *clock, uint64_t milliseconds)
{
    (void)clock;
    now_ns += milliseconds * UINT64_C(1000000);
    return UMI_STATUS_OK;
}
static UmiClock DeadlineClock(void)
{
    return (UmiClock){Now, Now, Sleep, NULL};
}
/* Compile the narrow production wait boundary with an isolated clock, not a
 * production injection flag. The actual server/framer/transport remain linked
 * from Framework; no child process, wall-clock sleep or GUI is involved. */
#define umi_clock_system DeadlineClock
#define UmiLanguageRuntimeServerInitialize DeadlineInitialize
#define UmiLanguageRuntimeServerShutdown DeadlineShutdown
#include "../../src/language_runtime/server_lifecycle.c"
#include "../../src/language_runtime/reply_wait.inc"
#undef UmiLanguageRuntimeServerShutdown
#undef UmiLanguageRuntimeServerInitialize
#undef umi_clock_system

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);result=1;goto cleanup; } } while(0)
typedef struct Peer {
    char input[4096];
    size_t size, offset, first_chunk;
    unsigned reads, writes, stops;
    unsigned write_delay_ms, read_delay_ms;
    uint32_t stop_timeout;
    int running;
} Peer;
static UmiStatus Write(void *instance, const void *bytes, size_t size)
{
    (void)bytes; (void)size;
    Peer *peer = instance;
    if (peer->writes++ == 0U) now_ns += (uint64_t)peer->write_delay_ms * UINT64_C(1000000);
    return UMI_STATUS_OK;
}
static UmiStatus Read(void *instance, void *bytes, size_t capacity, uint32_t timeout, size_t *out)
{
    (void)timeout;
    Peer *peer = instance;
    ++peer->reads;
    now_ns += (uint64_t)peer->read_delay_ms * UINT64_C(1000000);
    *out = 0U;
    if (peer->offset == peer->size) return UMI_STATUS_NOT_FOUND;
    size_t count = peer->size - peer->offset;
    if (count > capacity) count = capacity;
    if (peer->reads == 1U && peer->first_chunk != 0U && count > peer->first_chunk)
        count = peer->first_chunk;
    memcpy(bytes, peer->input + peer->offset, count);
    peer->offset += count; *out = count;
    return UMI_STATUS_OK;
}
static UmiStatus Stop(void *instance, uint32_t timeout)
{
    Peer *peer = instance;
    ++peer->stops; peer->stop_timeout = timeout; peer->running = 0;
    return UMI_STATUS_OK;
}
static int Running(void *instance) { return ((Peer *)instance)->running; }
static void Destroy(void *instance) { (void)instance; }
static int Push(Peer *peer, const char *json)
{
    int written = snprintf(peer->input + peer->size, sizeof(peer->input) - peer->size,
        "Content-Length: %zu\r\n\r\n%s", strlen(json), json);
    if (written < 0 || (size_t)written >= sizeof(peer->input) - peer->size) return 0;
    peer->size += (size_t)written;
    return 1;
}
static int Check(const char *name)
{
    int result = 0;
    UmiLanguageRuntimeServer *server = NULL;
    UmiLanguageRuntimeEnvelope *reply = NULL;
    UmiCancellationToken *token = NULL;
    Peer peer = {0}; peer.running = 1;
    int initialize = strncmp(name, "init-", 5U) == 0;
    int shutdown = strncmp(name, "shutdown-", 9U) == 0;
    CHECK(initialize || shutdown || strncmp(name, "query-", 6U) == 0);
    const char *mode = initialize ? name + 5U : shutdown ? name + 9U : name + 6U;
    uint32_t timeout = strncmp(mode, "zero", 4U) == 0 ? 0U : 10U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "send-expired") == 0) {peer.write_delay_ms=11U; expected=UMI_STATUS_TIMEOUT;}
    else if (strcmp(mode, "before") == 0) {now_ns=UINT64_C(11000000); expected=UMI_STATUS_TIMEOUT;}
    else if (strcmp(mode, "read-expired") == 0) {peer.read_delay_ms=11U; expected=UMI_STATUS_TIMEOUT;}
    else if (strcmp(mode, "boundary") == 0) {peer.read_delay_ms=10U; expected=UMI_STATUS_TIMEOUT;}
    else if (strcmp(mode, "near") == 0) peer.read_delay_ms=9U;
    else if (strcmp(mode, "zero-empty") == 0 || strcmp(mode, "zero-notification") == 0) expected=UMI_STATUS_TIMEOUT;
    else if (strcmp(mode, "cancel") == 0) expected=UMI_STATUS_CANCELLED;
    else if (strcmp(mode, "fragments") == 0) {peer.read_delay_ms=6U; expected=UMI_STATUS_TIMEOUT;}
    else CHECK(strcmp(mode,"valid")==0 || strcmp(mode,"zero")==0);
    if (strcmp(mode,"zero-notification")==0 || strcmp(mode,"fragments")==0) {
        CHECK(Push(&peer,"{\"jsonrpc\":\"2.0\",\"method\":\"fixture/notice\",\"params\":{}}"));
        if (strcmp(mode,"fragments")==0) peer.first_chunk=peer.size;
    }
    if (strcmp(mode,"zero-empty")!=0)
        CHECK(Push(&peer, initialize ?
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":{\"capabilities\":{}}}" :
            "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":null}"));
    UmiLanguageServerProfile profile={0};
    strcpy(profile.id,"deadline.peer");strcpy(profile.executable,"not-executed");profile.enabled=1;
    UmiLanguageRuntimeTransport transport={&peer,Write,Read,Stop,Running,Destroy};
    CHECK(umi_language_runtime_server_create_with_transport("deadline",&profile,"file:///workspace",&transport,&server)==UMI_STATUS_OK);
    if (strcmp(mode,"cancel")==0) {
        CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        umi_cancellation_token_request(token);
    }
    const UmiCancellationToken *cancel=token;
    UmiStatus status;
    if (initialize) {
        UmiLanguageRuntimeInitializeResult capabilities,previous;
        memset(&capabilities,0x5a,sizeof(capabilities));memcpy(&previous,&capabilities,sizeof(capabilities));
        status=DeadlineInitialize(server,"file:///workspace",timeout,cancel,&capabilities);
        CHECK(status==expected);
        if (status!=UMI_STATUS_OK)CHECK(memcmp(&capabilities,&previous,sizeof(capabilities))==0);
    } else if (shutdown) {
        CHECK(umi_language_runtime_server_transition(server,UMI_LANGUAGE_RUNTIME_SERVER_INITIALIZING)==UMI_STATUS_OK);
        CHECK(umi_language_runtime_server_transition(server,UMI_LANGUAGE_RUNTIME_SERVER_READY)==UMI_STATUS_OK);
        status=DeadlineShutdown(server,timeout);
        CHECK(status==expected && peer.stops==1U && !peer.running);
        if (expected==UMI_STATUS_TIMEOUT) CHECK(peer.stop_timeout==0U);
    } else {
        reply=malloc(sizeof(*reply));CHECK(reply!=NULL);
        UmiClock clock=DeadlineClock();
        status=QueryAwait(server,1U,&clock,0U,timeout,cancel,reply);
        CHECK(status==expected);
    }
    if (strcmp(mode,"send-expired")==0 || strcmp(mode,"before")==0 || strcmp(mode,"cancel")==0)
        CHECK(peer.reads==0U);
    else if (strcmp(mode,"fragments")==0)CHECK(peer.reads==2U);
    else CHECK(peer.reads==1U);
cleanup:
    umi_cancellation_token_destroy(token);
    free(reply);
    umi_language_runtime_server_destroy(server);
    return result;
}
int main(int argc,char **argv)
{
    return argc==2 ? Check(argv[1]) : 2;
}
