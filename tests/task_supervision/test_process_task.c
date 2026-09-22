/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/task_supervision/test_process_task.c
 * PURPOSE: Prove that a real process receives its owning task's Stop token.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include "umicom/platform/process.h"

typedef struct ProcessTask {
    const char *helper;
    const char *mode;
    uint32_t timeoutMs;
    atomic_int ready;
    UmiProcessResult result;
    UmiStatus status;
} ProcessTask;
static void Observe(const UmiProcessResult *result, void *payload)
{
    ProcessTask *work=payload;
    if(strstr(result->output,"helper ready")!=NULL) atomic_store(&work->ready,1);
}
static UmiStatus Execute(UmiTaskContext *context,void *payload)
{
    ProcessTask *work=payload;
    const char *arguments[]={work->mode};
    UmiProcessRequest request={0};
    request.program=work->helper;request.arguments=arguments;request.argument_count=1U;
    request.capture_stdout=1;request.capture_stderr=1;request.timeout_ms=work->timeoutMs;
    request.poll_interval_ms=2U;request.window_mode=UMI_PROCESS_WINDOW_HIDDEN;
    request.cancellation=UmiTaskContextCancellation(context);
    work->status=UmiProcessExecuteObserved(&request,Observe,work,&work->result);
    if(work->status==UMI_STATUS_OK && work->result.exit_code!=0) return UMI_STATUS_IO_ERROR;
    return work->status;
}
int main(int argc,char **argv)
{
    CHECK(argc==3);
    const char *scenario=argv[2];
    ProcessTask *work=calloc(1U,sizeof(*work));CHECK(work!=NULL);
    work->helper=argv[1];work->timeoutMs=5000U;work->mode="wait";atomic_init(&work->ready,0);
    int cancel=strcmp(scenario,"cancel")==0;
    int shutdown=strcmp(scenario,"queue-shutdown")==0;
    int timeout=strcmp(scenario,"timeout")==0;
    int failure=strcmp(scenario,"failure")==0;
    if(strcmp(scenario,"success")==0) work->mode="complete";
    else if(failure) work->mode="fail";
    else if(timeout) work->timeoutMs=80U;
    else CHECK(cancel||shutdown);
    UmiTaskConfig config={"Notes background process",Execute,work,NULL,NULL};
    UmiTask *task=NULL;CHECK(umi_task_create(&config,&task)==UMI_STATUS_OK);
    UmiTaskQueue *queue=TestQueue(1U,4U);
    CHECK(umi_task_queue_submit(queue,task)==UMI_STATUS_OK);
    if(cancel||shutdown) {
        TestAwait(&work->ready);
        if(shutdown) CHECK(UmiTaskQueueRequestShutdown(queue,1,1)==UMI_STATUS_OK);
        else CHECK(umi_task_cancel(task)==UMI_STATUS_OK);
    }
    CHECK(umi_task_wait(task,10000U)==UMI_STATUS_OK);
    if(cancel||shutdown) {
        CHECK(umi_task_result(task)==UMI_STATUS_CANCELLED);
        CHECK(work->result.cancelled && work->result.termination_requested);
    } else if(timeout) {
        CHECK(umi_task_result(task)==UMI_STATUS_TIMEOUT);
        CHECK(work->result.timed_out && work->result.termination_requested);
    } else if(failure) {
        CHECK(umi_task_result(task)==UMI_STATUS_INTERNAL_ERROR);CHECK(work->result.exit_code==7);
    } else CHECK(umi_task_result(task)==UMI_STATUS_OK && work->result.exit_code==0);
    CHECK(work->result.launched);
    CHECK(strstr(work->result.output,"helper ready")!=NULL);
    printf("PASS %s; process status=%d duration=%llu ms cancelled=%d timed_out=%d\n",scenario,
        (int)work->status,(unsigned long long)work->result.duration_ms,
        work->result.cancelled,work->result.timed_out);
    umi_task_destroy(task);TestFinish(queue);free(work);return 0;
}
