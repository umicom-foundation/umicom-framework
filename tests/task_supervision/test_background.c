/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/task_supervision/test_background.c
 * PURPOSE:
 *   Verify worker isolation, stale-result fences, Stop and exactly-once
 *   publication with a deliberately injected process byte stream.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include "umicom/test_platform/ctest.h"
#include "umicom/platform/process.h"

static atomic_int processEntered, processRelease;
static atomic_uint processCalls;
static int malformed;
static int failProcess;
static const char catalogue[] =
    "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1,\"minor\":0},\"tests\":["
    "{\"name\":\"notes.save[1]+\",\"command\":[\"notes-test\"],\"properties\":[]},"
    "{\"name\":\"notes.disabled\",\"properties\":[{\"name\":\"DISABLED\",\"value\":true}]}]}";

/* This explicit test double is not a claim of native process coverage. Native
 * process-task cancellation and real JSON discovery have separate tests. */
UmiStatus UmiProcessExecuteStreamed(const UmiProcessRequest *request,
    UmiProcessOutputObserver observer,void *payload,UmiProcessResult *result)
{
    CHECK(request!=NULL && request->cancellation!=NULL && observer!=NULL);
    CHECK(strcmp(request->program,"ctest")==0);
    memset(result,0,sizeof(*result));result->exit_code=-1;
    atomic_fetch_add(&processCalls,1U);atomic_store(&processEntered,1);
    while(!atomic_load(&processRelease)) {
        if(umi_cancellation_token_is_requested(request->cancellation)) {
            result->cancelled=1;result->launched=1;result->termination_requested=1;
            return UMI_STATUS_CANCELLED;
        }
        umi_thread_sleep_ms(1U);
    }
    result->launched=1;result->exit_code=failProcess ? 4 : 0;
    if(failProcess) return UMI_STATUS_INTERNAL_ERROR;
    const char *bytes=malformed ? "{invalid" : catalogue;
    size_t length=strlen(bytes);
    observer(bytes,length,payload);
    return UMI_STATUS_OK;
}

typedef struct Catalogue {
    UmiTestPlatformItemRegistry *items;
    UmiTestPlatformSuiteRegistry *suites;
    UmiTestPlatformDiscoveryRegistry *discoveries;
    UmiTestPlatformCtestImportOptions options;
} Catalogue;
static void Create(Catalogue *value,const char *root)
{
    memset(value,0,sizeof(*value));
    CHECK(umi_test_platform_item_registry_create(&value->items)==UMI_STATUS_OK);
    CHECK(umi_test_platform_suite_registry_create(&value->suites)==UMI_STATUS_OK);
    CHECK(umi_test_platform_discovery_registry_create(&value->discoveries)==UMI_STATUS_OK);
    snprintf(value->options.project_id,sizeof(value->options.project_id),"notes");
    snprintf(value->options.suite_id,sizeof(value->options.suite_id),"notes.ctest");
    snprintf(value->options.configuration,sizeof(value->options.configuration),"Debug");
    CHECK(strlen(root)<sizeof(value->options.build_directory));
    snprintf(value->options.build_directory,sizeof(value->options.build_directory),"%s",root);
}
static void Dispose(Catalogue *value)
{
    umi_test_platform_item_registry_destroy(value->items);
    umi_test_platform_suite_registry_destroy(value->suites);
    umi_test_platform_discovery_registry_destroy(value->discoveries);
}
static UmiCtestDiscoveryJob *Start(UmiTaskQueue *queue,Catalogue *value)
{
    UmiCtestDiscoveryJob *job=NULL;
    CHECK(UmiCtestDiscoveryJobStart(queue,&value->options,NULL,value->items,
        value->suites,value->discoveries,42U,&job)==UMI_STATUS_OK);
    return job;
}
static UmiStatus Poll(UmiCtestDiscoveryJob *job,uint64_t generation,int *published)
{
    UmiTestPlatformCtestImportSummary summary;
    char diagnostics[4096];UmiStatus status;
    uint64_t start=TestNowMs();
    do {
        status=UmiCtestDiscoveryJobPoll(job,generation,&summary,published,diagnostics,sizeof(diagnostics));
        if(status==UMI_STATUS_BUSY) umi_thread_sleep_ms(1U);
        CHECK(TestNowMs()-start<10000U);
    } while(status==UMI_STATUS_BUSY);
    if(status==UMI_STATUS_OK) CHECK(summary.discovered_count==2U && summary.disabled_count==1U);
    else CHECK(summary.discovered_count==0U);
    return status;
}
static int WrongThread(void *payload)
{
    UmiCtestDiscoveryJob *job=payload;int published=1;
    CHECK(UmiCtestDiscoveryJobPoll(job,42U,NULL,&published,NULL,0U)==UMI_STATUS_INVALID_STATE);
    CHECK(published==0);
    CHECK(UmiCtestDiscoveryJobCancel(job)==UMI_STATUS_INVALID_STATE);
    CHECK(UmiCtestDiscoveryJobDestroy(job)==UMI_STATUS_INVALID_STATE);
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==3);const char *scenario=argv[1];
    Catalogue value;Create(&value,argv[2]);
    atomic_init(&processEntered,0);atomic_init(&processRelease,0);atomic_init(&processCalls,0U);
    UmiTaskQueue *queue=TestQueue(1U,4U);
    int published=0;
    if(strcmp(scenario,"invalid")==0) {
        UmiCtestDiscoveryJob *job=NULL;
        CHECK(UmiCtestDiscoveryJobStart(NULL,&value.options,NULL,value.items,value.suites,
            value.discoveries,42U,&job)==UMI_STATUS_INVALID_ARGUMENT && job==NULL);
        UmiCancellationToken *token=NULL;
        CHECK(umi_cancellation_token_create(&token)==UMI_STATUS_OK);
        UmiTestPlatformCtestDiscoveryOptions execution={0};execution.cancellation=token;
        CHECK(UmiCtestDiscoveryJobStart(queue,&value.options,&execution,value.items,value.suites,
            value.discoveries,42U,&job)==UMI_STATUS_INVALID_ARGUMENT && job==NULL);
        umi_cancellation_token_destroy(token);
        CHECK(UmiCtestDiscoveryJobDestroy(NULL)==UMI_STATUS_OK);
        CHECK(UmiCtestDiscoveryJobCancel(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(strcmp(scenario,"cancel-queued-destroy")==0) {
        TestGate blocker;TestGateInit(&blocker,0);UmiTask *task=TestTask(&blocker);
        CHECK(umi_task_queue_submit(queue,task)==UMI_STATUS_OK);TestAwait(&blocker.entered);
        UmiCtestDiscoveryJob *job=Start(queue,&value);
        CHECK(UmiCtestDiscoveryJobDestroy(job)==UMI_STATUS_BUSY);
        CHECK(UmiCtestDiscoveryJobCancel(job)==UMI_STATUS_OK);
        CHECK(Poll(job,42U,&published)==UMI_STATUS_CANCELLED && !published);
        CHECK(UmiCtestDiscoveryJobDestroy(job)==UMI_STATUS_OK);
        atomic_store(&blocker.released,1);
        CHECK(umi_task_queue_wait_idle(queue,10000U)==UMI_STATUS_OK);
        CHECK(atomic_load(&processCalls)==0U);umi_task_destroy(task);
    } else if(strcmp(scenario,"queue-rejected")==0) {
        CHECK(UmiTaskQueueRequestShutdown(queue,0,0)==UMI_STATUS_OK);
        UmiCtestDiscoveryJob *job=NULL;
        CHECK(UmiCtestDiscoveryJobStart(queue,&value.options,NULL,value.items,value.suites,
            value.discoveries,42U,&job)==UMI_STATUS_INVALID_STATE && job==NULL);
    } else {
        malformed=strcmp(scenario,"malformed")==0;failProcess=strcmp(scenario,"process-failure")==0;
        UmiCtestDiscoveryJob *job=Start(queue,&value);
        TestAwait(&processEntered);
        CHECK(umi_test_platform_item_registry_count(value.items)==0U);
        CHECK(UmiCtestDiscoveryJobPoll(job,42U,NULL,&published,NULL,0U)==UMI_STATUS_BUSY);
        CHECK(UmiCtestDiscoveryJobDestroy(job)==UMI_STATUS_BUSY);
        if(strcmp(scenario,"wrong-thread")==0) {
            UmiThread *thread=NULL;CHECK(umi_thread_start(WrongThread,job,&thread)==UMI_STATUS_OK);
            CHECK(umi_thread_join(thread,NULL)==UMI_STATUS_OK);umi_thread_destroy(thread);
        }
        if(strcmp(scenario,"stale-items")==0) umi_test_platform_item_registry_clear(value.items);
        if(strcmp(scenario,"stale-suites")==0) umi_test_platform_suite_registry_clear(value.suites);
        if(strcmp(scenario,"stale-discoveries")==0) umi_test_platform_discovery_registry_clear(value.discoveries);
        int cancelled=strcmp(scenario,"cancel-running")==0 || strcmp(scenario,"cancel-completed")==0;
        if(strcmp(scenario,"cancel-running")==0) CHECK(UmiCtestDiscoveryJobCancel(job)==UMI_STATUS_OK);
        else atomic_store(&processRelease,1);
        CHECK(umi_task_queue_wait_idle(queue,10000U)==UMI_STATUS_OK);
        CHECK(umi_test_platform_item_registry_count(value.items)==0U);
        if(strcmp(scenario,"cancel-completed")==0) CHECK(UmiCtestDiscoveryJobCancel(job)==UMI_STATUS_OK);
        uint64_t generation=strcmp(scenario,"stale-generation")==0 ? 43U:42U;
        UmiStatus expected=cancelled ? UMI_STATUS_CANCELLED : malformed ? UMI_STATUS_PARSE_ERROR :
            failProcess ? UMI_STATUS_INTERNAL_ERROR : strncmp(scenario,"stale-",6U)==0
                ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
        UmiStatus status=Poll(job,generation,&published);
        CHECK(status==expected);
        CHECK(published==(status==UMI_STATUS_OK));
        CHECK(umi_test_platform_item_registry_count(value.items)==(published ? 3U:0U));
        CHECK(Poll(job,generation,&published)==expected && !published);
        CHECK(UmiCtestDiscoveryJobCancel(job)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiCtestDiscoveryJobDestroy(job)==UMI_STATUS_OK);
    }
    TestFinish(queue);Dispose(&value);printf("PASS %s\n",scenario);return 0;
}
