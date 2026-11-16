/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_task_store.c
 *
 * PURPOSE:
 *   Verify scoped task persistence, restart recovery and transaction failure preservation with isolated synthetic records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "../../src/provider_connections/internal.h"
#include "umicom/provider_connections/task_store.h"
static const char task_key_fixture[]="provider.tasks/media/owner/scene-one";
static UmiProviderTaskReceipt prepared(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"video");strcpy(c.provider_id,"example");
    strcpy(c.label,"Example video");strcpy(c.endpoint,"https://api.example.invalid/tasks");
    strcpy(c.model,"video-model");c.route=UMI_PROVIDER_CONNECTION_HTTPS;c.timeout_ms=30000U;c.enabled=true;
    UmiProviderTaskContext context={0};strcpy(context.application_id,"media");strcpy(context.profile_id,"owner");
    strcpy(context.local_id,"scene-one");strcpy(context.operation_id,"video.generate");context.request_sha256[0]=42U;
    UmiProviderTaskReceipt r;CHECK(UmiProviderTaskPrepare(&context,&c,7U,&r)==UMI_STATUS_OK);return r;
}
static void seed_uncertain(UmiProviderTasks *store,UmiProviderTaskReceipt *r)
{
    CHECK(UmiProviderTasksCreate(store,r)==UMI_STATUS_OK);
    CHECK(UmiProviderTasksAdvance(store,r->context.local_id,1U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,r)==UMI_STATUS_OK);
    CHECK(r->revision==2U && r->phase==UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN);
}
static void same(const UmiProviderTaskReceipt *r,const unsigned char *before)
{ CHECK(memcmp(r,before,sizeof(*r))==0); }
static void reopen(const char *path,UmiDataServer **server,UmiProviderTasks **store)
{
    UmiProviderTasksDestroy(*store);umi_data_server_destroy(*server);*store=NULL;*server=NULL;
    CHECK(umi_data_server_create_sqlite(path,server)==UMI_STATUS_OK);
    CHECK(UmiProviderTasksOpen(*server,"media","owner",store)==UMI_STATUS_OK);
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];bool sqlite=strncmp(mode,"sqlite-",7U)==0;
    UmiDataServer *server=NULL;UmiProviderTasks *store=NULL,*other=NULL;
    char path[UMI_PATH_CAPACITY]={0};
    if(sqlite) {
        char root[UMI_PATH_CAPACITY];FixtureDirectory(root);FixturePath(path,root,"provider-tasks.sqlite3");
        UmiStatus status=umi_data_server_create_sqlite(path,&server);
        if(status==UMI_STATUS_UNAVAILABLE) { fprintf(stderr,"SKIP: SQLite adapter unavailable.\n");return 77; }
        CHECK(status==UMI_STATUS_OK);
    } else CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    CHECK(UmiProviderTasksOpen(server,"media","owner",&store)==UMI_STATUS_OK);
    UmiProviderTaskReceipt r=prepared(),out;memset(&out,0xa5,sizeof(out));
    unsigned char before[sizeof(out)];memcpy(before,&out,sizeof(out));
    if(strcmp(mode,"lifecycle")==0) {
        seed_uncertain(store,&r);
        CHECK(UmiProviderTasksConfirm(store,"scene-one",2U,"Remote-123",&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksAdvance(store,"scene-one",3U,UMI_PROVIDER_TASK_OBSERVE_RUNNING,&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksAdvance(store,"scene-one",4U,UMI_PROVIDER_TASK_OBSERVE_SUCCESS,&r)==UMI_STATUS_OK);
        unsigned char digest[UMI_SHA256_BYTES]={9U};
        CHECK(UmiProviderTasksRecordOutput(store,"scene-one",5U,42U,digest,&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksRead(store,"scene-one",&out)==UMI_STATUS_OK);
        CHECK(out.revision==6U && out.phase==UMI_PROVIDER_TASK_OUTPUT_STORED && out.output_bytes==42U);
        CHECK(out.output_sha256[0]==9U && strcmp(out.remote_id,"Remote-123")==0);
        memcpy(before,&out,sizeof(out));
        CHECK(UmiProviderTasksAdvance(store,"scene-one",6U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&out)==UMI_STATUS_INVALID_STATE);same(&out,before);
    } else if(strcmp(mode,"duplicate")==0) {
        CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_ALREADY_EXISTS);
        r.context.request_sha256[0]=43U;CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiProviderTasksRead(store,"scene-one",&out)==UMI_STATUS_OK);
        CHECK(out.revision==1U && out.context.request_sha256[0]==42U);
    } else if(strcmp(mode,"missing")==0) {
        CHECK(UmiProviderTasksRead(store,"scene-one",&out)==UMI_STATUS_NOT_FOUND);same(&out,before);
        CHECK(UmiProviderTasksAdvance(store,"scene-one",1U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&out)==UMI_STATUS_NOT_FOUND);same(&out,before);
        CHECK(UmiProviderTasksConfirm(store,"scene-one",1U,"Remote-123",&out)==UMI_STATUS_NOT_FOUND);same(&out,before);
    } else if(strcmp(mode,"scope")==0) {
        CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksOpen(server,"studio","owner",&other)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksRead(other,"scene-one",&out)==UMI_STATUS_NOT_FOUND);same(&out,before);
        CHECK(UmiProviderTasksCreate(other,&r)==UMI_STATUS_INVALID_ARGUMENT);
        strcpy(r.context.application_id,"studio");CHECK(UmiProviderTasksCreate(other,&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksRead(other,"scene-one",&out)==UMI_STATUS_OK);
        CHECK(strcmp(out.context.application_id,"studio")==0);
    } else if(strcmp(mode,"stale")==0) {
        seed_uncertain(store,&r);CHECK(UmiProviderTasksOpen(server,"media","owner",&other)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksConfirm(other,"scene-one",2U,"Remote-123",&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksConfirm(store,"scene-one",2U,"Remote-456",&out)==UMI_STATUS_BUSY);same(&out,before);
        CHECK(UmiProviderTasksRead(store,"scene-one",&r)==UMI_STATUS_OK);
        CHECK(r.revision==3U && strcmp(r.remote_id,"Remote-123")==0);
    } else if(strcmp(mode,"transaction")==0) {
        CHECK(umi_data_server_begin(server)==UMI_STATUS_OK);
        CHECK(umi_data_server_set(server,"caller-owned","keep")==UMI_STATUS_OK);
        CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_BUSY);
        CHECK(UmiProviderTasksRead(store,"scene-one",&out)==UMI_STATUS_BUSY);same(&out,before);
        CHECK(UmiProviderTasksAdvance(store,"scene-one",1U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&out)==UMI_STATUS_BUSY);same(&out,before);
        CHECK(umi_data_server_in_transaction(server));CHECK(umi_data_server_commit(server)==UMI_STATUS_OK);
        char value[16];CHECK(umi_data_server_get(server,"caller-owned",value,sizeof(value))==UMI_STATUS_OK);
        CHECK(strcmp(value,"keep")==0);CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_OK);
    } else if(strcmp(mode,"corrupt")==0) {
        const char *records[]={"broken","provider-task\n2\n"};
        for(size_t i=0U;i<sizeof(records)/sizeof(records[0]);++i) {
            CHECK(umi_data_server_set(server,task_key_fixture,records[i])==UMI_STATUS_OK);
            UmiStatus expected=i==0U?UMI_STATUS_PARSE_ERROR:UMI_STATUS_NOT_IMPLEMENTED;
            CHECK(UmiProviderTasksRead(store,"scene-one",&out)==expected);same(&out,before);
            CHECK(UmiProviderTasksAdvance(store,"scene-one",1U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&out)==expected);same(&out,before);
            CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_ALREADY_EXISTS);
            char kept[64];CHECK(umi_data_server_get(server,task_key_fixture,kept,sizeof(kept))==UMI_STATUS_OK);
            CHECK(strcmp(kept,records[i])==0);
        }
    } else if(strcmp(mode,"identity")==0) {
        for(unsigned field=0U;field<3U;++field) {
            r=prepared();if(field==0U) strcpy(r.context.application_id,"studio");
            else if(field==1U) strcpy(r.context.profile_id,"other");else strcpy(r.context.local_id,"other");
            char wire[UMI_PROVIDER_TASK_WIRE_CAPACITY];size_t length;
            CHECK(UmiProviderTaskEncode(&r,wire,sizeof(wire),&length)==UMI_STATUS_OK);
            CHECK(umi_data_server_set(server,task_key_fixture,wire)==UMI_STATUS_OK);
            CHECK(UmiProviderTasksRead(store,"scene-one",&out)==UMI_STATUS_PARSE_ERROR);same(&out,before);
        }
    } else if(strcmp(mode,"borrowed-inputs")==0) {
        seed_uncertain(store,&r);out=r;strcpy(out.remote_id,"Remote-123");
        CHECK(UmiProviderTasksConfirm(store,out.context.local_id,2U,out.remote_id,&out)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksAdvance(store,out.context.local_id,3U,UMI_PROVIDER_TASK_OBSERVE_SUCCESS,&out)==UMI_STATUS_OK);
        out.output_sha256[0]=9U;
        CHECK(UmiProviderTasksRecordOutput(store,out.context.local_id,4U,42U,out.output_sha256,&out)==UMI_STATUS_OK);
        CHECK(out.revision==5U && out.output_sha256[0]==9U);
        CHECK(UmiProviderTasksRead(store,out.context.local_id,&out)==UMI_STATUS_OK && out.revision==5U);
    } else if(strcmp(mode,"invalid-input")==0) {
        UmiProviderTasks *unchanged=store;
        CHECK(UmiProviderTasksOpen(NULL,"media","owner",&unchanged)==UMI_STATUS_INVALID_ARGUMENT && unchanged==store);
        CHECK(UmiProviderTasksOpen(server,"bad/scope","owner",&unchanged)==UMI_STATUS_INVALID_ARGUMENT && unchanged==store);
        CHECK(UmiProviderTasksOpen(server,"media","owner",NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderTasksCreate(NULL,&r)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderTasksCreate(store,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        r.revision=2U;CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_INVALID_ARGUMENT);
        r=prepared();r.phase=UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN;CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderTasksRead(NULL,"scene-one",&out)==UMI_STATUS_INVALID_ARGUMENT);same(&out,before);
        CHECK(UmiProviderTasksRead(store,NULL,&out)==UMI_STATUS_INVALID_ARGUMENT);same(&out,before);
        CHECK(UmiProviderTasksRead(store,"scene-one",NULL)==UMI_STATUS_INVALID_ARGUMENT);
        UmiProviderTasksDestroy(NULL);
    } else if(strcmp(mode,"boundaries")==0) {
        char id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];memset(id,'a',sizeof(id)-1U);id[sizeof(id)-1U]='\0';
        CHECK(UmiProviderTasksOpen(server,id,id,&other)==UMI_STATUS_OK);
        strcpy(r.context.application_id,id);strcpy(r.context.profile_id,id);strcpy(r.context.local_id,id);
        CHECK(UmiProviderTasksCreate(other,&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksRead(other,id,&out)==UMI_STATUS_OK && strcmp(out.context.local_id,id)==0);
    } else if(strcmp(mode,"sqlite-reopen")==0) {
        seed_uncertain(store,&r);reopen(path,&server,&store);
        CHECK(UmiProviderTasksRead(store,"scene-one",&out)==UMI_STATUS_OK && out.phase==UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN);
        CHECK(UmiProviderTasksConfirm(store,"scene-one",2U,"Remote-123",&out)==UMI_STATUS_OK);
        reopen(path,&server,&store);
        CHECK(UmiProviderTasksRead(store,"scene-one",&out)==UMI_STATUS_OK);
        CHECK(out.revision==3U && strcmp(out.remote_id,"Remote-123")==0);
    } else if(strcmp(mode,"sqlite-failed-write")==0 || strcmp(mode,"sqlite-lost-transaction")==0) {
        seed_uncertain(store,&r);bool lost=strcmp(mode,"sqlite-lost-transaction")==0;
        const char *trigger=lost?
            "CREATE TRIGGER reject_task BEFORE INSERT ON umicom_kv WHEN NEW.key='provider.tasks/media/owner/scene-one' BEGIN SELECT RAISE(ROLLBACK,'fixture abort'); END;":
            "CREATE TRIGGER reject_task BEFORE INSERT ON umicom_kv WHEN NEW.key='provider.tasks/media/owner/scene-one' BEGIN SELECT RAISE(ABORT,'fixture rejection'); END;";
        CHECK(umi_data_server_execute(server,trigger)==UMI_STATUS_OK);
        UmiStatus status=UmiProviderTasksConfirm(store,"scene-one",2U,"Remote-123",&out);
        CHECK(status==(lost?UMI_STATUS_INVALID_STATE:UMI_STATUS_IO_ERROR));same(&out,before);
        if(lost) {
            CHECK(UmiProviderTasksRead(store,"scene-one",&out)==UMI_STATUS_INVALID_STATE);same(&out,before);
            reopen(path,&server,&store);
        }
        CHECK(umi_data_server_execute(server,"DROP TRIGGER reject_task;")==UMI_STATUS_OK);
        CHECK(UmiProviderTasksRead(store,"scene-one",&out)==UMI_STATUS_OK);
        CHECK(out.revision==2U && out.phase==UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN);
        CHECK(UmiProviderTasksConfirm(store,"scene-one",2U,"Remote-123",&out)==UMI_STATUS_OK);
    } else if(strcmp(mode,"sqlite-separate-server")==0) {
        seed_uncertain(store,&r);UmiDataServer *second=NULL;
        CHECK(umi_data_server_create_sqlite(path,&second)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksOpen(second,"media","owner",&other)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksRead(other,"scene-one",&r)==UMI_STATUS_OK && r.revision==2U);
        CHECK(UmiProviderTasksConfirm(store,"scene-one",2U,"Remote-123",&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksConfirm(other,"scene-one",2U,"Remote-456",&out)==UMI_STATUS_BUSY);same(&out,before);
        UmiProviderTasksDestroy(other);other=NULL;umi_data_server_destroy(second);
    } else if(strcmp(mode,"sqlite-noop")==0) {
        seed_uncertain(store,&r);CHECK(UmiProviderTasksConfirm(store,"scene-one",2U,"Remote-123",&r)==UMI_STATUS_OK);
        CHECK(umi_data_server_execute(server,"CREATE TRIGGER reject_task BEFORE INSERT ON umicom_kv BEGIN SELECT RAISE(ABORT,'no write allowed'); END;")==UMI_STATUS_OK);
        CHECK(UmiProviderTasksConfirm(store,"scene-one",3U,"Remote-123",&out)==UMI_STATUS_OK && out.revision==3U);
        memcpy(before,&out,sizeof(out));
        CHECK(UmiProviderTasksConfirm(store,"scene-one",3U,"Remote-456",&out)==UMI_STATUS_INVALID_STATE);same(&out,before);
    } else if(strcmp(mode,"sqlite-readonly")==0) {
        seed_uncertain(store,&r);UmiDataServer *readonly=NULL;
        CHECK(umi_data_server_open_sqlite_readonly(path,&readonly)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksOpen(readonly,"media","owner",&other)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksRead(other,"scene-one",&r)==UMI_STATUS_OK && r.revision==2U);
        CHECK(UmiProviderTasksConfirm(other,"scene-one",2U,"Remote-123",&out)==UMI_STATUS_PERMISSION_DENIED);same(&out,before);
        UmiProviderTasksDestroy(other);other=NULL;umi_data_server_destroy(readonly);
    } else { UmiProviderTasksDestroy(store);umi_data_server_destroy(server);return 2; }
    UmiProviderTasksDestroy(other);UmiProviderTasksDestroy(store);umi_data_server_destroy(server);return 0;
}
