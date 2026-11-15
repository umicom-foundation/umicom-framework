/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_job_lifecycle.c
 *
 * PURPOSE:
 *   Verify copied job inputs, first-failure status, queue lifetime and one-shot result transfer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/job.h"
#include "umicom/codeguard/scanner.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); exit(1); } } while (0)

static const char *scenario;
static UmiCodeGuardJob *active_job;
static unsigned scans, job_frees;
static char observed_root[UMI_PATH_CAPACITY];
static size_t observed_limit;
static int observed_c;
static int cancel_after_store;
static UmiMutex *gate_mutex;
static UmiCondition *gate_condition;
static int gate_entered, gate_release;

static UmiStatus Gate(void)
{
    UmiStatus status = umi_mutex_lock(gate_mutex);
    if (status != UMI_STATUS_OK) return status;
    gate_entered = 1;
    (void)umi_condition_broadcast(gate_condition);
    while (!gate_release && status == UMI_STATUS_OK)
        status = umi_condition_wait_for(gate_condition, gate_mutex, 5000U);
    (void)umi_mutex_unlock(gate_mutex);
    return status;
}
static void AwaitGate(void)
{
    CHECK(umi_mutex_lock(gate_mutex) == UMI_STATUS_OK);
    while (!gate_entered)
        CHECK(umi_condition_wait_for(gate_condition, gate_mutex, 5000U) == UMI_STATUS_OK);
    CHECK(umi_mutex_unlock(gate_mutex) == UMI_STATUS_OK);
}
static void ReleaseGate(void)
{
    CHECK(umi_mutex_lock(gate_mutex) == UMI_STATUS_OK);
    gate_release = 1;
    CHECK(umi_condition_broadcast(gate_condition) == UMI_STATUS_OK);
    CHECK(umi_mutex_unlock(gate_mutex) == UMI_STATUS_OK);
}
static UmiStatus Scan(const UmiCodeGuardConfig *config, UmiCodeGuardResult *result,
    const UmiCancellationToken *cancellation)
{
    ++scans;
    (void)snprintf(observed_root, sizeof observed_root, "%s", config->root);
    observed_limit = config->profile.max_file_bytes;
    observed_c = config->profile.scan_c;
    if (!strcmp(scenario, "running") || !strcmp(scenario, "running-cancel")) {
        UmiStatus status = Gate();
        if (status != UMI_STATUS_OK) return status;
    }
    UmiCodeGuardFinding finding = {.severity = UMI_CODEGUARD_LOW,
        .category = UMI_CODEGUARD_CATEGORY_QUALITY, .confidence = 100U, .line = 1U};
    strcpy(finding.rule_id, "JOB-TEST");
    strcpy(finding.path, "fixture.c");
    UmiStatus status = umi_codeguard_result_add(result, &finding);
    if (status != UMI_STATUS_OK) return status;
    if (!strcmp(scenario, "cancel-error") || !strcmp(scenario, "cancel-success"))
        CHECK(UmiCodeGuardJobCancel(active_job) == UMI_STATUS_OK);
    if (!strcmp(scenario, "failure") || !strcmp(scenario, "cancel-error"))
        return UMI_STATUS_IO_ERROR;
    if (!strcmp(scenario, "publication-cancel")) cancel_after_store = 1;
    return umi_cancellation_token_is_requested(cancellation)
        ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
static void *AllocateJob(size_t count, size_t size)
{
    return !strcmp(scenario, "allocation") ? NULL : calloc(count, size);
}
static void FreeJob(void *value) { if (value != NULL) ++job_frees; free(value); }
static UmiStatus MutexCreate(UmiMutex **out)
{
    return !strcmp(scenario, "mutex-allocation") ? UMI_STATUS_OUT_OF_MEMORY : umi_mutex_create(out);
}
static UmiStatus TaskCreate(const UmiTaskConfig *config, UmiTask **out)
{
    return !strcmp(scenario, "task-allocation") ? UMI_STATUS_OUT_OF_MEMORY : umi_task_create(config, out);
}
static UmiStatus ResultCreate(size_t capacity, UmiCodeGuardResult **out)
{
    return !strcmp(scenario, "candidate-allocation") ? UMI_STATUS_OUT_OF_MEMORY
        : umi_codeguard_result_create(capacity, out);
}

/* Request cancellation after the worker stores its successful private result,
 * but before the task runner classifies completion. A terminal task check,
 * not merely operation_completed, must guard transfer. */
static UmiStatus UnlockJob(UmiMutex *mutex)
{
    UmiStatus status = umi_mutex_unlock(mutex);
    if (cancel_after_store) {
        cancel_after_store = 0;
        CHECK(UmiCodeGuardJobCancel(active_job) == UMI_STATUS_OK);
    }
    return status;
}

/* Only input analysis and allocation boundaries are controlled. Real task
 * cancellation, queue references, mutexes and result storage remain active. */
#define umi_mutex_unlock UnlockJob
#define calloc AllocateJob
#define free FreeJob
#define umi_mutex_create MutexCreate
#define umi_task_create TaskCreate
#define umi_codeguard_result_create ResultCreate
#define umi_codeguard_scan_cancellable Scan
#include "../../src/codeguard/job.c"
#undef umi_codeguard_scan_cancellable
#undef umi_codeguard_result_create
#undef umi_task_create
#undef umi_mutex_create
#undef free
#undef calloc
#undef umi_mutex_unlock

static UmiCodeGuardJobSnapshot Snapshot(UmiCodeGuardJob *job)
{
    UmiCodeGuardJobSnapshot snapshot;
    CHECK(UmiCodeGuardJobRead(job, &snapshot) == UMI_STATUS_OK);
    return snapshot;
}
static UmiStatus Block(UmiTaskContext *context, void *data)
{
    (void)context; (void)data;
    return Gate();
}
static UmiStatus Noop(UmiTaskContext *context, void *data)
{
    (void)context; (void)data;
    return UMI_STATUS_OK;
}
typedef struct WrongOwnerInput { UmiCodeGuardJob *job; UmiTaskQueue *queue; int passed; } WrongOwnerInput;
static int WrongOwner(void *data)
{
    WrongOwnerInput *input = data;
    UmiCodeGuardJob *owned = input->job;
    UmiCodeGuardResult *result = NULL;
    UmiCodeGuardJobSnapshot snapshot;
    input->passed = UmiCodeGuardJobRead(input->job, &snapshot) == UMI_STATUS_OK &&
        UmiCodeGuardJobSubmit(input->job, input->queue) == UMI_STATUS_INVALID_STATE &&
        UmiCodeGuardJobTake(input->job, &result) == UMI_STATUS_INVALID_STATE &&
        UmiCodeGuardJobDestroy(&owned) == UMI_STATUS_INVALID_STATE &&
        owned == input->job && result == NULL;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    scenario = argv[1];
    static const char *cases[] = {"arguments", "allocation", "mutex-allocation",
        "task-allocation", "candidate-allocation", "created", "copy", "defaults",
        "success", "failure", "cancel-before", "cancel-success", "cancel-error",
        "publication-cancel", "late-cancel", "double-take", "wrong-owner", "queue-full", "queue-stopped",
        "queued-cancel", "running", "running-cancel"};
    size_t selected = 0U;
    while (selected < sizeof cases / sizeof cases[0] && strcmp(cases[selected], scenario)) ++selected;
    if (selected == sizeof cases / sizeof cases[0]) return 2;
    UmiCodeGuardJob *job = (UmiCodeGuardJob *)(uintptr_t)1U;
    if (!strcmp(scenario, "arguments")) {
        CHECK(UmiCodeGuardJobCreate(".", NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardJobCreate(NULL, NULL, &job) == UMI_STATUS_INVALID_ARGUMENT && job == NULL);
        CHECK(UmiCodeGuardJobCreate("", NULL, &job) == UMI_STATUS_INVALID_ARGUMENT && job == NULL);
        char long_root[UMI_PATH_CAPACITY + 1U];
        memset(long_root, 'x', sizeof long_root); long_root[UMI_PATH_CAPACITY] = '\0';
        CHECK(UmiCodeGuardJobCreate(long_root, NULL, &job) == UMI_STATUS_CAPACITY_EXCEEDED && job == NULL);
        CHECK(UmiCodeGuardJobSubmit(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardJobCancel(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardJobRead(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardJobTake(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardJobDestroy(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCodeGuardJobDestroy(&job) == UMI_STATUS_OK && scans == 0U);
        return 0;
    }
    char root[32] = ".";
    UmiCodeGuardProfile profile = umi_codeguard_profile_default();
    profile.max_file_bytes = 17U;
    UmiStatus status = UmiCodeGuardJobCreate(root,
        !strcmp(scenario, "defaults") ? NULL : &profile, &job);
    if (!strcmp(scenario, "allocation") || !strcmp(scenario, "mutex-allocation") ||
        !strcmp(scenario, "task-allocation")) {
        CHECK(status == UMI_STATUS_OUT_OF_MEMORY && job == NULL && scans == 0U);
        CHECK(job_frees == (!strcmp(scenario, "allocation") ? 0U : 1U));
        return 0;
    }
    CHECK(status == UMI_STATUS_OK && job != NULL);
    active_job = job;
    char expected_root[UMI_PATH_CAPACITY];
    CHECK(umi_fs_current_directory(expected_root, sizeof expected_root) == UMI_STATUS_OK);
    strcpy(root, "changed-root");
    profile.max_file_bytes = 0U;
    profile.scan_c = 0;
    UmiCodeGuardResult *result = (UmiCodeGuardResult *)(uintptr_t)1U;
    UmiCodeGuardJobSnapshot initial = Snapshot(job);
    CHECK(initial.state == UMI_TASK_CREATED && initial.status == UMI_STATUS_BUSY);
    CHECK(initial.operation_status == UMI_STATUS_BUSY && !initial.operation_completed);
    CHECK(!initial.result_available && initial.summary.total == 0U);
    CHECK(UmiCodeGuardJobTake(job, &result) == UMI_STATUS_BUSY &&
        result == (UmiCodeGuardResult *)(uintptr_t)1U);
    if (!strcmp(scenario, "created")) {
        CHECK(UmiCodeGuardJobDestroy(&job) == UMI_STATUS_OK && job == NULL && scans == 0U);
        return 0;
    }
    UmiTaskQueue *queue = NULL;
    UmiTask *blocker = NULL, *filler = NULL;
    if (!strncmp(scenario, "queue", 5U) || !strncmp(scenario, "running", 7U) ||
        !strcmp(scenario, "wrong-owner")) {
        UmiTaskQueueConfig config = {.worker_count = 1U, .capacity = 1U};
        CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
    }
    if (!strcmp(scenario, "wrong-owner")) {
        WrongOwnerInput input = {.job = job, .queue = queue};
        UmiThread *thread = NULL;
        CHECK(umi_thread_start(WrongOwner, &input, &thread) == UMI_STATUS_OK);
        int code = -1;
        CHECK(umi_thread_join(thread, &code) == UMI_STATUS_OK && code == 0 && input.passed);
        umi_thread_destroy(thread);
        CHECK(Snapshot(job).state == UMI_TASK_CREATED && scans == 0U);
    } else if (!strcmp(scenario, "queue-stopped")) {
        CHECK(UmiTaskQueueRequestShutdown(queue, 0, 0) == UMI_STATUS_OK);
        CHECK(UmiCodeGuardJobSubmit(job, queue) == UMI_STATUS_INVALID_STATE);
        CHECK(Snapshot(job).state == UMI_TASK_CREATED && scans == 0U);
    } else {
        if (!strcmp(scenario, "queue-full") || !strcmp(scenario, "queued-cancel") ||
            !strncmp(scenario, "running", 7U)) {
            CHECK(umi_mutex_create(&gate_mutex) == UMI_STATUS_OK);
            CHECK(umi_condition_create(&gate_condition) == UMI_STATUS_OK);
        }
        if (!strcmp(scenario, "queue-full") || !strcmp(scenario, "queued-cancel")) {
            UmiTaskConfig block = {.label = "Hold queue", .function = Block};
            CHECK(umi_task_create(&block, &blocker) == UMI_STATUS_OK);
            CHECK(umi_task_queue_submit(queue, blocker) == UMI_STATUS_OK);
            AwaitGate();
            if (!strcmp(scenario, "queue-full")) {
                UmiTaskConfig fill = {.label = "Fill queue", .function = Noop};
                CHECK(umi_task_create(&fill, &filler) == UMI_STATUS_OK);
                CHECK(umi_task_queue_submit(queue, filler) == UMI_STATUS_OK);
                CHECK(UmiCodeGuardJobSubmit(job, queue) == UMI_STATUS_CAPACITY_EXCEEDED);
                CHECK(Snapshot(job).state == UMI_TASK_CREATED);
                ReleaseGate();
                CHECK(umi_task_queue_wait_idle(queue, 5000U) == UMI_STATUS_OK);
                CHECK(UmiCodeGuardJobSubmit(job, queue) == UMI_STATUS_OK);
            } else {
                CHECK(UmiCodeGuardJobSubmit(job, queue) == UMI_STATUS_OK);
                CHECK(Snapshot(job).state == UMI_TASK_QUEUED);
                CHECK(UmiCodeGuardJobDestroy(&job) == UMI_STATUS_BUSY && job == active_job);
                CHECK(UmiCodeGuardJobCancel(job) == UMI_STATUS_OK);
                CHECK(UmiCodeGuardJobTake(job, &result) == UMI_STATUS_CANCELLED);
                ReleaseGate();
            }
            CHECK(umi_task_queue_wait_idle(queue, 5000U) == UMI_STATUS_OK);
        } else if (!strncmp(scenario, "running", 7U)) {
            CHECK(UmiCodeGuardJobSubmit(job, queue) == UMI_STATUS_OK);
            AwaitGate();
            CHECK(Snapshot(job).state == UMI_TASK_RUNNING);
            CHECK(UmiCodeGuardJobDestroy(&job) == UMI_STATUS_BUSY && job == active_job);
            CHECK(UmiCodeGuardJobTake(job, &result) == UMI_STATUS_BUSY);
            if (!strcmp(scenario, "running-cancel")) CHECK(UmiCodeGuardJobCancel(job) == UMI_STATUS_OK);
            ReleaseGate();
            CHECK(umi_task_queue_wait_idle(queue, 5000U) == UMI_STATUS_OK);
        } else {
            if (!strcmp(scenario, "cancel-before")) CHECK(UmiCodeGuardJobCancel(job) == UMI_STATUS_OK);
            (void)umi_task_run(job->task); /* Created, never submitted: deterministic terminal boundary. */
        }
        UmiCodeGuardJobSnapshot done = Snapshot(job);
        UmiStatus expected = !strcmp(scenario, "failure") || !strcmp(scenario, "cancel-error")
            ? UMI_STATUS_IO_ERROR : !strcmp(scenario, "candidate-allocation")
            ? UMI_STATUS_OUT_OF_MEMORY : !strcmp(scenario, "cancel-before") ||
              !strcmp(scenario, "cancel-success") || !strcmp(scenario, "publication-cancel") ||
              !strcmp(scenario, "queued-cancel") ||
              !strcmp(scenario, "running-cancel") ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
        CHECK(done.status == expected);
        if (expected == UMI_STATUS_OK) {
            CHECK(done.state == UMI_TASK_SUCCEEDED && done.operation_status == UMI_STATUS_OK);
            CHECK(done.operation_completed && done.result_available && done.summary.total == 1U);
            if (!strcmp(scenario, "late-cancel")) {
                CHECK(UmiCodeGuardJobCancel(job) == UMI_STATUS_OK);
                CHECK(Snapshot(job).state == UMI_TASK_SUCCEEDED);
            }
            CHECK(UmiCodeGuardJobTake(job, &result) == UMI_STATUS_OK);
            CHECK(umi_codeguard_result_count(result) == 1U);
            CHECK(!Snapshot(job).result_available && Snapshot(job).summary.total == 1U);
            if (!strcmp(scenario, "double-take")) {
                UmiCodeGuardResult *again = result;
                CHECK(UmiCodeGuardJobTake(job, &again) == UMI_STATUS_NOT_FOUND && again == result);
            }
            CHECK(umi_path_equal(observed_root, expected_root));
            CHECK(observed_c == 1 && observed_limit == (!strcmp(scenario, "defaults")
                ? umi_codeguard_profile_default().max_file_bytes : 17U));
        } else {
            CHECK(!done.result_available && done.summary.total == 0U);
            CHECK(UmiCodeGuardJobTake(job, &result) == expected &&
                result == (UmiCodeGuardResult *)(uintptr_t)1U);
            result = NULL;
            if (!strcmp(scenario, "publication-cancel"))
                CHECK(done.state == UMI_TASK_CANCELLED && done.operation_completed &&
                    done.operation_status == UMI_STATUS_OK);
            if (!strcmp(scenario, "cancel-error"))
                CHECK(done.state == UMI_TASK_CANCELLED && done.operation_status == UMI_STATUS_IO_ERROR);
            if (!strcmp(scenario, "cancel-before") || !strcmp(scenario, "queued-cancel"))
                CHECK(!done.operation_completed && scans == 0U);
        }
    }
    if (!strcmp(scenario, "wrong-owner") || !strcmp(scenario, "queue-stopped")) result = NULL;
    CHECK(UmiCodeGuardJobDestroy(&job) == UMI_STATUS_OK && job == NULL && job_frees == 1U);
    /* A transferred result is independent of the destroyed job. */
    if (result != NULL) CHECK(umi_codeguard_result_count(result) == 1U);
    umi_codeguard_result_destroy(result);
    if (queue != NULL) CHECK(umi_task_queue_shutdown(queue, 0) == UMI_STATUS_OK);
    umi_task_queue_destroy(queue);
    umi_task_destroy(filler);
    umi_task_destroy(blocker);
    umi_condition_destroy(gate_condition);
    umi_mutex_destroy(gate_mutex);
    return 0;
}
