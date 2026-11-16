/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_pixverse_tasks.c
 *
 * PURPOSE:
 *   Exercise offline video receipt binding, durable transitions and explicit status recovery without provider access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/pixverse_tasks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x);exit(1); } } while(0)
static const char trace[]="93cb2fa6-e5da-4c69-9588-94ca9839f238";
static const char next_trace[]="93cb2fa6-e5da-4c69-9588-94ca9839f239";
typedef struct Fixture {
    UmiProviderConnection connection;
    UmiPixVerseDraft draft;
    UmiPixVersePlan *plan;
    UmiProviderTaskReceipt receipt;
    UmiDataServer *server;
    UmiProviderTasks *store;
    UmiCancellationToken *cancel;
    const char *reply;
    UmiStatus exchange_status;
    unsigned calls;
    bool cancel_during;
} Fixture;
/* The only transport used by this executable is a bounded synthetic reply.
 * No fixture invokes curl, loads keys, follows URLs or launches a process. */
static UmiStatus Exchange(void *context,const char *url,const char *body,const char *request_trace,
    const char *key,const UmiCancellationToken *cancel,char *reply,size_t capacity,size_t *length,unsigned *http)
{
    Fixture *f=context;(void)cancel;
    CHECK(url && request_trace && !strcmp(key,"fixture-key"));
    CHECK(body?strstr(url,"/video/text/generate")!=NULL:strstr(url,"/video/result/")!=NULL);
    ++f->calls;*http=200U;*length=strlen(f->reply);CHECK(*length<capacity);
    memcpy(reply,f->reply,*length);
    if(f->cancel_during) umi_cancellation_token_request(f->cancel);
    return f->exchange_status;
}
static void Init(Fixture *f)
{
    memset(f,0,sizeof(*f));
    strcpy(f->connection.id,"video");strcpy(f->connection.provider_id,"pixverse");
    strcpy(f->connection.label,"PixVerse fixture");
    strcpy(f->connection.endpoint,"https://app-api.pixverse.ai/openapi/v2/");
    strcpy(f->connection.model,"v6");strcpy(f->connection.secret_reference,"vault:fixture");
    f->connection.route=UMI_PROVIDER_CONNECTION_HTTPS;f->connection.timeout_ms=30000U;f->connection.enabled=true;
    f->draft.operation=UMI_PIXVERSE_CREATE_VIDEO;strcpy(f->draft.trace_id,trace);
    strcpy(f->draft.prompt,"A quiet sea");strcpy(f->draft.aspect_ratio,"16:9");
    f->draft.seconds=5U;f->draft.quality=720U;
    CHECK(UmiPixVersePrepare(&f->draft,&f->plan)==UMI_STATUS_OK);
    CHECK(UmiPixVerseTaskPrepare(f->plan,"media","lesson","scene-one",&f->connection,7U,&f->receipt)==UMI_STATUS_OK);
    CHECK(umi_data_server_create_memory(&f->server)==UMI_STATUS_OK);
    CHECK(UmiProviderTasksOpen(f->server,"media","lesson",&f->store)==UMI_STATUS_OK);
    CHECK(UmiProviderTasksCreate(f->store,&f->receipt)==UMI_STATUS_OK);
    CHECK(umi_cancellation_token_create(&f->cancel)==UMI_STATUS_OK);
    f->reply="{\"ErrCode\":0,\"Resp\":{\"video_id\":42}}";f->exchange_status=UMI_STATUS_OK;
}
static void Begin(Fixture *f)
{
    CHECK(UmiProviderTasksAdvance(f->store,"scene-one",1U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&f->receipt)==UMI_STATUS_OK);
    CHECK(f->receipt.phase==UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN && f->receipt.revision==2U);
}
static UmiStatus Execute(Fixture *f,UmiPixVersePlan *plan,UmiPixVerseResult *result)
{
    return UmiPixVerseExecuteWithTransport(plan,true,"fixture-key",f->cancel,Exchange,f,result);
}
static void Accepted(Fixture *f)
{
    UmiPixVerseResult result;Begin(f);CHECK(Execute(f,f->plan,&result)==UMI_STATUS_OK);
    CHECK(UmiPixVerseTaskRecordResult(f->store,"scene-one",2U,f->plan,UMI_STATUS_OK,&result,&f->receipt)==UMI_STATUS_OK);
    CHECK(f->receipt.phase==UMI_PROVIDER_TASK_SUBMITTED && f->receipt.revision==3U && !strcmp(f->receipt.remote_id,"42"));
}
static void ObserveFailure(Fixture *f,UmiPixVersePlan *plan,UmiStatus execution,
    const UmiPixVerseResult *result,UmiStatus wanted)
{
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
    UmiProviderTaskReceipt output,before;memset(&output,0xa5,sizeof(output));before=output;
#endif
    UmiProviderTaskReceipt output,before;memset(&output,0xa5,sizeof(output));memcpy(&before,&output,sizeof(before));
    CHECK(UmiPixVerseTaskObserve(&f->receipt,f->receipt.revision,plan,execution,result,&output)==wanted);
    CHECK(!memcmp(&output,&before,sizeof(output)));
}

/* Successful persistence reconstructs values, not compiler padding. Validate
 * bounded strings first, then compare every declared receipt field. Refusal
 * checks below still compare exact bytes captured with memcpy. */
static void SameReceipt(const UmiProviderTaskReceipt *actual,const UmiProviderTaskReceipt *expected)
{
    CHECK(UmiProviderTaskValidate(actual)==UMI_STATUS_OK && UmiProviderTaskValidate(expected)==UMI_STATUS_OK);
    CHECK(actual->schema==expected->schema && actual->revision==expected->revision);
    CHECK(!strcmp(actual->context.application_id,expected->context.application_id));
    CHECK(!strcmp(actual->context.profile_id,expected->context.profile_id));
    CHECK(!strcmp(actual->context.local_id,expected->context.local_id));
    CHECK(!strcmp(actual->context.operation_id,expected->context.operation_id));
    CHECK(!memcmp(actual->context.request_sha256,expected->context.request_sha256,UMI_SHA256_BYTES));
    CHECK(!strcmp(actual->connection_id,expected->connection_id));
    CHECK(!strcmp(actual->provider_id,expected->provider_id));
    CHECK(actual->connection_revision==expected->connection_revision);
    CHECK(!memcmp(actual->connection_fingerprint,expected->connection_fingerprint,UMI_SHA256_BYTES));
    CHECK(actual->phase==expected->phase && !strcmp(actual->remote_id,expected->remote_id));
    CHECK(actual->output_bytes==expected->output_bytes);
    CHECK(!memcmp(actual->output_sha256,expected->output_sha256,UMI_SHA256_BYTES));
}

static void Close(Fixture *f)
{
    UmiPixVerseDestroy(f->plan);umi_cancellation_token_destroy(f->cancel);
    UmiProviderTasksDestroy(f->store);umi_data_server_destroy(f->server);
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];Fixture f;Init(&f);
    if(!strcmp(mode,"prepare")) {
        char hex[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(f.receipt.context.request_sha256,hex);
        CHECK(!strcmp(hex,"53200540d51b265bce89654b81d3b411f8f9e89b21204f784d0479cf7c5bf3d0"));
        CHECK(f.receipt.phase==UMI_PROVIDER_TASK_PREPARED && f.receipt.revision==1U && f.receipt.connection_revision==7U);
        CHECK(!strcmp(f.receipt.context.operation_id,UMI_PIXVERSE_TASK_OPERATION) && !f.receipt.remote_id[0] && f.calls==0U);
    } else if(!strcmp(mode,"request-identity")) {
        for(unsigned i=0U;i<2U;++i) {
            UmiPixVerseDraft draft=f.draft;UmiPixVersePlan *other=NULL;UmiProviderTaskReceipt receipt;
            if(i==0U) strcpy(draft.prompt,"A different scene");else strcpy(draft.trace_id,next_trace);
            CHECK(UmiPixVersePrepare(&draft,&other)==UMI_STATUS_OK);
            CHECK(UmiPixVerseTaskPrepare(other,"media","lesson","scene-one",&f.connection,7U,&receipt)==UMI_STATUS_OK);
            CHECK(memcmp(receipt.context.request_sha256,f.receipt.context.request_sha256,UMI_SHA256_BYTES)!=0);
            UmiPixVerseDestroy(other);
        }
    } else if(!strcmp(mode,"connection")) {
        for(unsigned i=0U;i<6U;++i) {
            UmiProviderConnection connection=f.connection;UmiProviderTaskReceipt output,before;
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
            memset(&output,0xa5,sizeof(output));before=output;
#endif
            memset(&output,0xa5,sizeof(output));memcpy(&before,&output,sizeof(before));
            if(i==0U) strcpy(connection.provider_id,"another");
            if(i==1U) strcpy(connection.endpoint,"https://unrelated.example/api");
            if(i==2U) strcpy(connection.model,"another-model");
            if(i==3U) connection.enabled=false;
            if(i==4U) connection.route=(UmiProviderConnectionRoute)99;
            if(i==5U) memset(connection.endpoint,'x',sizeof(connection.endpoint));
            CHECK(UmiPixVerseTaskPrepare(f.plan,"media","lesson","scene-one",&connection,7U,&output)!=UMI_STATUS_OK);
            CHECK(!memcmp(&output,&before,sizeof(output)));
        }
    } else if(!strcmp(mode,"scope")) {
        const char *bad[]={"","UPPER","path/name",NULL};
        for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i) {
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
            UmiProviderTaskReceipt output=f.receipt;
#endif
            UmiProviderTaskReceipt output;memcpy(&output,&f.receipt,sizeof(output));
            CHECK(UmiPixVerseTaskPrepare(f.plan,bad[i],"lesson","scene-one",&f.connection,7U,&output)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(!memcmp(&output,&f.receipt,sizeof(output)));
        }
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
        char full[49];memset(full,'a',sizeof(full));UmiProviderTaskReceipt output=f.receipt;
#endif
        char full[49];memset(full,'a',sizeof(full));UmiProviderTaskReceipt output;memcpy(&output,&f.receipt,sizeof(output));
        CHECK(UmiPixVerseTaskPrepare(f.plan,"media","lesson",full,&f.connection,7U,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&output,&f.receipt,sizeof(output)));
    } else if(!strcmp(mode,"unconsumed")) {
        Begin(&f);UmiPixVerseResult result={0};result.video_id=42;result.http_status=200U;
        result.creation_may_exist=true;result.creation_confirmed=true;
        ObserveFailure(&f,f.plan,UMI_STATUS_OK,&result,UMI_STATUS_INVALID_STATE);CHECK(f.calls==0U);
    } else if(!strcmp(mode,"uncertain-timeout") || !strcmp(mode,"cancel-before")) {
        Begin(&f);UmiPixVerseResult result;
        UmiStatus expected=!strcmp(mode,"cancel-before")?UMI_STATUS_CANCELLED:UMI_STATUS_TIMEOUT;
        if(expected==UMI_STATUS_CANCELLED) umi_cancellation_token_request(f.cancel);else f.exchange_status=expected;
        CHECK(Execute(&f,f.plan,&result)==expected);ObserveFailure(&f,f.plan,expected,&result,expected);
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
        UmiProviderTaskReceipt output=f.receipt;
#endif
        UmiProviderTaskReceipt output;memcpy(&output,&f.receipt,sizeof(output));
        CHECK(UmiPixVerseTaskRecordResult(f.store,"scene-one",2U,f.plan,expected,&result,&output)==expected);
        CHECK(!memcmp(&output,&f.receipt,sizeof(output)));
        CHECK(UmiProviderTasksRead(f.store,"scene-one",&output)==UMI_STATUS_OK && output.phase==UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN);
        CHECK(f.calls==(expected==UMI_STATUS_CANCELLED?0U:1U));
    } else if(!strcmp(mode,"creation-binding")) {
        Begin(&f);UmiPixVersePlan *other=NULL;UmiPixVerseDraft draft=f.draft;
        strcpy(draft.prompt,"A different request");CHECK(UmiPixVersePrepare(&draft,&other)==UMI_STATUS_OK);
        UmiPixVerseResult result;CHECK(Execute(&f,other,&result)==UMI_STATUS_OK);
        ObserveFailure(&f,other,UMI_STATUS_OK,&result,UMI_STATUS_INVALID_STATE);UmiPixVerseDestroy(other);
    } else if(!strcmp(mode,"creation-flags")) {
        Begin(&f);UmiPixVerseResult result;CHECK(Execute(&f,f.plan,&result)==UMI_STATUS_OK);
        for(unsigned i=0U;i<6U;++i) {
            UmiPixVerseResult bad=result;
/* Make independent fixture statements explicit without weakening the existing checks. The former implementation is retained for owner review. */
#if 0
            if(i==0U) bad.creation_confirmed=false;if(i==1U) bad.creation_may_exist=false;
            if(i==2U) bad.video_id=0;if(i==3U) bad.http_status=201U;
            if(i==4U) bad.state=5U;if(i==5U) strcpy(bad.video_url,"private");
#endif
            if(i==0U) { bad.creation_confirmed=false; }
            if(i==1U) { bad.creation_may_exist=false; }
            if(i==2U) { bad.video_id=0; }
            if(i==3U) { bad.http_status=201U; }
            if(i==4U) { bad.state=5U; }
            if(i==5U) { strcpy(bad.video_url,"private"); }
            ObserveFailure(&f,f.plan,UMI_STATUS_OK,&bad,UMI_STATUS_INVALID_STATE);
        }
    } else if(!strcmp(mode,"store-transaction")) {
        Begin(&f);UmiPixVerseResult result;CHECK(Execute(&f,f.plan,&result)==UMI_STATUS_OK);
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
        CHECK(umi_data_server_begin(f.server)==UMI_STATUS_OK);UmiProviderTaskReceipt output=f.receipt;
#endif
        CHECK(umi_data_server_begin(f.server)==UMI_STATUS_OK);UmiProviderTaskReceipt output;memcpy(&output,&f.receipt,sizeof(output));
        CHECK(UmiPixVerseTaskRecordResult(f.store,"scene-one",2U,f.plan,UMI_STATUS_OK,&result,&output)==UMI_STATUS_BUSY);
        CHECK(!memcmp(&output,&f.receipt,sizeof(output)));CHECK(umi_data_server_rollback(f.server)==UMI_STATUS_OK);
        CHECK(UmiPixVerseTaskRecordResult(f.store,"scene-one",2U,f.plan,UMI_STATUS_OK,&result,&f.receipt)==UMI_STATUS_OK);
    } else if(!strcmp(mode,"revision-limit")) {
        Begin(&f);UmiPixVerseResult result;CHECK(Execute(&f,f.plan,&result)==UMI_STATUS_OK);
        f.receipt.revision=UINT64_MAX;ObserveFailure(&f,f.plan,UMI_STATUS_OK,&result,UMI_STATUS_CAPACITY_EXCEEDED);
    } else if(!strcmp(mode,"invalid-input")) {
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
        UmiProviderTaskReceipt output=f.receipt;UmiPixVerseResult result={0};
#endif
        UmiProviderTaskReceipt output;memcpy(&output,&f.receipt,sizeof(output));UmiPixVerseResult result={0};
        CHECK(UmiPixVerseTaskPrepare(NULL,"media","lesson","scene-one",&f.connection,7U,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPixVerseTaskPrepare(f.plan,"media","lesson","scene-one",&f.connection,0U,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPixVerseTaskObserve(NULL,1U,f.plan,UMI_STATUS_OK,&result,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPixVerseTaskObserve(&f.receipt,1U,f.plan,UMI_STATUS_OK,NULL,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPixVerseTaskRecordResult(NULL,"scene-one",1U,f.plan,UMI_STATUS_OK,&result,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&output,&f.receipt,sizeof(output)));
    } else {
        f.cancel_during=!strcmp(mode,"cancel-receipt");Accepted(&f);
        if(!strcmp(mode,"submission") || !strcmp(mode,"cancel-receipt")) {
            CHECK(f.calls==1U);CHECK(!f.cancel_during || umi_cancellation_token_is_requested(f.cancel));
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
            UmiProviderTaskReceipt output=f.receipt;
#endif
            UmiProviderTaskReceipt output;memcpy(&output,&f.receipt,sizeof(output));
            CHECK(UmiPixVerseTaskPrepare(f.plan,"media","lesson","new-id",&f.connection,7U,&output)==UMI_STATUS_INVALID_STATE);
            CHECK(!memcmp(&output,&f.receipt,sizeof(output)));
        } else if(!strcmp(mode,"duplicate") || !strcmp(mode,"stale")) {
            UmiPixVerseResult result={0};result.video_id=42;result.http_status=200U;
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
            result.creation_may_exist=true;result.creation_confirmed=true;UmiProviderTaskReceipt output=f.receipt;
#endif
            result.creation_may_exist=true;result.creation_confirmed=true;UmiProviderTaskReceipt output;memcpy(&output,&f.receipt,sizeof(output));
            bool stale=!strcmp(mode,"stale");
            CHECK(UmiPixVerseTaskRecordResult(f.store,"scene-one",stale?2U:3U,f.plan,UMI_STATUS_OK,&result,&output)==
                (stale?UMI_STATUS_BUSY:UMI_STATUS_OK));
/* A successful duplicate observation preserves the logical receipt; only a refused stale update promises the original output bytes. The former implementation is retained for owner review. */
#if 0
            CHECK(!memcmp(&output,&f.receipt,sizeof(output)) && f.calls==1U);
#endif
            if(stale) CHECK(!memcmp(&output,&f.receipt,sizeof(output)));
            else SameReceipt(&output,&f.receipt);
            CHECK(f.calls==1U);
        } else if(!strcmp(mode,"store-reopen")) {
            UmiProviderTasksDestroy(f.store);f.store=NULL;
            CHECK(UmiProviderTasksOpen(f.server,"media","lesson",&f.store)==UMI_STATUS_OK);
/* Reopening a canonical stored receipt reconstructs all fields but does not preserve compiler-specific padding. The former implementation is retained for owner review. */
#if 0
            UmiProviderTaskReceipt output;CHECK(UmiProviderTasksRead(f.store,"scene-one",&output)==UMI_STATUS_OK);
            CHECK(!memcmp(&output,&f.receipt,sizeof(output)));
#endif
            UmiProviderTaskReceipt output;CHECK(UmiProviderTasksRead(f.store,"scene-one",&output)==UMI_STATUS_OK);
            SameReceipt(&output,&f.receipt);
        } else if(!strcmp(mode,"recovery-binding")) {
            for(unsigned i=0U;i<5U;++i) {
                UmiProviderConnection connection=f.connection;UmiPixVersePlan *poll=NULL;
/* Make independent fixture statements explicit without weakening the existing checks. The former implementation is retained for owner review. */
#if 0
                if(i==0U) strcpy(connection.label,"Edited");if(i==1U) connection.enabled=false;
#endif
                if(i==0U) { strcpy(connection.label,"Edited"); }
                if(i==1U) { connection.enabled=false; }
                CHECK(UmiPixVerseTaskStatusPlan(&f.receipt,i==2U?"studio":"media","lesson",
                    &connection,i==3U?8U:7U,i==4U?"invalid":next_trace,&poll)!=UMI_STATUS_OK && poll==NULL);
            }
        } else if(!strcmp(mode,"recovery-id")) {
            const char *bad[]={"0","01","9223372036854775808","a42","42:sub"};
            for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i) {
                UmiProviderTaskReceipt receipt=f.receipt;strcpy(receipt.remote_id,bad[i]);UmiPixVersePlan *poll=NULL;
                CHECK(UmiPixVerseTaskStatusPlan(&receipt,"media","lesson",&f.connection,7U,next_trace,&poll)==UMI_STATUS_INVALID_STATE && !poll);
            }
            UmiProviderTaskReceipt receipt=f.receipt;strcpy(receipt.remote_id,"9223372036854775807");UmiPixVersePlan *poll=NULL;
            CHECK(UmiPixVerseTaskStatusPlan(&receipt,"media","lesson",&f.connection,7U,next_trace,&poll)==UMI_STATUS_OK);
            UmiPixVerseDestroy(poll);
        } else if(!strcmp(mode,"recovery-terminal")) {
            for(unsigned i=0U;i<2U;++i) {
                UmiProviderTaskReceipt receipt;
                CHECK(UmiProviderTaskAdvance(&f.receipt,3U,i?UMI_PROVIDER_TASK_OBSERVE_FAILURE:UMI_PROVIDER_TASK_OBSERVE_SUCCESS,&receipt)==UMI_STATUS_OK);
                UmiPixVersePlan *poll=NULL;
                CHECK(UmiPixVerseTaskStatusPlan(&receipt,"media","lesson",&f.connection,7U,next_trace,&poll)==UMI_STATUS_INVALID_STATE && !poll);
            }
        } else {
            UmiPixVersePlan *poll=NULL;
            CHECK(UmiPixVerseTaskStatusPlan(&f.receipt,"media","lesson",&f.connection,7U,next_trace,&poll)==UMI_STATUS_OK);
/* Capture the exact object representation before a refusal assertion; C structure assignment does not promise to copy padding. The former implementation is retained for owner review. */
#if 0
            UmiProviderTaskReceipt output=f.receipt;
#endif
            UmiProviderTaskReceipt output;memcpy(&output,&f.receipt,sizeof(output));
            CHECK(UmiPixVerseTaskPrepare(poll,"media","lesson","another",&f.connection,7U,&output)==UMI_STATUS_INVALID_STATE);
            CHECK(!memcmp(&output,&f.receipt,sizeof(output)));
            unsigned state=5U;UmiStatus expected=UMI_STATUS_OK;
            if(!strcmp(mode,"status-ready")) state=1U;
            else if(!strcmp(mode,"status-deleted")) state=6U;
            else if(!strcmp(mode,"status-moderated")) state=7U;
            else if(!strcmp(mode,"status-failed")) state=8U;
            else if(!strcmp(mode,"status-unknown")) { state=9U;expected=UMI_STATUS_PARSE_ERROR; }
            else if(!strcmp(mode,"status-mismatch")) expected=UMI_STATUS_PARSE_ERROR;
            else if(!strcmp(mode,"status-cancel")) { f.cancel_during=true;expected=UMI_STATUS_CANCELLED; }
            else CHECK(!strcmp(mode,"status-running") || !strcmp(mode,"status-consumed"));
            char reply[256];int count=snprintf(reply,sizeof(reply),
                "{\"ErrCode\":0,\"Resp\":{\"id\":%u,\"status\":%u,\"url\":\"https://video.example/clip?token=private\"}}",
                !strcmp(mode,"status-mismatch")?43U:42U,state);
            CHECK(count>0 && (size_t)count<sizeof(reply));f.reply=reply;
            UmiPixVerseResult result;CHECK(Execute(&f,poll,&result)==expected);
            CHECK(UmiPixVerseTaskRecordResult(f.store,"scene-one",3U,poll,expected,&result,&output)==expected);
            if(expected!=UMI_STATUS_OK) CHECK(!memcmp(&output,&f.receipt,sizeof(output)));
            else {
                CHECK(output.revision==4U && !strcmp(output.remote_id,"42"));
                CHECK(output.phase==(state==1U?UMI_PROVIDER_TASK_SUCCEEDED:state==5U?UMI_PROVIDER_TASK_RUNNING:UMI_PROVIDER_TASK_FAILED));
                CHECK(output.output_bytes==0U);
                CHECK(UmiPixVerseTaskRecordResult(f.store,"scene-one",4U,poll,UMI_STATUS_OK,&result,&output)==UMI_STATUS_OK && output.revision==4U);
                if(!strcmp(mode,"status-consumed")) {
                    CHECK(Execute(&f,poll,&result)==UMI_STATUS_INVALID_STATE);
                    CHECK(UmiPixVerseTaskRecordResult(f.store,"scene-one",4U,poll,UMI_STATUS_INVALID_STATE,&result,&output)==UMI_STATUS_INVALID_STATE);
                    CHECK(output.revision==4U && f.calls==2U);
                }
            }
            UmiPixVerseDestroy(poll);
        }
    }
    Close(&f);return 0;
}
