/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_stream_job.c
 *
 * PURPOSE:
 *   Exercise actual worker ownership with controlled exchange completion, partial text and native lifecycle failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/local_stream.h"
#include "umicom/platform/threading.h"
#include "local_http_stream_internal.h"
#include "local_chat_stream_fixture.h"
#include <stdatomic.h>
static const char *mode,*expected_body;
static atomic_bool entered,partial,proceed;
static bool fail_start,fail_join,fail_release;
static UmiStatus StartThread(UmiThreadEntry entry,void *context,UmiThread **out)
{
    if(fail_start){fail_start=false;return UMI_STATUS_IO_ERROR;}
    return umi_thread_start(entry,context,out);
}
static UmiStatus JoinThread(UmiThread *thread,int *out)
{
    if(fail_join){fail_join=false;return UMI_STATUS_IO_ERROR;}
    return umi_thread_join(thread,out);
}
static UmiStatus ReleaseThread(UmiThread **thread)
{
    if(fail_release){fail_release=false;return UMI_STATUS_IO_ERROR;}
    return UmiThreadRelease(thread);
}
static UmiStatus Exchange(uint16_t port,uint32_t timeout,const char *body,uint64_t limit,
    const UmiAiWorkspaceCancellation *token,AwLocalChatStream *protocol,AwLocalHttpStreamResult *out)
{
    atomic_store_explicit(&entered,true,memory_order_release);
    if(port!=8080U || timeout!=1000U || limit!=16384U || strcmp(body,expected_body)!=0)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out=(AwLocalHttpStreamResult){0};out->response.http_status=200U;
    if(strcmp(mode,"setup-failure")==0)return UMI_STATUS_IO_ERROR;
    UmiStatus status=AwLocalChatStreamFeed(protocol,TEXT_HELLO,strlen(TEXT_HELLO));
    if(status!=UMI_STATUS_OK)return status;
    atomic_store_explicit(&partial,true,memory_order_release);
    size_t waited=0U;
    while(!atomic_load_explicit(&proceed,memory_order_acquire) &&
        !UmiAiWorkspaceCancellationIsRequested(token)){
        if(++waited>5000U)return UMI_STATUS_TIMEOUT;
        umi_thread_sleep_ms(1U);
    }
    if(UmiAiWorkspaceCancellationIsRequested(token))return UMI_STATUS_CANCELLED;
    if(strcmp(mode,"transport-failure")==0)return UMI_STATUS_IO_ERROR;
    status=AwLocalChatStreamFeed(protocol,STOP DONE,strlen(STOP DONE));
    if(status==UMI_STATUS_OK)status=AwLocalChatStreamFinish(protocol);
    out->status=status;return status;
}
/* Run the actual controller and Framework thread/text/cancellation owners.
 * Only HTTP exchange and three native lifecycle error boundaries are injected. */
#define AwLocalChatExchangeStream Exchange
#define umi_thread_start StartThread
#define umi_thread_join JoinThread
#define UmiThreadRelease ReleaseThread
#include "../../src/ai_workspace/local_stream.c"
#undef AwLocalChatExchangeStream
#undef umi_thread_start
#undef umi_thread_join
#undef UmiThreadRelease
static int AwaitFlag(atomic_bool *flag)
{
    for(size_t i=0U;i<5000U;++i){
        if(atomic_load_explicit(flag,memory_order_acquire))return 0;
        umi_thread_sleep_ms(1U);
    }
    return 1;
}
static int AwaitFinished(UmiAiLocalStream *job)
{
    for(size_t i=0U;i<5000U;++i){
        UmiAiLocalStreamSnapshot state;
        if(UmiAiLocalStreamPoll(job,&state)!=UMI_STATUS_OK)return 1;
        if(state.state==UMI_AI_LOCAL_STREAM_FINISHED)return 0;
        umi_thread_sleep_ms(1U);
    }
    return 1;
}
static int Text(UmiAiLocalStream *job,const char *expected,UmiAiResponseStreamState state)
{
    char *copy=NULL;UmiAiResponseStreamSnapshot snapshot;
    CHECK(UmiAiLocalStreamCopyText(job,&copy,&snapshot)==UMI_STATUS_OK);
    CHECK(strcmp(copy,expected)==0 && snapshot.state==state);
    CHECK(snapshot.identity.request_id==17U && snapshot.identity.model_revision==29U);
    UmiAiResponseStreamTextFree(copy);return 0;
}
typedef struct WrongOwnerContext {UmiAiLocalStream *job;UmiAiLocalTransport *runtime;} WrongOwnerContext;
static int WrongOwner(void *opaque)
{
    WrongOwnerContext *context=opaque;UmiAiLocalStream *job=context->job,*before=job;
    UmiAiLocalTransport *runtime=context->runtime,*runtime_before=runtime;
    UmiAiLocalStreamSnapshot snapshot;UmiAiLocalStreamResult result;
    UmiAiResponseStreamSnapshot text_snapshot;char *text=NULL;
    CHECK(UmiAiLocalStreamStart(job)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiLocalStreamCancel(job)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiLocalStreamPoll(job,&snapshot)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiLocalStreamCopyText(job,&text,&text_snapshot)==UMI_STATUS_INVALID_STATE && text==NULL);
    CHECK(UmiAiLocalStreamReadResult(job,&result)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiLocalStreamTryJoin(job)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiLocalStreamJoin(job)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiLocalStreamDestroy(&job)==UMI_STATUS_INVALID_STATE && job==before);
    CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_INVALID_STATE && runtime==runtime_before);
    return 0;
}
static int Arguments(UmiAiLocalTransport *runtime,UmiAiRequest *request,UmiAiLocalStreamOptions options)
{
    UmiAiLocalStream *sentinel=(UmiAiLocalStream *)(uintptr_t)1U,*output=sentinel;
    CHECK(UmiAiLocalStreamCreate(NULL,request,&options,&output)==UMI_STATUS_INVALID_ARGUMENT && output==sentinel);
    CHECK(UmiAiLocalStreamCreate(runtime,NULL,&options,&output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiLocalStreamCreate(runtime,request,NULL,&output)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiLocalStreamCreate(runtime,request,&options,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    UmiAiLocalStreamOptions invalid=options;invalid.allow_network=false;
    CHECK(UmiAiLocalStreamCreate(runtime,request,&invalid,&output)==UMI_STATUS_PERMISSION_DENIED);
    invalid=options;invalid.port=1023U;
    CHECK(UmiAiLocalStreamCreate(runtime,request,&invalid,&output)==UMI_STATUS_INVALID_ARGUMENT);
    invalid=options;invalid.timeout_millis=99U;
    CHECK(UmiAiLocalStreamCreate(runtime,request,&invalid,&output)==UMI_STATUS_INVALID_ARGUMENT);
    invalid=options;invalid.maximum_text_bytes=0U;
    CHECK(UmiAiLocalStreamCreate(runtime,request,&invalid,&output)==UMI_STATUS_INVALID_ARGUMENT);
    invalid=options;invalid.maximum_wire_bytes=UMI_AI_LOCAL_STREAM_MAX_WIRE_BYTES+1U;
    CHECK(UmiAiLocalStreamCreate(runtime,request,&invalid,&output)==UMI_STATUS_INVALID_ARGUMENT);
    invalid=options;invalid.identity.model_revision=0U;
    CHECK(UmiAiLocalStreamCreate(runtime,request,&invalid,&output)==UMI_STATUS_INVALID_ARGUMENT);
    request->message_count=0U;
    CHECK(UmiAiLocalStreamCreate(runtime,request,&options,&output)!=UMI_STATUS_OK && output==sentinel);
    size_t count=99U;CHECK(UmiAiLocalTransportRequestCount(runtime,&count)==UMI_STATUS_OK && count==0U);
    CHECK(UmiAiLocalStreamStart(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiLocalStreamCancel(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiLocalStreamDestroy(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    output=NULL;CHECK(UmiAiLocalStreamDestroy(&output)==UMI_STATUS_OK);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;mode=argv[1];
    const char *modes[]={"arguments","success","input-copy","setup-failure","transport-failure","cancel-before",
        "cancel-running","completed-before-join","thread-start-failure","join-failure","release-failure","wrong-owner"};
    bool known=false;for(size_t i=0U;i<sizeof modes/sizeof modes[0];++i)if(strcmp(mode,modes[i])==0)known=true;
    if(!known)return 2;
    UmiAiLocalTransport *runtime=NULL;
    UmiStatus available=UmiAiLocalTransportCreate(&runtime);
    if(available==UMI_STATUS_UNAVAILABLE){fputs("SKIP: native thread-safe HTTP runtime unavailable.\n",stderr);return 77;}
    CHECK(available==UMI_STATUS_OK);
    atomic_init(&entered,false);atomic_init(&partial,false);atomic_init(&proceed,false);
    UmiAiRequest *request=calloc(1U,sizeof *request);CHECK(request!=NULL);
    strcpy(request->request_id,"sequence.test");strcpy(request->model_id,"test-model");
    request->message_count=1U;request->max_output_tokens=64U;
    request->messages[0].role=UMI_AI_ROLE_USER;strcpy(request->messages[0].text,"Explain ownership");
    UmiAiLocalStreamOptions options={.port=8080U,.timeout_millis=1000U,.maximum_text_bytes=64U,
        .maximum_wire_bytes=16384U,.identity={17U,29U},.allow_network=true};
    if(strcmp(mode,"arguments")==0){
        int result=Arguments(runtime,request,options);free(request);
        CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_OK);return result;
    }
    char *encoded=NULL;CHECK(AwLocalChatEncodeStreaming(request,&encoded)==UMI_STATUS_OK);expected_body=encoded;
    UmiAiLocalStream *job=NULL;CHECK(UmiAiLocalStreamCreate(runtime,request,&options,&job)==UMI_STATUS_OK);
    size_t count=0U;CHECK(UmiAiLocalTransportRequestCount(runtime,&count)==UMI_STATUS_OK && count==1U);
    CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_BUSY);
    UmiAiLocalStreamResult result,before;memset(&result,0xa5,sizeof result);memcpy(&before,&result,sizeof before);
    CHECK(UmiAiLocalStreamReadResult(job,&result)==UMI_STATUS_BUSY && memcmp(&result,&before,sizeof result)==0);
    CHECK(Text(job,"",UMI_AI_RESPONSE_STREAM_ACTIVE)==0);
    if(strcmp(mode,"wrong-owner")==0){
        WrongOwnerContext context={job,runtime};UmiThread *thread=NULL;int exit_code=1;
        CHECK(umi_thread_start(WrongOwner,&context,&thread)==UMI_STATUS_OK);
        CHECK(umi_thread_join(thread,&exit_code)==UMI_STATUS_OK && exit_code==0);
        CHECK(UmiThreadRelease(&thread)==UMI_STATUS_OK);
        CHECK(!atomic_load_explicit(&entered,memory_order_acquire));
    }else if(strcmp(mode,"cancel-before")==0){
        CHECK(UmiAiLocalStreamCancel(job)==UMI_STATUS_OK);
        CHECK(UmiAiLocalStreamStart(job)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiAiLocalStreamReadResult(job,&result)==UMI_STATUS_OK && result.operation_status==UMI_STATUS_CANCELLED);
        CHECK(Text(job,"",UMI_AI_RESPONSE_STREAM_CANCELLED)==0);
        CHECK(!atomic_load_explicit(&entered,memory_order_acquire));
    }else{
        if(strcmp(mode,"input-copy")==0){memset(request,0xa5,sizeof *request);memset(&options,0,sizeof options);}
        if(strcmp(mode,"thread-start-failure")==0){
            fail_start=true;CHECK(UmiAiLocalStreamStart(job)==UMI_STATUS_IO_ERROR);
            UmiAiLocalStreamSnapshot state;CHECK(UmiAiLocalStreamPoll(job,&state)==UMI_STATUS_OK && state.state==UMI_AI_LOCAL_STREAM_CREATED);
        }
        CHECK(UmiAiLocalStreamStart(job)==UMI_STATUS_OK);
        CHECK(UmiAiLocalStreamStart(job)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiAiLocalStreamDestroy(&job)==UMI_STATUS_BUSY);
        if(strcmp(mode,"setup-failure")!=0){
            CHECK(AwaitFlag(&partial)==0);
            CHECK(Text(job,"Hello",UMI_AI_RESPONSE_STREAM_ACTIVE)==0);
            CHECK(UmiAiLocalStreamReadResult(job,&result)==UMI_STATUS_BUSY);
        }
        if(strcmp(mode,"cancel-running")==0)CHECK(UmiAiLocalStreamCancel(job)==UMI_STATUS_OK);
        atomic_store_explicit(&proceed,true,memory_order_release);
        if(strcmp(mode,"completed-before-join")==0){
            CHECK(AwaitFinished(job)==0);
            CHECK(UmiAiLocalStreamDestroy(&job)==UMI_STATUS_BUSY);
        }
        if(strcmp(mode,"join-failure")==0){
            fail_join=true;CHECK(UmiAiLocalStreamJoin(job)==UMI_STATUS_IO_ERROR);
            CHECK(UmiAiLocalStreamDestroy(&job)==UMI_STATUS_BUSY);
        }
        CHECK(UmiAiLocalStreamJoin(job)==UMI_STATUS_OK);
        CHECK(UmiAiLocalStreamTryJoin(job)==UMI_STATUS_OK);
        CHECK(UmiAiLocalStreamReadResult(job,&result)==UMI_STATUS_OK && result.observation_status==UMI_STATUS_OK);
        UmiStatus expected=strcmp(mode,"cancel-running")==0?UMI_STATUS_CANCELLED:
            (strcmp(mode,"setup-failure")==0 || strcmp(mode,"transport-failure")==0)?UMI_STATUS_IO_ERROR:UMI_STATUS_OK;
        CHECK(result.operation_status==expected);
        CHECK(Text(job,strcmp(mode,"setup-failure")==0?"":"Hello",expected==UMI_STATUS_OK?UMI_AI_RESPONSE_STREAM_COMPLETE:
            expected==UMI_STATUS_CANCELLED?UMI_AI_RESPONSE_STREAM_CANCELLED:UMI_AI_RESPONSE_STREAM_FAILED)==0);
        CHECK(UmiAiLocalStreamCancel(job)==UMI_STATUS_OK);
        CHECK(UmiAiLocalStreamReadResult(job,&result)==UMI_STATUS_OK && result.operation_status==expected);
        if(strcmp(mode,"release-failure")==0){
            fail_release=true;UmiAiLocalStream *owned=job;
            CHECK(UmiAiLocalStreamDestroy(&job)==UMI_STATUS_IO_ERROR && job==owned);
            CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_BUSY);
        }
    }
    CHECK(UmiAiLocalStreamDestroy(&job)==UMI_STATUS_OK && job==NULL);
    CHECK(UmiAiLocalTransportRequestCount(runtime,&count)==UMI_STATUS_OK && count==0U);
    CHECK(UmiAiLocalTransportDestroy(&runtime)==UMI_STATUS_OK);
    AwLocalChatFree(encoded);free(request);return 0;
}
