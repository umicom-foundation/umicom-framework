/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/task_supervision/test_tasks.c
 * PURPOSE: Exercise actual task and queue sources, including cancellation races.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

static void TestBasic(const char *scenario)
{
    TestGate gate;
    UmiTask *task;
    TestGateInit(&gate, 1);
    task = TestTask(&gate);
    CHECK(umi_task_state(task) == UMI_TASK_CREATED);
    CHECK(umi_task_result(task) == UMI_STATUS_INVALID_STATE);
    CHECK(strcmp(umi_task_label(task), "Umicom Notes: index project") == 0);
    CHECK(umi_task_progress(task) == 0U);
    if (strcmp(scenario, "cancel-created") == 0) {
        CHECK(umi_task_cancel(task) == UMI_STATUS_OK);
        CHECK(umi_task_wait(task, 20U) == UMI_STATUS_OK);
        CHECK(umi_task_run(task) == UMI_STATUS_CANCELLED);
        CHECK(atomic_load(&gate.calls) == 0U);
        CHECK(umi_task_state(task) == UMI_TASK_CANCELLED);
    } else if (strcmp(scenario, "wait-timeout") == 0) {
        CHECK(umi_task_wait(task, 10U) == UMI_STATUS_TIMEOUT);
        CHECK(umi_task_state(task) == UMI_TASK_CREATED);
    } else {
        atomic_store(&gate.released, 1);
        if (strcmp(scenario, "failure") == 0) gate.returnStatus = UMI_STATUS_IO_ERROR;
        if (strcmp(scenario, "returned-cancel") == 0) gate.returnStatus = UMI_STATUS_CANCELLED;
        CHECK(umi_task_run(task) == gate.returnStatus);
        CHECK(umi_task_result(task) == gate.returnStatus);
        CHECK(umi_task_wait(task, 0U) == UMI_STATUS_OK);
        CHECK(umi_task_state(task) == (gate.returnStatus == UMI_STATUS_OK
            ? UMI_TASK_SUCCEEDED : gate.returnStatus == UMI_STATUS_CANCELLED
                ? UMI_TASK_CANCELLED : UMI_TASK_FAILED));
        CHECK(umi_task_cancel(task) == UMI_STATUS_OK);
        CHECK(umi_task_result(task) == gate.returnStatus);
        CHECK(umi_task_run(task) == (gate.returnStatus == UMI_STATUS_CANCELLED
            ? UMI_STATUS_CANCELLED : UMI_STATUS_INVALID_STATE));
        CHECK(atomic_load(&gate.calls) == 1U);
        if (gate.returnStatus == UMI_STATUS_OK) CHECK(umi_task_progress(task) == 100U);
    }
    umi_task_destroy(task);
}

static void TestQueuedRelease(int releaseBeforeCancel)
{
    TestGate running, pending;
    TestGateInit(&running, 0); TestGateInit(&pending, 1);
    UmiTaskQueue *queue = TestQueue(1U, 4U);
    UmiTask *first = TestTask(&running), *second = TestTask(&pending);
    CHECK(umi_task_queue_submit(queue, first) == UMI_STATUS_OK);
    TestAwait(&running.entered);
    CHECK(umi_task_queue_submit(queue, second) == UMI_STATUS_OK);
    if (releaseBeforeCancel) {
        umi_task_destroy(second);
        CHECK(UmiTaskQueueCancelAll(queue) == UMI_STATUS_OK);
    } else {
        CHECK(umi_task_cancel(second) == UMI_STATUS_OK);
        CHECK(umi_task_wait(second, 100U) == UMI_STATUS_OK);
        /* The old implementation frees a task that is still in queue->items. */
        umi_task_destroy(second);
    }
    atomic_store(&running.released, 1);
    CHECK(umi_task_queue_wait_idle(queue, 10000U) == UMI_STATUS_OK);
    CHECK(atomic_load(&pending.calls) == 0U);
    UmiTaskQueueStats stats = umi_task_queue_stats(queue);
    CHECK(stats.submitted == 2U && stats.completed == 2U);
    CHECK(stats.cancelled == (releaseBeforeCancel ? 2U : 1U));
    umi_task_destroy(first);
    TestFinish(queue);
}

static void TestRunningRelease(int honourCancel, int releaseEarly)
{
    TestGate gate;
    TestGateInit(&gate, honourCancel);
    UmiTaskQueue *queue = TestQueue(1U, 4U);
    UmiTask *task = TestTask(&gate);
    CHECK(umi_task_queue_submit(queue, task) == UMI_STATUS_OK);
    TestAwait(&gate.entered);
    CHECK(umi_task_cancel(task) == UMI_STATUS_OK);
    if (releaseEarly) umi_task_destroy(task);
    if (!honourCancel) {
        TestAwait(&gate.cancelled);
        if (!releaseEarly) {
            CHECK(umi_task_wait(task, 10U) == UMI_STATUS_TIMEOUT);
            CHECK(umi_task_state(task) == UMI_TASK_RUNNING);
        }
        atomic_store(&gate.released, 1);
    }
    CHECK(umi_task_queue_wait_idle(queue, 10000U) == UMI_STATUS_OK);
    CHECK(atomic_load(&gate.exited) == 1);
    CHECK(umi_task_queue_stats(queue).cancelled == 1U);
    if (!releaseEarly) {
        CHECK(umi_task_state(task) == UMI_TASK_CANCELLED);
        CHECK(umi_task_result(task) == UMI_STATUS_CANCELLED);
        umi_task_destroy(task);
    }
    TestFinish(queue);
}

static void TestFullQueue(void)
{
    TestGate gate, pending;
    TestGateInit(&gate, 0);TestGateInit(&pending, 1);
    atomic_store(&pending.released, 1);
    UmiTaskQueue *queue = TestQueue(1U, 1U);
    UmiTask *first=TestTask(&gate), *second=TestTask(&pending), *third=TestTask(&pending);
    CHECK(umi_task_queue_submit(queue, first) == UMI_STATUS_OK);
    TestAwait(&gate.entered);
    CHECK(umi_task_queue_submit(queue, second) == UMI_STATUS_OK);
    CHECK(umi_task_queue_submit(queue, third) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_task_state(third) == UMI_TASK_CREATED);
    atomic_store(&gate.released, 1);
    CHECK(umi_task_queue_wait_idle(queue, 10000U) == UMI_STATUS_OK);
    CHECK(umi_task_queue_submit(queue, third) == UMI_STATUS_OK);
    CHECK(umi_task_queue_wait_idle(queue, 10000U) == UMI_STATUS_OK);
    CHECK(umi_task_queue_stats(queue).submitted == 3U);
    CHECK(atomic_load(&pending.calls) == 2U);
    umi_task_destroy(first);umi_task_destroy(second);umi_task_destroy(third);
    TestFinish(queue);
}

static void TestStop(const char *scenario)
{
    TestGate running, pending;
    TestGateInit(&running, 0);TestGateInit(&pending, 1);
    atomic_store(&pending.released, 1);
    UmiTaskQueue *queue=TestQueue(1U, 4U);
    UmiTask *first=TestTask(&running), *second=TestTask(&pending), *next=TestTask(&pending);
    CHECK(umi_task_queue_submit(queue, first) == UMI_STATUS_OK);
    TestAwait(&running.entered);
    CHECK(umi_task_queue_submit(queue, second) == UMI_STATUS_OK);
    int cancelPending = strcmp(scenario,"shutdown-drain") != 0;
    int cancelRunning = strcmp(scenario,"shutdown-running") == 0;
    int reusable = strcmp(scenario,"cancel-all-reusable") == 0;
    int escalate = strcmp(scenario,"shutdown-escalate") == 0;
    if (escalate) cancelPending=0;
    uint64_t start = TestNowMs();
    CHECK((reusable ? UmiTaskQueueCancelAll(queue) :
        UmiTaskQueueRequestShutdown(queue, cancelPending, cancelRunning)) == UMI_STATUS_OK);
    CHECK(TestNowMs()-start < 1000U);
    CHECK(!atomic_load(&running.exited));
    CHECK(umi_task_queue_wait_idle(queue, 10U) == UMI_STATUS_TIMEOUT);
    if (escalate) {
        CHECK(UmiTaskQueueRequestShutdown(queue, 1, 1) == UMI_STATUS_OK);
        cancelPending=1;cancelRunning=1;
    }
    if (reusable || cancelRunning) TestAwait(&running.cancelled);
    if (!reusable) {
        CHECK(umi_task_queue_submit(queue, next) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_task_state(next) == UMI_TASK_CREATED);
    }
    atomic_store(&running.released, 1);
    CHECK(umi_task_queue_wait_idle(queue, 10000U) == UMI_STATUS_OK);
    CHECK(atomic_load(&pending.calls) == (cancelPending ? 0U : 1U));
    if (reusable) {
        CHECK(umi_task_queue_submit(queue, next) == UMI_STATUS_OK);
        CHECK(umi_task_queue_wait_idle(queue, 10000U) == UMI_STATUS_OK);
        CHECK(umi_task_result(next) == UMI_STATUS_OK);
    }
    CHECK(umi_task_queue_shutdown(queue, 1) == UMI_STATUS_OK);
    CHECK(umi_task_queue_shutdown(queue, 0) == UMI_STATUS_OK);
    CHECK(umi_task_result(first) == ((cancelRunning || reusable) ? UMI_STATUS_CANCELLED : UMI_STATUS_OK));
    umi_task_destroy(first);umi_task_destroy(second);umi_task_destroy(next);
    umi_task_queue_destroy(queue);
}

typedef struct Reentrant { UmiTaskQueue *queue; int kind; } Reentrant;
static UmiStatus TestWorkerControl(UmiTaskContext *context, void *payload)
{
    Reentrant *value=payload;
    (void)context;
    if (value->kind == 0) CHECK(umi_task_queue_shutdown(value->queue,1) == UMI_STATUS_INVALID_STATE);
    else if (value->kind == 1) CHECK(umi_task_queue_wait_idle(value->queue,10U) == UMI_STATUS_INVALID_STATE);
    else CHECK(UmiTaskQueueRequestShutdown(value->queue,0,0) == UMI_STATUS_OK);
    return UMI_STATUS_OK;
}
static void TestReentrant(int kind)
{
    UmiTaskQueue *queue=TestQueue(1U,4U);
    Reentrant value={queue,kind};
    UmiTaskConfig config={"Notes task control",TestWorkerControl,&value,NULL,NULL};
    UmiTask *task=NULL;
    CHECK(umi_task_create(&config,&task)==UMI_STATUS_OK);
    CHECK(umi_task_queue_submit(queue,task)==UMI_STATUS_OK);
    CHECK(umi_task_wait(task,10000U)==UMI_STATUS_OK);
    CHECK(umi_task_result(task)==UMI_STATUS_OK);
    umi_task_destroy(task);TestFinish(queue);
}

typedef struct Progress { UmiTask *task; atomic_uint calls; int cancel; } Progress;
static void TestProgress(uint64_t taskId,unsigned percent,const char *message,void *payload)
{
    Progress *p=payload;
    CHECK(taskId==umi_task_id(p->task)); CHECK(percent==25U);
    CHECK(strcmp(message,"Notes indexing")==0);
    CHECK(umi_task_progress(p->task)==25U);
    atomic_fetch_add(&p->calls,1U);
    if(p->cancel) CHECK(umi_task_cancel(p->task)==UMI_STATUS_OK);
}
static UmiStatus TestProgressTask(UmiTaskContext *context,void *payload)
{
    Progress *p=payload;
    CHECK(umi_task_context_report(context,101U,"invalid")==UMI_STATUS_INVALID_ARGUMENT);
    UmiStatus result=umi_task_context_report(context,25U,"Notes indexing");
    CHECK(result==(p->cancel ? UMI_STATUS_CANCELLED : UMI_STATUS_OK));
    CHECK(umi_cancellation_token_is_requested(UmiTaskContextCancellation(context))==p->cancel);
    return result;
}
static void TestProgressCase(int cancel)
{
    Progress p={0};atomic_init(&p.calls,0U);p.cancel=cancel;
    UmiTaskConfig config={"Notes indexing",TestProgressTask,&p,TestProgress,&p};
    CHECK(umi_task_create(&config,&p.task)==UMI_STATUS_OK);
    CHECK(umi_task_run(p.task)==(cancel ? UMI_STATUS_CANCELLED : UMI_STATUS_OK));
    CHECK(atomic_load(&p.calls)==1U);umi_task_destroy(p.task);
}

static void TestMany(int cancel)
{
    TestGate gate;
    TestGateInit(&gate,1);atomic_store(&gate.released,1);
    UmiTaskQueue *queue=TestQueue(4U,32U);
    for(size_t index=0U;index<2000U;++index) {
        UmiTask *task=TestTask(&gate);
        UmiStatus status;
        while((status=umi_task_queue_submit(queue,task))==UMI_STATUS_CAPACITY_EXCEEDED) {
            CHECK(umi_task_state(task)==UMI_TASK_CREATED);umi_thread_sleep_ms(1U);
        }
        CHECK(status==UMI_STATUS_OK);
        if(cancel && index%2U==0U) CHECK(umi_task_cancel(task)==UMI_STATUS_OK);
        umi_task_destroy(task);
    }
    CHECK(umi_task_queue_wait_idle(queue,10000U)==UMI_STATUS_OK);
    UmiTaskQueueStats stats=umi_task_queue_stats(queue);
    CHECK(stats.submitted==2000U && stats.completed==2000U);
    CHECK(stats.failed==0U && stats.queued==0U && stats.running==0U);
    CHECK(atomic_load(&gate.calls)<=2000U);
    if(!cancel) CHECK(atomic_load(&gate.calls)==2000U && stats.cancelled==0U);
    TestFinish(queue);
}

static int TestRetainThread(void *payload)
{
    UmiTask *task=payload;
    for(size_t index=0;index<10000U;++index) {
        CHECK(UmiTaskRetain(task)==UMI_STATUS_OK);CHECK(umi_task_id(task)!=0U);umi_task_destroy(task);
    }
    umi_task_destroy(task);return 0;
}
static void TestRetain(void)
{
    TestGate gate;TestGateInit(&gate,1);
    UmiTask *task=TestTask(&gate);
    UmiThread *threads[8]={0};
    for(size_t index=0;index<8U;++index) {
        CHECK(UmiTaskRetain(task)==UMI_STATUS_OK);
        CHECK(umi_thread_start(TestRetainThread,task,&threads[index])==UMI_STATUS_OK);
    }
    for(size_t index=0;index<8U;++index) {
        CHECK(umi_thread_join(threads[index],NULL)==UMI_STATUS_OK);umi_thread_destroy(threads[index]);
    }
    CHECK(umi_task_state(task)==UMI_TASK_CREATED);umi_task_destroy(task);
}

static void TestDuplicate(void)
{
    TestGate gate;TestGateInit(&gate,0);
    UmiTaskQueue *queue=TestQueue(1U,4U);UmiTask *task=TestTask(&gate);
    CHECK(umi_task_queue_submit(queue,task)==UMI_STATUS_OK);TestAwait(&gate.entered);
    CHECK(umi_task_queue_submit(queue,task)==UMI_STATUS_INVALID_STATE);
    CHECK(umi_task_run(task)==UMI_STATUS_INVALID_STATE);
    atomic_store(&gate.released,1);TestFinish(queue);
    CHECK(atomic_load(&gate.calls)==1U);umi_task_destroy(task);
}

static void TestInvalid(void)
{
    UmiTask *task=NULL;UmiTaskQueue *queue=NULL;
    UmiTaskConfig taskConfig={0};UmiTaskQueueConfig config={0U,2U};
    CHECK(umi_task_create(NULL,&task)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_task_create(&taskConfig,&task)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTaskRetain(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTaskContextCancellation(NULL)==NULL);
    CHECK(umi_task_cancel(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_task_wait(NULL,1U)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_task_run(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_task_queue_create(&config,&queue)==UMI_STATUS_INVALID_ARGUMENT);
    config.worker_count=65U;CHECK(umi_task_queue_create(&config,&queue)==UMI_STATUS_INVALID_ARGUMENT);
    config.worker_count=1U;config.capacity=0U;CHECK(umi_task_queue_create(&config,&queue)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTaskQueueCancelAll(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTaskQueueRequestShutdown(NULL,1,1)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_task_queue_shutdown(NULL,0)==UMI_STATUS_INVALID_ARGUMENT);
    umi_task_destroy(NULL);umi_task_queue_destroy(NULL);
    CHECK(umi_task_queue_create(NULL,&queue)==UMI_STATUS_OK);
    CHECK(umi_task_queue_stats(queue).worker_count==2U);TestFinish(queue);
}

int main(int argc,char **argv)
{
    CHECK(argc==2);const char *s=argv[1];
    if(strcmp(s,"success")==0 || strcmp(s,"failure")==0 || strcmp(s,"returned-cancel")==0 ||
        strcmp(s,"cancel-created")==0 || strcmp(s,"wait-timeout")==0) TestBasic(s);
    else if(strcmp(s,"cancel-queued-release")==0) TestQueuedRelease(0);
    else if(strcmp(s,"release-queued-cancel-all")==0) TestQueuedRelease(1);
    else if(strcmp(s,"cancel-running")==0) TestRunningRelease(1,0);
    else if(strcmp(s,"release-running")==0) TestRunningRelease(0,1);
    else if(strcmp(s,"cooperation-required")==0) TestRunningRelease(0,0);
    else if(strcmp(s,"full-queue-retry")==0) TestFullQueue();
    else if(strncmp(s,"shutdown-",9U)==0 || strcmp(s,"cancel-all-reusable")==0) TestStop(s);
    else if(strcmp(s,"worker-join-refused")==0) TestReentrant(0);
    else if(strcmp(s,"worker-wait-refused")==0) TestReentrant(1);
    else if(strcmp(s,"worker-request-shutdown")==0) TestReentrant(2);
    else if(strcmp(s,"progress")==0) TestProgressCase(0);
    else if(strcmp(s,"progress-cancel")==0) TestProgressCase(1);
    else if(strcmp(s,"many-tasks")==0) TestMany(0);
    else if(strcmp(s,"cancel-completion-races")==0) TestMany(1);
    else if(strcmp(s,"retained-concurrent")==0) TestRetain();
    else if(strcmp(s,"double-submit")==0) TestDuplicate();
    else if(strcmp(s,"invalid")==0) TestInvalid();
    else CHECK(0);
    printf("PASS %s\n",s);return 0;
}
