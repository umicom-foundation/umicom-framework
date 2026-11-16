/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_task_catalogue.c
 *
 * PURPOSE:
 *   Verify complete scoped recovery pages, stable ordering and refusal of stale or partial task discovery.
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
static UmiProviderTaskReceipt add_task(UmiProviderTasks *store,const char *id)
{
    UmiProviderConnection c={0};strcpy(c.id,"local");strcpy(c.provider_id,"example");
    strcpy(c.label,"Example");strcpy(c.endpoint,"http://127.0.0.1:8080/generate");
    c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;c.timeout_ms=30000U;c.enabled=true;
    UmiProviderTaskContext context={0};strcpy(context.application_id,"media");strcpy(context.profile_id,"owner");
    strcpy(context.local_id,id);strcpy(context.operation_id,"video.generate");context.request_sha256[0]=42U;
    UmiProviderTaskReceipt r;CHECK(UmiProviderTaskPrepare(&context,&c,7U,&r)==UMI_STATUS_OK);
    CHECK(UmiProviderTasksCreate(store,&r)==UMI_STATUS_OK);return r;
}
static void three(UmiProviderTasks *store)
{ (void)add_task(store,"charlie");(void)add_task(store,"alpha");(void)add_task(store,"bravo"); }
static void next(UmiProviderTaskQuery *query,const UmiProviderTaskPage *page)
{ strcpy(query->after_id,page->next_after_id);strcpy(query->expected_snapshot,page->snapshot); }
static void failed(UmiProviderTasks *store,const UmiProviderTaskQuery *query,UmiStatus expected)
{
    UmiProviderTaskPage page;memset(&page,0xa5,sizeof(page));unsigned char before[sizeof(page)];memcpy(before,&page,sizeof(page));
    CHECK(UmiProviderTasksReadPage(store,query,&page)==expected);CHECK(memcmp(&page,before,sizeof(page))==0);
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];bool sqlite=strncmp(mode,"sqlite-",7U)==0;
    UmiDataServer *server=NULL;UmiProviderTasks *store=NULL;char path[UMI_PATH_CAPACITY]={0};
    if(sqlite) {
        char root[UMI_PATH_CAPACITY];FixtureDirectory(root);FixturePath(path,root,"task-catalogue.sqlite3");
        UmiStatus status=umi_data_server_create_sqlite(path,&server);
        if(status==UMI_STATUS_UNAVAILABLE) { fprintf(stderr,"SKIP: SQLite adapter unavailable.\n");return 77; }
        CHECK(status==UMI_STATUS_OK);
    } else CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    CHECK(UmiProviderTasksOpen(server,"media","owner",&store)==UMI_STATUS_OK);
    UmiProviderTaskQuery query={0};query.limit=2U;UmiProviderTaskPage page;
    if(strcmp(mode,"empty")==0) {
        CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK);
        CHECK(page.count==0U && page.matching_count==0U && page.scope_count==0U && page.next_after_id[0]=='\0');
        CHECK(strcmp(page.snapshot,"e862e046b0313c017e9e0812476d1337575b196c917b77b887b3e08ef39ad6eb")==0);
    } else if(strcmp(mode,"order")==0 || strcmp(mode,"sqlite-pages")==0) {
        three(store);CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK);
        CHECK(page.count==2U && page.scope_count==3U && page.matching_count==3U);
        CHECK(strcmp(page.items[0].context.local_id,"alpha")==0 && strcmp(page.items[1].context.local_id,"bravo")==0);
        CHECK(strcmp(page.next_after_id,"bravo")==0);next(&query,&page);query.limit=1U;
        CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK);
        CHECK(page.count==1U && strcmp(page.items[0].context.local_id,"charlie")==0 && page.next_after_id[0]=='\0');
        strcpy(query.after_id,"charlie");CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK && page.count==0U);
    } else if(strcmp(mode,"filter")==0) {
        three(store);UmiProviderTaskReceipt r;
        CHECK(UmiProviderTasksAdvance(store,"bravo",1U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&r)==UMI_STATUS_OK);
        query.phase=UMI_PROVIDER_TASK_SUBMISSION_UNCERTAIN;
        CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK);
        CHECK(page.count==1U && page.matching_count==1U && page.scope_count==3U);
        CHECK(strcmp(page.items[0].context.local_id,"bravo")==0);
    } else if(strcmp(mode,"stale")==0 || strcmp(mode,"filter-stale")==0) {
        three(store);CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK);next(&query,&page);
        if(strcmp(mode,"stale")==0) {
            UmiProviderTaskReceipt r;CHECK(UmiProviderTasksAdvance(store,"charlie",1U,UMI_PROVIDER_TASK_BEGIN_SUBMISSION,&r)==UMI_STATUS_OK);
        } else query.phase=UMI_PROVIDER_TASK_PREPARED;
        failed(store,&query,UMI_STATUS_BUSY);
    } else if(strcmp(mode,"cursor")==0) {
        three(store);CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK);next(&query,&page);
        strcpy(query.after_id,"missing");failed(store,&query,UMI_STATUS_INVALID_STATE);
    } else if(strcmp(mode,"scope")==0) {
        three(store);CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK);next(&query,&page);
        CHECK(umi_data_server_set(server,"provider.tasks/media/another/damaged","bad")==UMI_STATUS_OK);
        CHECK(umi_data_server_set(server,"unrelated","bad")==UMI_STATUS_OK);
        CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK);
        CHECK(page.scope_count==3U && page.count==1U && strcmp(page.items[0].context.local_id,"charlie")==0);
    } else if(strcmp(mode,"corrupt")==0 || strcmp(mode,"schema")==0 || strcmp(mode,"malformed-key")==0) {
        three(store);query.phase=UMI_PROVIDER_TASK_RUNNING;
        const char *key=strcmp(mode,"malformed-key")==0?"provider.tasks/media/owner/invalid/key":"provider.tasks/media/owner/zulu";
        const char *value=strcmp(mode,"schema")==0?"provider-task\n2\n":"bad";
        CHECK(umi_data_server_set(server,key,value)==UMI_STATUS_OK);
        failed(store,&query,strcmp(mode,"schema")==0?UMI_STATUS_NOT_IMPLEMENTED:UMI_STATUS_PARSE_ERROR);
    } else if(strcmp(mode,"identity")==0) {
        UmiProviderTaskReceipt r=add_task(store,"alpha");strcpy(r.context.profile_id,"another");
        char wire[UMI_PROVIDER_TASK_WIRE_CAPACITY];size_t length;
        CHECK(UmiProviderTaskEncode(&r,wire,sizeof(wire),&length)==UMI_STATUS_OK);
        CHECK(umi_data_server_set(server,"provider.tasks/media/owner/alpha",wire)==UMI_STATUS_OK);
        failed(store,&query,UMI_STATUS_PARSE_ERROR);
    } else if(strcmp(mode,"catalogue-limit")==0) {
        for(unsigned i=0U;i<UMI_PROVIDER_TASK_CATALOGUE_LIMIT;++i) {
            char id[32];CHECK(snprintf(id,sizeof(id),"task-%03u",i)>0);(void)add_task(store,id);
        }
        CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK && page.scope_count==UMI_PROVIDER_TASK_CATALOGUE_LIMIT);
        (void)add_task(store,"overflow");failed(store,&query,UMI_STATUS_CAPACITY_EXCEEDED);
    } else if(strcmp(mode,"invalid-query")==0) {
        for(unsigned variant=0U;variant<9U;++variant) {
            UmiProviderTaskQuery bad={0};bad.limit=2U;
            switch(variant) {
                case 0:bad.limit=0U;break;case 1:bad.limit=UMI_PROVIDER_TASK_PAGE_CAPACITY+1U;break;
                case 2:bad.phase=(UmiProviderTaskPhase)8;break;
                case 3:strcpy(bad.after_id,"alpha");break;
                case 4:memset(bad.expected_snapshot,'0',64U);break;
                case 5:strcpy(bad.after_id,"alpha");strcpy(bad.expected_snapshot,"not-a-digest");break;
                case 6:memset(bad.after_id,'a',sizeof(bad.after_id));memset(bad.expected_snapshot,'0',64U);break;
                case 7:strcpy(bad.after_id,"alpha");memset(bad.expected_snapshot,'0',sizeof(bad.expected_snapshot));break;
                case 8:strcpy(bad.after_id,"alpha");memset(bad.expected_snapshot,'A',64U);break;
            }
            failed(store,&bad,UMI_STATUS_INVALID_ARGUMENT);
        }
        failed(NULL,&query,UMI_STATUS_INVALID_ARGUMENT);failed(store,NULL,UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderTasksReadPage(store,&query,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(strcmp(mode,"transaction")==0) {
        CHECK(umi_data_server_begin(server)==UMI_STATUS_OK);CHECK(umi_data_server_set(server,"caller-owned","keep")==UMI_STATUS_OK);
        failed(store,&query,UMI_STATUS_BUSY);CHECK(umi_data_server_in_transaction(server));
        CHECK(umi_data_server_commit(server)==UMI_STATUS_OK);
        char value[16];CHECK(umi_data_server_get(server,"caller-owned",value,sizeof(value))==UMI_STATUS_OK && strcmp(value,"keep")==0);
    } else if(strcmp(mode,"insertion-order")==0) {
        three(store);CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK);char snapshot[65];strcpy(snapshot,page.snapshot);
        UmiDataServer *second=NULL;UmiProviderTasks *other=NULL;
        CHECK(umi_data_server_create_memory(&second)==UMI_STATUS_OK);CHECK(UmiProviderTasksOpen(second,"media","owner",&other)==UMI_STATUS_OK);
        (void)add_task(other,"bravo");(void)add_task(other,"charlie");(void)add_task(other,"alpha");
        CHECK(UmiProviderTasksReadPage(other,&query,&page)==UMI_STATUS_OK && strcmp(page.snapshot,snapshot)==0);
        UmiProviderTasksDestroy(other);umi_data_server_destroy(second);
    } else if(strcmp(mode,"sqlite-scan-limit")==0) {
        CHECK(umi_data_server_execute(server,"WITH RECURSIVE n(v) AS (VALUES(1) UNION ALL SELECT v+1 FROM n WHERE v<32768) INSERT INTO umicom_kv(key,value) SELECT printf('extra/%05d',v),'unrelated' FROM n;")==UMI_STATUS_OK);
        CHECK(UmiProviderTasksReadPage(store,&query,&page)==UMI_STATUS_OK && page.count==0U);
        CHECK(umi_data_server_set(server,"extra/overflow","unrelated")==UMI_STATUS_OK);
        failed(store,&query,UMI_STATUS_CAPACITY_EXCEEDED);
    } else if(strcmp(mode,"sqlite-readonly")==0) {
        three(store);UmiDataServer *readonly=NULL;UmiProviderTasks *other=NULL;
        CHECK(umi_data_server_open_sqlite_readonly(path,&readonly)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksOpen(readonly,"media","owner",&other)==UMI_STATUS_OK);
        CHECK(UmiProviderTasksReadPage(other,&query,&page)==UMI_STATUS_OK && page.scope_count==3U);
        UmiProviderTasksDestroy(other);umi_data_server_destroy(readonly);
    } else { UmiProviderTasksDestroy(store);umi_data_server_destroy(server);return 2; }
    UmiProviderTasksDestroy(store);umi_data_server_destroy(server);return 0;
}
