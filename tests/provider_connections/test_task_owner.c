/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_task_owner.c
 *
 * PURPOSE:
 *   Verify native task guard contention, stable scoped identities and persisted receipt freshness in isolated fixtures.
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
#include "umicom/provider_connections/task_owner.h"

static UmiProviderTaskReceipt prepared(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"video");strcpy(c.provider_id,"example");
    strcpy(c.label,"Example video");strcpy(c.endpoint,"https://api.example.invalid/tasks");
    strcpy(c.model,"video-model");c.route=UMI_PROVIDER_CONNECTION_HTTPS;c.timeout_ms=30000U;c.enabled=true;
    UmiProviderTaskContext context={0};strcpy(context.application_id,"media");strcpy(context.profile_id,"owner");
    strcpy(context.local_id,"scene-one");strcpy(context.operation_id,"video.generate");context.request_sha256[0]=42U;
    UmiProviderTaskReceipt r;CHECK(UmiProviderTaskPrepare(&context,&c,7U,&r)==UMI_STATUS_OK);return r;
}
static void path_refused(const char *root,const UmiProviderTaskReceipt *r,size_t capacity,UmiStatus expected)
{
    char out[UMI_PATH_CAPACITY],before[UMI_PATH_CAPACITY];
    memset(out,0xa5,sizeof(out));memcpy(before,out,sizeof(out));
    CHECK(UmiProviderTaskOwnerPath(root,r,out,capacity)==expected);
    CHECK(memcmp(out,before,sizeof(out))==0);
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    char root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY],other_path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    UmiDataServer *server=NULL;UmiProviderTasks *store=NULL,*other=NULL;
    CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    CHECK(UmiProviderTasksOpen(server,"media","owner",&store)==UMI_STATUS_OK);
    UmiProviderTaskReceipt r=prepared(),changed=r;
    CHECK(UmiProviderTaskOwnerPath(root,&r,path,sizeof(path))==UMI_STATUS_OK);
    CHECK(strstr(path,"umicom-provider-task-396a97524e386ad468ed410d478b86993f2ecadd74578e9f60dfd2dc78ec0c62.lock")!=NULL);
    UmiProviderTaskOwner *first=NULL,*second=NULL;
    if(strcmp(mode,"path-stable")==0) {
        changed.revision=UINT64_MAX;changed.connection_revision=123U;changed.context.request_sha256[0]=9U;
        strcpy(changed.connection_id,"another");
        CHECK(UmiProviderTaskOwnerPath(root,&changed,other_path,sizeof(other_path))==UMI_STATUS_OK);
        CHECK(strcmp(path,other_path)==0);
        changed=r;CHECK(UmiProviderTaskAdvance(&changed,changed.revision,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&changed)==UMI_STATUS_OK);
        CHECK(UmiProviderTaskConfirm(&changed,changed.revision,"Remote-123",&changed)==UMI_STATUS_OK);
        CHECK(UmiProviderTaskOwnerPath(root,&changed,other_path,sizeof(other_path))==UMI_STATUS_OK && strcmp(path,other_path)==0);
        char trailing[UMI_PATH_CAPACITY];CHECK(snprintf(trailing,sizeof(trailing),"%s/",root)>0);
        CHECK(UmiProviderTaskOwnerPath(trailing,&r,other_path,sizeof(other_path))==UMI_STATUS_OK && strcmp(path,other_path)==0);
    } else if(strcmp(mode,"path-scope")==0) {
        char prior[3][UMI_PATH_CAPACITY];
        for(unsigned i=0U;i<3U;++i) {
            changed=r;
            if(i==0U) strcpy(changed.context.application_id,"studio");
            if(i==1U) strcpy(changed.context.profile_id,"second");
            if(i==2U) strcpy(changed.context.local_id,"scene-two");
            CHECK(UmiProviderTaskOwnerPath(root,&changed,prior[i],sizeof(prior[i]))==UMI_STATUS_OK);
            CHECK(strcmp(path,prior[i])!=0);
            for(unsigned j=0U;j<i;++j) CHECK(strcmp(prior[i],prior[j])!=0);
        }
    } else if(strcmp(mode,"path-boundaries")==0) {
        size_t length=strlen(path);path_refused(root,&r,length,UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiProviderTaskOwnerPath(root,&r,other_path,length+1U)==UMI_STATUS_OK && strcmp(path,other_path)==0);
        path_refused(root,&r,0U,UMI_STATUS_CAPACITY_EXCEEDED);
        path_refused("relative",&r,sizeof(path),UMI_STATUS_INVALID_ARGUMENT);
        path_refused("",&r,sizeof(path),UMI_STATUS_INVALID_ARGUMENT);
        path_refused(NULL,&r,sizeof(path),UMI_STATUS_INVALID_ARGUMENT);
        path_refused(root,NULL,sizeof(path),UMI_STATUS_INVALID_ARGUMENT);
        changed.revision=0U;path_refused(root,&changed,sizeof(path),UMI_STATUS_INVALID_ARGUMENT);
        char long_root[UMI_PATH_CAPACITY];memset(long_root,'a',sizeof(long_root));
        path_refused(long_root,&r,sizeof(path),UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiProviderTaskOwnerPath(root,&r,NULL,sizeof(path))==UMI_STATUS_INVALID_ARGUMENT);
    } else if(strcmp(mode,"missing-record")==0) {
        CHECK(UmiProviderTaskOwnerAcquire(store,root,&r,&first)==UMI_STATUS_NOT_FOUND && first==NULL);
        CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_OK);
        CHECK(UmiProviderTaskOwnerAcquire(store,root,&r,&first)==UMI_STATUS_OK);
    } else {
        CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_OK);
        if(strcmp(mode,"missing-directory")==0) {
            FixturePath(other_path,root,"missing");
            CHECK(UmiProviderTaskOwnerAcquire(store,other_path,&r,&first)==UMI_STATUS_NOT_FOUND && first==NULL);
        } else if(strcmp(mode,"stale-acquire")==0) {
            CHECK(UmiProviderTasksAdvance(store,r.context.local_id,r.revision,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&changed)==UMI_STATUS_OK);
            CHECK(UmiProviderTaskOwnerAcquire(store,root,&r,&first)==UMI_STATUS_BUSY && first==NULL);
            CHECK(UmiProviderTaskOwnerAcquire(store,root,&changed,&first)==UMI_STATUS_OK);
        } else if(strcmp(mode,"changed-content")==0) {
            /* Simulate an outside writer retaining a revision. Canonical value
             * checks must detect substitution rather than trusting revision alone. */
            changed.context.request_sha256[0]=9U;
            char wire[UMI_PROVIDER_TASK_WIRE_CAPACITY];size_t length;
            CHECK(UmiProviderTaskEncode(&changed,wire,sizeof(wire),&length)==UMI_STATUS_OK);
            CHECK(umi_data_server_set(server,"provider.tasks/media/owner/scene-one",wire)==UMI_STATUS_OK);
            CHECK(UmiProviderTaskOwnerAcquire(store,root,&r,&first)==UMI_STATUS_BUSY && first==NULL);
            CHECK(UmiProviderTaskOwnerAcquire(store,root,&changed,&first)==UMI_STATUS_OK);
        } else {
            if(strcmp(mode,"existing-guard")==0) {
                UmiOutputFile *file=NULL;CHECK(UmiOutputFileCreate(path,&file)==UMI_STATUS_OK);
                CHECK(UmiOutputFileWrite(file,"keep\0guard",10U)==UMI_STATUS_OK);UmiOutputFileDestroy(file);
            }
            CHECK(UmiProviderTaskOwnerAcquire(store,root,&r,&first)==UMI_STATUS_OK);
            CHECK(UmiProviderTaskOwnerCheck(first,&r)==UMI_STATUS_OK);
            if(strcmp(mode,"lifecycle")==0) {
                CHECK(UmiProviderTasksAdvance(store,r.context.local_id,r.revision,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&r)==UMI_STATUS_OK);
                CHECK(UmiProviderTaskOwnerCheck(first,&r)==UMI_STATUS_OK);
                CHECK(UmiProviderTasksConfirm(store,r.context.local_id,r.revision,"Remote-123",&r)==UMI_STATUS_OK);
                CHECK(UmiProviderTaskOwnerCheck(first,&r)==UMI_STATUS_OK);
                CHECK(UmiProviderTasksAdvance(store,r.context.local_id,r.revision,UMI_PROVIDER_TASK_OBSERVE_SUCCESS,&r)==UMI_STATUS_OK);
                CHECK(UmiProviderTaskOwnerCheck(first,&r)==UMI_STATUS_OK);
                unsigned char digest[UMI_SHA256_BYTES]={9U};
                CHECK(UmiProviderTasksRecordOutput(store,r.context.local_id,r.revision,42U,digest,&r)==UMI_STATUS_OK);
                CHECK(UmiProviderTaskOwnerCheck(first,&r)==UMI_STATUS_OK);
            } else if(strcmp(mode,"contention")==0) {
                CHECK(UmiProviderTasksOpen(server,"media","owner",&other)==UMI_STATUS_OK);
                CHECK(UmiProviderTaskOwnerAcquire(other,root,&r,&second)==UMI_STATUS_BUSY && second==NULL);
                CHECK(UmiProviderTaskOwnerRelease(&first)==UMI_STATUS_OK && first==NULL);
                CHECK(UmiProviderTaskOwnerAcquire(other,root,&r,&second)==UMI_STATUS_OK);
            } else if(strcmp(mode,"distinct-task")==0 || strcmp(mode,"distinct-scope")==0) {
                bool scope=strcmp(mode,"distinct-scope")==0;
                if(scope) strcpy(changed.context.profile_id,"second");else strcpy(changed.context.local_id,"scene-two");
                CHECK(UmiProviderTasksOpen(server,"media",scope?"second":"owner",&other)==UMI_STATUS_OK);
                CHECK(UmiProviderTasksCreate(other,&changed)==UMI_STATUS_OK);
                CHECK(UmiProviderTaskOwnerAcquire(other,root,&changed,&second)==UMI_STATUS_OK);
                CHECK(UmiProviderTaskOwnerCheck(first,&r)==UMI_STATUS_OK && UmiProviderTaskOwnerCheck(second,&changed)==UMI_STATUS_OK);
            } else if(strcmp(mode,"stale-check")==0) {
                CHECK(UmiProviderTasksAdvance(store,r.context.local_id,r.revision,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&changed)==UMI_STATUS_OK);
                CHECK(UmiProviderTaskOwnerCheck(first,&r)==UMI_STATUS_BUSY);
                CHECK(UmiProviderTaskOwnerAcquire(store,root,&changed,&second)==UMI_STATUS_BUSY && second==NULL);
                CHECK(UmiProviderTaskOwnerCheck(first,&changed)==UMI_STATUS_OK);
            } else if(strcmp(mode,"wrong-scope")==0) {
                for(unsigned i=0U;i<3U;++i) {
                    changed=r;
                    if(i==0U) strcpy(changed.context.application_id,"studio");
                    if(i==1U) strcpy(changed.context.profile_id,"second");
                    if(i==2U) strcpy(changed.context.local_id,"scene-two");
                    CHECK(UmiProviderTaskOwnerCheck(first,&changed)==UMI_STATUS_INVALID_ARGUMENT);
                }
            } else if(strcmp(mode,"invalid")==0) {
                UmiProviderTaskOwner *saved=first;
                CHECK(UmiProviderTaskOwnerAcquire(store,root,&r,&first)==UMI_STATUS_INVALID_ARGUMENT && first==saved);
                CHECK(UmiProviderTaskOwnerAcquire(NULL,root,&r,&second)==UMI_STATUS_INVALID_ARGUMENT && second==NULL);
                CHECK(UmiProviderTaskOwnerAcquire(store,root,&r,NULL)==UMI_STATUS_INVALID_ARGUMENT);
                CHECK(UmiProviderTaskOwnerCheck(NULL,&r)==UMI_STATUS_INVALID_ARGUMENT);
                CHECK(UmiProviderTaskOwnerCheck(first,NULL)==UMI_STATUS_INVALID_ARGUMENT);
                CHECK(UmiProviderTaskOwnerRelease(NULL)==UMI_STATUS_INVALID_ARGUMENT);
            } else CHECK(strcmp(mode,"existing-guard")==0);
        }
    }
    CHECK(UmiProviderTaskOwnerRelease(&first)==UMI_STATUS_OK && first==NULL);
    CHECK(UmiProviderTaskOwnerRelease(&second)==UMI_STATUS_OK && second==NULL);
    CHECK(UmiProviderTaskOwnerRelease(&first)==UMI_STATUS_OK);
    if(strcmp(mode,"existing-guard")==0) {
        size_t length;unsigned char *bytes=FixtureRead(path,&length);
        CHECK(length==10U && memcmp(bytes,"keep\0guard",10U)==0);free(bytes);
    }
    UmiProviderTasksDestroy(other);UmiProviderTasksDestroy(store);umi_data_server_destroy(server);return 0;
}
