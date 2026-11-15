/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_review_controller.c
 *
 * PURPOSE:
 *   Exercise current-request publication, failure preservation and nonblocking review lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/review.h"
#include "umicom/platform/path.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); exit(1); } } while (0)

/* Inject only controller boundary failures. Real jobs, file analysis, queues
 * and retained result storage remain active for the ordinary paths. */
static int allocation_failure, service_failure, job_failure, submit_failure;
static int read_failure, cancel_failure, retirement_failure;
static void *AllocateReview(size_t count,size_t size)
{ return allocation_failure ? NULL : calloc(count,size); }
static UmiStatus CreateService(const char *root,UmiCodeGuardService **out)
{ return service_failure ? UMI_STATUS_OUT_OF_MEMORY : umi_codeguard_service_create(root,out); }
static UmiStatus CreateJob(UmiCodeGuardService *service,const UmiCodeGuardProfile *profile,UmiCodeGuardJob **out)
{ return job_failure ? UMI_STATUS_OUT_OF_MEMORY : umi_codeguard_service_create_job(service,profile,out); }
static UmiStatus SubmitJob(UmiCodeGuardJob *job,UmiTaskQueue *queue)
{ return submit_failure ? UMI_STATUS_UNAVAILABLE : UmiCodeGuardJobSubmit(job,queue); }
static UmiStatus ReadJob(UmiCodeGuardJob *job,UmiCodeGuardJobSnapshot *out)
{
    if(read_failure) { read_failure=0; return UMI_STATUS_IO_ERROR; }
    return UmiCodeGuardJobRead(job,out);
}
static UmiStatus CancelJob(UmiCodeGuardJob *job)
{
    if(cancel_failure) { cancel_failure=0; return UMI_STATUS_IO_ERROR; }
    return UmiCodeGuardJobCancel(job);
}
static UmiStatus RetireJob(UmiCodeGuardJob **job)
{
    if(retirement_failure) { retirement_failure=0; return UMI_STATUS_IO_ERROR; }
    return UmiCodeGuardJobDestroy(job);
}
#define calloc AllocateReview
#define umi_codeguard_service_create CreateService
#define umi_codeguard_service_create_job CreateJob
#define UmiCodeGuardJobSubmit SubmitJob
#define UmiCodeGuardJobRead ReadJob
#define UmiCodeGuardJobCancel CancelJob
#define UmiCodeGuardJobDestroy RetireJob
#include "../../src/codeguard/review.c"
#undef UmiCodeGuardJobDestroy
#undef UmiCodeGuardJobCancel
#undef UmiCodeGuardJobRead
#undef UmiCodeGuardJobSubmit
#undef umi_codeguard_service_create_job
#undef umi_codeguard_service_create
#undef calloc

static UmiMutex *gate_mutex;
static UmiCondition *gate_condition;
static int gate_entered, gate_release;
static UmiStatus BlockWorker(UmiTaskContext *context,void *data)
{
    (void)context; (void)data;
    UmiStatus status=umi_mutex_lock(gate_mutex);
    if(status!=UMI_STATUS_OK)return status;
    gate_entered=1;
    (void)umi_condition_broadcast(gate_condition);
    while(!gate_release && status==UMI_STATUS_OK)
        status=umi_condition_wait_for(gate_condition,gate_mutex,5000U);
    (void)umi_mutex_unlock(gate_mutex);
    return status;
}
static UmiTask *OccupyQueue(UmiTaskQueue *queue)
{
    CHECK(umi_mutex_create(&gate_mutex)==UMI_STATUS_OK);
    CHECK(umi_condition_create(&gate_condition)==UMI_STATUS_OK);
    UmiTaskConfig config={.label="controlled blocker",.function=BlockWorker};
    UmiTask *task=NULL;
    CHECK(umi_task_create(&config,&task)==UMI_STATUS_OK);
    CHECK(umi_task_queue_submit(queue,task)==UMI_STATUS_OK);
    CHECK(umi_mutex_lock(gate_mutex)==UMI_STATUS_OK);
    while(!gate_entered)
        CHECK(umi_condition_wait_for(gate_condition,gate_mutex,5000U)==UMI_STATUS_OK);
    CHECK(umi_mutex_unlock(gate_mutex)==UMI_STATUS_OK);
    return task;
}
static void ReleaseQueue(UmiTaskQueue *queue,UmiTask *task)
{
    CHECK(umi_mutex_lock(gate_mutex)==UMI_STATUS_OK);
    gate_release=1;
    CHECK(umi_condition_broadcast(gate_condition)==UMI_STATUS_OK);
    CHECK(umi_mutex_unlock(gate_mutex)==UMI_STATUS_OK);
    CHECK(umi_task_queue_wait_idle(queue,10000U)==UMI_STATUS_OK);
    CHECK(umi_task_result(task)==UMI_STATUS_OK);
    umi_task_destroy(task);
    umi_condition_destroy(gate_condition);
    umi_mutex_destroy(gate_mutex);
}
static UmiCodeGuardReviewSnapshot Poll(UmiCodeGuardReview *review,uint64_t context)
{
    UmiCodeGuardReviewSnapshot snapshot;
    CHECK(UmiCodeGuardReviewPoll(review,context,&snapshot)==UMI_STATUS_OK);
    return snapshot;
}
typedef struct OtherOwner {
    UmiCodeGuardReview *review;
    UmiTaskQueue *queue;
    UmiCodeGuardProfile profile;
    int passed;
} OtherOwner;
static int WrongOwner(void *data)
{
    OtherOwner *input=data;
    uint64_t request=77U;
    UmiCodeGuardReview *borrow=input->review;
    UmiCodeGuardReviewSnapshot snapshot;
    memset(&snapshot,0x5a,sizeof snapshot);
    UmiCodeGuardReviewSnapshot saved;
    memcpy(&saved,&snapshot,sizeof saved);
    input->passed=UmiCodeGuardReviewBegin(input->review,input->queue,&input->profile,41U,&request)==UMI_STATUS_INVALID_STATE &&
        request==77U && UmiCodeGuardReviewPoll(input->review,41U,&snapshot)==UMI_STATUS_INVALID_STATE &&
        !memcmp(&snapshot,&saved,sizeof saved) &&
        UmiCodeGuardReviewCancel(input->review)==UMI_STATUS_INVALID_STATE &&
        UmiCodeGuardReviewResult(input->review,41U)==NULL &&
        UmiCodeGuardReviewDestroy(&borrow)==UMI_STATUS_INVALID_STATE && borrow==input->review;
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    const char *name=argv[1];
    const char *cases[]={"arguments","idle","success","empty","budget","missing","cancel-terminal","cancel-failure",
        "cancel-after-failure","stale-terminal","stale-cancel-failure","new-context","wrong-owner","busy","queued-cancel",
        "queue-stopped","allocation","service-allocation","job-allocation","submit-failure",
        "read-failure","retirement-failure","request-overflow","profile-copy","destroy-terminal"};
    size_t selected=0U;
    while(selected<sizeof cases/sizeof cases[0] && strcmp(name,cases[selected])) ++selected;
    if(selected==sizeof cases/sizeof cases[0])return 2;
    char root[1024];
    int written=snprintf(root,sizeof root,"%s/%s",argv[2],!strcmp(name,"missing")?"absent":"workspace");
    CHECK(written>0 && (size_t)written<sizeof root);
    UmiCodeGuardReview *review=NULL;
    allocation_failure=!strcmp(name,"allocation");
    service_failure=!strcmp(name,"service-allocation");
    UmiStatus created=UmiCodeGuardReviewCreate(root,&review);
    if(allocation_failure || service_failure) {
        CHECK(created==UMI_STATUS_OUT_OF_MEMORY && review==NULL);
        return 0;
    }
    CHECK(created==UMI_STATUS_OK);
    UmiCodeGuardProfile profile=umi_codeguard_profile_default();
    profile.scan_architecture=0;
    profile.scan_source_names=0;
    profile.scan_duplicates=0;
    profile.max_line_length=SIZE_MAX;
    UmiTaskQueueConfig config={.worker_count=1U,.capacity=2U};
    UmiTaskQueue *queue=NULL;
    CHECK(umi_task_queue_create(&config,&queue)==UMI_STATUS_OK);
    uint64_t request=77U;
    if(!strcmp(name,"arguments")) {
        UmiCodeGuardReview *output=review;
        CHECK(UmiCodeGuardReviewCreate(NULL,&output)==UMI_STATUS_INVALID_ARGUMENT && output==NULL);
        output=review;
        CHECK(UmiCodeGuardReviewCreate("relative",&output)==UMI_STATUS_INVALID_ARGUMENT && output==NULL);
        CHECK(UmiCodeGuardReviewCreate(root,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardReviewBegin(NULL,queue,&profile,41U,&request)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardReviewBegin(review,NULL,&profile,41U,&request)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardReviewBegin(review,queue,&profile,0U,&request)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardReviewBegin(review,queue,&profile,41U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(request==77U);
        CHECK(UmiCodeGuardReviewPoll(review,41U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardReviewCancel(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardReviewDestroy(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardReviewResult(NULL,41U)==NULL);
    }
    if(!strcmp(name,"idle") || !strcmp(name,"arguments")) {
        UmiCodeGuardReviewSnapshot snapshot=Poll(review,41U);
        CHECK(snapshot.request_id==0U && !snapshot.pending && !snapshot.has_report);
        CHECK(snapshot.status==UMI_STATUS_NOT_FOUND && snapshot.operation_status==UMI_STATUS_NOT_FOUND);
        CHECK(UmiCodeGuardReviewCancel(review)==UMI_STATUS_OK);
        CHECK(UmiCodeGuardReviewResult(review,41U)==NULL);
    } else if(!strcmp(name,"wrong-owner")) {
        OtherOwner input={.review=review,.queue=queue,.profile=profile};
        UmiThread *thread=NULL;
        CHECK(umi_thread_start(WrongOwner,&input,&thread)==UMI_STATUS_OK);
        int code=-1;
        CHECK(umi_thread_join(thread,&code)==UMI_STATUS_OK && code==0 && input.passed);
        umi_thread_destroy(thread);
    } else {
        const UmiCodeGuardResult *retained=NULL;
        const UmiCodeGuardFinding *borrow=NULL;
        size_t before=0U;
        /* Establish a real complete report before the next attempt, except when
         * the requested root intentionally does not exist. */
        if(strcmp(name,"missing")) {
            CHECK(UmiCodeGuardReviewBegin(review,queue,&profile,41U,&request)==UMI_STATUS_OK && request==1U);
            CHECK(umi_task_queue_wait_idle(queue,10000U)==UMI_STATUS_OK);
            UmiCodeGuardReviewSnapshot first=Poll(review,41U);
            CHECK(first.status==UMI_STATUS_OK && first.has_report && !first.pending);
            retained=UmiCodeGuardReviewResult(review,41U);
            before=umi_codeguard_result_count(retained);
            borrow=umi_codeguard_result_at(retained,0U);
            CHECK(before>0U && borrow!=NULL);
        }
        if(!strcmp(name,"job-allocation"))job_failure=1;
        if(!strcmp(name,"submit-failure"))submit_failure=1;
        if(!strcmp(name,"queue-stopped"))CHECK(UmiTaskQueueRequestShutdown(queue,0,0)==UMI_STATUS_OK);
        if(!strcmp(name,"request-overflow"))review->sequence=UINT64_MAX;
        if(!strcmp(name,"empty"))profile.scan_c=0;
        if(!strcmp(name,"budget") || !strcmp(name,"cancel-after-failure"))profile.max_file_bytes=1U;
        UmiTask *blocker=NULL;
        if(!strcmp(name,"busy") || !strcmp(name,"queued-cancel"))blocker=OccupyQueue(queue);
        uint64_t context=!strcmp(name,"new-context")?42U:41U;
        request=77U;
        UmiStatus status=UmiCodeGuardReviewBegin(review,queue,&profile,context,&request);
        int rejected=job_failure || submit_failure || !strcmp(name,"queue-stopped") || !strcmp(name,"request-overflow");
        if(rejected) {
            UmiStatus expected=job_failure?UMI_STATUS_OUT_OF_MEMORY:submit_failure?UMI_STATUS_UNAVAILABLE:
                !strcmp(name,"request-overflow")?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_INVALID_STATE;
            CHECK(status==expected && request==77U);
            CHECK(UmiCodeGuardReviewResult(review,41U)==retained);
            CHECK(umi_codeguard_result_at(retained,0U)==borrow);
            UmiCodeGuardReviewSnapshot rejected_snapshot=Poll(review,41U);
            CHECK(!rejected_snapshot.pending);
            CHECK(rejected_snapshot.request_id==(!strcmp(name,"request-overflow")?UINT64_MAX:1U));
        } else {
            CHECK(status==UMI_STATUS_OK && request==(!strcmp(name,"missing")?1U:2U));
            if(!strcmp(name,"profile-copy"))profile.scan_c=0;
            if(blocker!=NULL) {
                uint64_t next=88U;
                CHECK(UmiCodeGuardReviewBegin(review,queue,&profile,41U,&next)==UMI_STATUS_BUSY && next==88U);
                UmiCodeGuardReview *owned=review;
                CHECK(UmiCodeGuardReviewDestroy(&owned)==UMI_STATUS_BUSY && owned==review);
                CHECK(Poll(review,41U).pending);
                /* Busy admission and cancellation are distinct scenarios: a
                 * busy owner can finish normally without losing its request. */
                if (!strcmp(name,"queued-cancel"))
                    CHECK(UmiCodeGuardReviewCancel(review)==UMI_STATUS_OK);
                ReleaseQueue(queue,blocker);
                if (!strcmp(name,"busy")) blocker=NULL;
            } else CHECK(umi_task_queue_wait_idle(queue,10000U)==UMI_STATUS_OK);
            if(!strcmp(name,"destroy-terminal")) {
                CHECK(UmiCodeGuardReviewDestroy(&review)==UMI_STATUS_OK && review==NULL);
            } else {
                if(!strcmp(name,"cancel-terminal") || !strcmp(name,"cancel-after-failure"))
                    CHECK(UmiCodeGuardReviewCancel(review)==UMI_STATUS_OK);
                if(!strcmp(name,"cancel-failure")) {
                    cancel_failure=1;
                    CHECK(UmiCodeGuardReviewCancel(review)==UMI_STATUS_IO_ERROR);
                }
                if(!strcmp(name,"stale-terminal") || !strcmp(name,"stale-cancel-failure")) context=0U;
                UmiCodeGuardReviewSnapshot snapshot,saved;
                memset(&snapshot,0x5a,sizeof snapshot);
                memcpy(&saved,&snapshot,sizeof saved);
                if(!strcmp(name,"read-failure"))read_failure=1;
                if(!strcmp(name,"retirement-failure"))retirement_failure=1;
                if(!strcmp(name,"stale-cancel-failure"))cancel_failure=1;
                if(read_failure || retirement_failure || cancel_failure) {
                    CHECK(UmiCodeGuardReviewPoll(review,context,&snapshot)==UMI_STATUS_IO_ERROR);
                    CHECK(!memcmp(&snapshot,&saved,sizeof saved));
                    /* Once stale, returning to the old token cannot revive it. */
                    if(!strcmp(name,"stale-cancel-failure"))context=41U;
                }
                snapshot=Poll(review,context);
                CHECK(!snapshot.pending && snapshot.request_id==request);
                if(!strcmp(name,"missing")) {
                    CHECK(snapshot.status!=UMI_STATUS_OK && !snapshot.has_report);
                    CHECK(UmiCodeGuardReviewResult(review,41U)==NULL);
                } else if(!strcmp(name,"budget") || !strcmp(name,"cancel-after-failure") || !strcmp(name,"cancel-terminal") || blocker!=NULL ||
                    !strcmp(name,"stale-terminal") || !strcmp(name,"stale-cancel-failure")) {
                    UmiStatus expected=(!strcmp(name,"budget") || !strcmp(name,"cancel-after-failure"))?UMI_STATUS_CAPACITY_EXCEEDED:
                        (!strcmp(name,"stale-terminal") || !strcmp(name,"stale-cancel-failure"))?
                        UMI_STATUS_INVALID_STATE:UMI_STATUS_CANCELLED;
                    CHECK(snapshot.status==expected);
                    CHECK(UmiCodeGuardReviewResult(review,41U)==retained);
                    CHECK(umi_codeguard_result_at(retained,0U)==borrow);
                    CHECK(umi_codeguard_result_count(retained)==before);
                    if(snapshot.stale) {
                        if(context==0U) CHECK(!snapshot.has_report && snapshot.summary.total==0U);
                        CHECK(Poll(review,41U).stale);
                        CHECK(UmiCodeGuardReviewResult(review,42U)==NULL);
                    }
                } else {
                    CHECK(snapshot.status==UMI_STATUS_OK && snapshot.has_report);
                    CHECK(snapshot.operation_status==UMI_STATUS_OK && !snapshot.cancel_requested);
                    CHECK(snapshot.summary.total==(!strcmp(name,"empty")?0U:before));
                    CHECK(UmiCodeGuardReviewResult(review,context)==retained);
                    if(context==42U)CHECK(UmiCodeGuardReviewResult(review,41U)==NULL);
                    UmiCodeGuardReviewSnapshot again=Poll(review,context);
                    CHECK(again.status==UMI_STATUS_OK && again.request_id==request && !again.pending);
                    CHECK(UmiCodeGuardReviewCancel(review)==UMI_STATUS_OK);
                    CHECK(Poll(review,context).status==UMI_STATUS_OK);
                }
            }
        }
    }
    CHECK(UmiCodeGuardReviewDestroy(&review)==UMI_STATUS_OK && review==NULL);
    CHECK(UmiCodeGuardReviewDestroy(&review)==UMI_STATUS_OK);
    CHECK(umi_task_queue_shutdown(queue,0)==UMI_STATUS_OK);
    umi_task_queue_destroy(queue);
    return 0;
}
