/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/queue_shutdown/test_shutdown.c
 * PURPOSE:
 *   Native lifecycle tests. Gates establish ordering; sleeps only yield the CPU. Tests own
 *   every callback payload until the corresponding native join.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: tests/queue_shutdown/test_shutdown.c
 * Native lifecycle tests. Gates establish ordering; sleeps only yield the CPU.
 * Tests own every callback payload until the corresponding native join.
 *---------------------------------------------------------------------------*/
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"
#include "umicom/platform/clock.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__linux__)
#include <pthread.h>
#endif

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)

typedef struct Work {
    atomic_int entered;
    atomic_int allow;
    atomic_int finished;
    atomic_int cancellationSeen;
    int cooperate;
    UmiStatus result;
    UmiThread *thread;
    UmiTaskQueue *queue;
    UmiTaskQueue *other;
    UmiStatus selfResult;
    UmiStatus releaseResult;
    int selfCheck;
#if defined(__linux__)
    pthread_key_t key;
    int installKey;
    atomic_int cleanupEntered;
    atomic_int cleanupAllow;
    atomic_int cleanupFinished;
#endif
} Work;

static void Init(Work *w, int ready)
{
    memset(w, 0, sizeof(*w));
    atomic_init(&w->entered, 0);
    atomic_init(&w->allow, ready);
    atomic_init(&w->finished, 0);
    atomic_init(&w->cancellationSeen, 0);
#if defined(__linux__)
    atomic_init(&w->cleanupEntered, 0);
    atomic_init(&w->cleanupAllow, 0);
    atomic_init(&w->cleanupFinished, 0);
#endif
}

static uint64_t Now(void)
{
    UmiClock clock = umi_clock_system();
    return clock.monotonic_nanoseconds(&clock);
}

static void WaitFlag(const atomic_int *flag)
{
    uint64_t start = Now();
    while (!atomic_load_explicit(flag, memory_order_acquire)) {
        CHECK(Now() - start < 5000000000ULL);
        umi_thread_sleep_ms(1U);
    }
}

static UmiStatus Finish(UmiTaskQueue *queue)
{
    uint64_t start = Now();
    UmiStatus status;
    do {
        status = UmiTaskQueueTryFinishShutdown(queue);
        if (status != UMI_STATUS_BUSY) return status;
        CHECK(Now() - start < 5000000000ULL);
        umi_thread_sleep_ms(1U);
    } while (1);
}

static UmiStatus Join(UmiThread *thread)
{
    uint64_t start = Now();
    UmiStatus status;
    do {
        status = UmiThreadTryJoin(thread);
        if (status != UMI_STATUS_BUSY) return status;
        CHECK(Now() - start < 5000000000ULL);
        umi_thread_sleep_ms(1U);
    } while (1);
}

#if defined(__linux__)
static void Cleanup(void *data)
{
    Work *w = data;
    atomic_store_explicit(&w->cleanupEntered, 1, memory_order_release);
    WaitFlag(&w->cleanupAllow);
    atomic_store_explicit(&w->cleanupFinished, 1, memory_order_release);
}
#endif

static int ThreadWork(void *data)
{
    Work *w = data;
    atomic_store_explicit(&w->entered, 1, memory_order_release);
    WaitFlag(&w->allow);
    if (w->selfCheck) w->selfResult = UmiThreadTryJoin(w->thread);
#if defined(__linux__)
    if (w->installKey) CHECK(pthread_setspecific(w->key, w) == 0);
#endif
    atomic_store_explicit(&w->finished, 1, memory_order_release);
    return -73;
}

static UmiStatus TaskWork(UmiTaskContext *context, void *data)
{
    Work *w = data;
    uint64_t start = Now();
    atomic_store_explicit(&w->entered, 1, memory_order_release);
    while (!atomic_load_explicit(&w->allow, memory_order_acquire)) {
        if (umi_task_context_is_cancelled(context)) {
            atomic_store(&w->cancellationSeen, 1);
            if (w->cooperate) return UMI_STATUS_CANCELLED;
        }
        CHECK(Now() - start < 5000000000ULL);
        umi_thread_sleep_ms(1U);
    }
    if (w->selfCheck) {
        UmiTaskQueue *alias = w->queue;
        w->selfResult = UmiTaskQueueTryFinishShutdown(w->queue);
        w->releaseResult = UmiTaskQueueReleaseStopped(&alias);
        CHECK(alias == w->queue);
        CHECK(UmiTaskQueueRequestShutdown(w->queue, 0, 0) == UMI_STATUS_OK);
    }
    if (w->other != NULL) CHECK(Finish(w->other) == UMI_STATUS_OK);
#if defined(__linux__)
    if (w->installKey) CHECK(pthread_setspecific(w->key, w) == 0);
#endif
    atomic_store_explicit(&w->finished, 1, memory_order_release);
    return w->result;
}

static UmiTaskQueue *Queue(size_t workers, size_t capacity)
{
    UmiTaskQueueConfig config = {workers, capacity};
    UmiTaskQueue *queue = NULL;
    CHECK(umi_task_queue_create(&config, &queue) == UMI_STATUS_OK);
    return queue;
}

static UmiTask *Task(Work *w)
{
    UmiTaskConfig config = {"Notes index", TaskWork, w, NULL, NULL};
    UmiTask *task = NULL;
    CHECK(umi_task_create(&config, &task) == UMI_STATUS_OK);
    return task;
}

static UmiTaskQueueShutdownSnapshot Snapshot(UmiTaskQueue *queue)
{
    UmiTaskQueueShutdownSnapshot snapshot;
    CHECK(UmiTaskQueueCaptureShutdown(queue, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.tasks.submitted == snapshot.tasks.queued +
        snapshot.tasks.running + snapshot.tasks.completed);
    CHECK(snapshot.tasks.completed >= snapshot.tasks.cancelled + snapshot.tasks.failed);
    return snapshot;
}

static void TestNull(void)
{
    UmiTaskQueue *none = NULL;
    UmiTaskQueueShutdownSnapshot out, before;
    memset(&out, 0x6d, sizeof(out));
    memcpy(&before, &out, sizeof(out));
    CHECK(UmiThreadTryJoin(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTaskQueueTryFinishShutdown(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTaskQueueCaptureShutdown(NULL, &out) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&out, &before, sizeof(out)) == 0);
    CHECK(UmiTaskQueueReleaseStopped(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTaskQueueReleaseStopped(&none) == UMI_STATUS_OK);
}

static void TestThread(const char *name)
{
    Work w;
    UmiThread *thread = NULL;
    int exitCode = 99;
    Init(&w, 0);
    w.selfCheck = strcmp(name, "thread_self") == 0;
    CHECK(umi_thread_start(ThreadWork, &w, &thread) == UMI_STATUS_OK);
    w.thread = thread;
    WaitFlag(&w.entered);
    CHECK(UmiThreadTryJoin(thread) == UMI_STATUS_BUSY);
    CHECK(UmiThreadTryGetExitCode(thread, &exitCode) == UMI_STATUS_BUSY);
    CHECK(exitCode == 99);
    atomic_store_explicit(&w.allow, 1, memory_order_release);
    CHECK(Join(thread) == UMI_STATUS_OK);
    CHECK(UmiThreadTryGetExitCode(thread, &exitCode) == UMI_STATUS_OK);
    CHECK(exitCode == -73);
    if (w.selfCheck) CHECK(w.selfResult == UMI_STATUS_INVALID_STATE);
    if (strcmp(name, "thread_rejoin") == 0) {
        CHECK(UmiThreadTryJoin(thread) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_thread_join(thread, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    CHECK(UmiThreadRelease(&thread) == UMI_STATUS_OK);
    CHECK(thread == NULL);
}

static void TestEmpty(const char *name)
{
    UmiTaskQueue *queue = Queue(2U, 4U), *original = queue;
    UmiTaskQueueShutdownSnapshot snapshot = Snapshot(queue);
    CHECK(snapshot.phase == UMI_TASK_QUEUE_ACCEPTING);
    CHECK(snapshot.nonblockingJoinAvailable);
    CHECK(UmiTaskQueueTryFinishShutdown(queue) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_BUSY);
    CHECK(queue == original);
    CHECK(UmiTaskQueueCaptureShutdown(queue, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiTaskQueueRequestShutdown(queue, 0, 0) == UMI_STATUS_OK);
    CHECK(Finish(queue) == UMI_STATUS_OK);
    snapshot = Snapshot(queue);
    CHECK(snapshot.phase == UMI_TASK_QUEUE_STOPPED);
    CHECK(snapshot.tasks.completed == 0U);
    if (strcmp(name, "empty_repeat") == 0) {
        CHECK(UmiTaskQueueTryFinishShutdown(queue) == UMI_STATUS_OK);
        CHECK(UmiTaskQueueRequestShutdown(queue, 1, 1) == UMI_STATUS_OK);
        CHECK(umi_task_queue_shutdown(queue, 1) == UMI_STATUS_OK);
    }
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK);
    CHECK(queue == NULL);
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK);
    CHECK(snapshot.phase == UMI_TASK_QUEUE_STOPPED);
}

static void TestWork(const char *name)
{
    UmiTaskQueue *queue = Queue(1U, 4U), *original = queue;
    Work work[3];
    UmiTask *tasks[3];
    int pending = strcmp(name, "cancel_pending") == 0 || strcmp(name, "escalate") == 0;
    int running = strcmp(name, "cancel_running") == 0 || strcmp(name, "escalate") == 0 ||
        strcmp(name, "ignore_cancel") == 0;
    int releaseTasks = strcmp(name, "caller_release") == 0;
    for (size_t i = 0; i < 3U; ++i) {
        Init(&work[i], i != 0U);
        if (i == 0U) work[i].cooperate = strcmp(name, "cancel_running") == 0;
        if (i == 1U && strcmp(name, "mixed_results") == 0) work[i].result = UMI_STATUS_IO_ERROR;
        tasks[i] = Task(&work[i]);
        CHECK(umi_task_queue_submit(queue, tasks[i]) == UMI_STATUS_OK);
        if (i == 0U) WaitFlag(&work[0].entered);
    }
    if (strcmp(name, "mixed_results") == 0) {
        CHECK(umi_task_cancel(tasks[2]) == UMI_STATUS_OK);
    }
    if (releaseTasks) for (size_t i = 0; i < 3U; ++i) umi_task_destroy(tasks[i]);
    CHECK(UmiTaskQueueRequestShutdown(queue, 0, 0) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueTryFinishShutdown(queue) == UMI_STATUS_BUSY);
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_BUSY);
    CHECK(queue == original);
    CHECK(Snapshot(queue).phase == UMI_TASK_QUEUE_STOP_REQUESTED);
    if (pending || running) CHECK(UmiTaskQueueRequestShutdown(queue, pending, running) == UMI_STATUS_OK);
    if (pending) {
        UmiTaskQueueShutdownSnapshot snap = Snapshot(queue);
        CHECK(snap.tasks.queued == 0U && snap.tasks.cancelled == 2U);
        CHECK(!atomic_load(&work[1].entered) && !atomic_load(&work[2].entered));
    }
    if (strcmp(name, "ignore_cancel") == 0) {
        WaitFlag(&work[0].cancellationSeen);
        CHECK(UmiTaskQueueTryFinishShutdown(queue) == UMI_STATUS_BUSY);
        CHECK(!atomic_load(&work[0].finished));
    }
    if (strcmp(name, "post_stop") == 0) {
        Work rejected;
        Init(&rejected, 1);
        UmiTask *extra = Task(&rejected);
        CHECK(umi_task_queue_submit(queue, extra) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_task_state(extra) == UMI_TASK_CREATED);
        umi_task_destroy(extra);
    }
    atomic_store_explicit(&work[0].allow, 1, memory_order_release);
    if (strcmp(name, "legacy_finish") == 0)
        CHECK(umi_task_queue_shutdown(queue, 0) == UMI_STATUS_OK);
    else CHECK(Finish(queue) == UMI_STATUS_OK);
    UmiTaskQueueShutdownSnapshot snap = Snapshot(queue);
    CHECK(snap.phase == UMI_TASK_QUEUE_STOPPED && snap.tasks.completed == 3U);
    CHECK(snap.tasks.running == 0U && snap.tasks.queued == 0U);
    if (strcmp(name, "mixed_results") == 0) {
        CHECK(snap.tasks.failed == 1U && snap.tasks.cancelled == 1U);
    } else CHECK(snap.tasks.cancelled == (uint64_t)(2 * pending + running));
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK);
    if (!releaseTasks) for (size_t i = 0; i < 3U; ++i) umi_task_destroy(tasks[i]);
}

static void TestWorker(void)
{
    UmiTaskQueue *queue = Queue(1U, 2U);
    Work work;
    Init(&work, 0);
    work.selfCheck = 1;
    work.queue = queue;
    UmiTask *task = Task(&work);
    CHECK(umi_task_queue_submit(queue, task) == UMI_STATUS_OK);
    WaitFlag(&work.entered);
    atomic_store_explicit(&work.allow, 1, memory_order_release);
    CHECK(umi_task_wait(task, 5000U) == UMI_STATUS_OK);
    CHECK(work.selfResult == UMI_STATUS_INVALID_STATE);
    CHECK(work.releaseResult == UMI_STATUS_INVALID_STATE);
    CHECK(Finish(queue) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK);
    umi_task_destroy(task);
}

static void TestOther(void)
{
    UmiTaskQueue *other = Queue(1U, 2U), *queue = Queue(1U, 2U);
    Work work;
    Init(&work, 1);
    work.other = other;
    CHECK(UmiTaskQueueRequestShutdown(other, 0, 0) == UMI_STATUS_OK);
    UmiTask *task = Task(&work);
    CHECK(umi_task_queue_submit(queue, task) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueRequestShutdown(queue, 0, 0) == UMI_STATUS_OK);
    CHECK(Finish(queue) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueReleaseStopped(&other) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK);
    umi_task_destroy(task);
}

static void TestCancelAll(void)
{
    UmiTaskQueue *queue = Queue(1U, 2U);
    Work first, second;
    Init(&first, 0); first.cooperate = 1;
    Init(&second, 1);
    UmiTask *a = Task(&first), *b = Task(&second);
    CHECK(umi_task_queue_submit(queue, a) == UMI_STATUS_OK);
    WaitFlag(&first.entered);
    CHECK(UmiTaskQueueCancelAll(queue) == UMI_STATUS_OK);
    CHECK(umi_task_queue_wait_idle(queue, 5000U) == UMI_STATUS_OK);
    CHECK(Snapshot(queue).phase == UMI_TASK_QUEUE_ACCEPTING);
    CHECK(umi_task_queue_submit(queue, b) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueRequestShutdown(queue, 0, 0) == UMI_STATUS_OK);
    CHECK(Finish(queue) == UMI_STATUS_OK);
    CHECK(Snapshot(queue).tasks.completed == 2U);
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK);
    umi_task_destroy(a); umi_task_destroy(b);
}

/* A blocking lifecycle helper and the polling controller share the existing
 * joining exclusion. The poller must return BUSY, not race a second join. */
typedef struct JoinRequest {
    UmiTaskQueue *queue;
    UmiStatus result;
} JoinRequest;

static int BlockingQueueJoin(void *data)
{
    JoinRequest *request = data;
    request->result = umi_task_queue_shutdown(request->queue, 0);
    return 0;
}

static void TestCompetingJoin(void)
{
    UmiTaskQueue *queue = Queue(1U, 2U);
    Work work;
    Init(&work, 0);
    UmiTask *task = Task(&work);
    UmiThread *joiner = NULL;
    JoinRequest request = {queue, UMI_STATUS_INVALID_STATE};
    CHECK(umi_task_queue_submit(queue, task) == UMI_STATUS_OK);
    WaitFlag(&work.entered);
    CHECK(umi_thread_start(BlockingQueueJoin, &request, &joiner) == UMI_STATUS_OK);
    uint64_t start = Now();
    while (Snapshot(queue).phase != UMI_TASK_QUEUE_JOINING) {
        CHECK(Now() - start < 5000000000ULL);
        umi_thread_sleep_ms(1U);
    }
    CHECK(UmiTaskQueueTryFinishShutdown(queue) == UMI_STATUS_BUSY);
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_BUSY);
    atomic_store_explicit(&work.allow, 1, memory_order_release);
    CHECK(Join(joiner) == UMI_STATUS_OK);
    CHECK(request.result == UMI_STATUS_OK);
    CHECK(UmiThreadRelease(&joiner) == UMI_STATUS_OK);
    CHECK(Snapshot(queue).phase == UMI_TASK_QUEUE_STOPPED);
    CHECK(Finish(queue) == UMI_STATUS_OK);
    CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK);
    umi_task_destroy(task);
}

static void TestStress(void)
{
    for (size_t pass = 0U; pass < 40U; ++pass) {
        UmiTaskQueue *queue = Queue(4U, 64U);
        Work work[32]; UmiTask *tasks[32];
        for (size_t i = 0U; i < 32U; ++i) {
            Init(&work[i], 1);
            tasks[i] = Task(&work[i]);
            CHECK(umi_task_queue_submit(queue, tasks[i]) == UMI_STATUS_OK);
            (void)Snapshot(queue);
        }
        CHECK(UmiTaskQueueRequestShutdown(queue, 0, 0) == UMI_STATUS_OK);
        CHECK(Finish(queue) == UMI_STATUS_OK);
        CHECK(Snapshot(queue).tasks.completed == 32U);
        CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK);
        for (size_t i = 0U; i < 32U; ++i) {
            CHECK(atomic_load(&work[i].finished));
            CHECK(umi_task_result(tasks[i]) == UMI_STATUS_OK);
            umi_task_destroy(tasks[i]);
        }
    }
}

#if defined(__linux__)
static int NoResult(void *data)
{
    (void)data;
    pthread_exit(NULL);
    return 0;
}

static void TestNoResult(void)
{
    UmiThread *thread = NULL;
    int code = 123;
    CHECK(umi_thread_start(NoResult, NULL, &thread) == UMI_STATUS_OK);
    CHECK(Join(thread) == UMI_STATUS_OK);
    CHECK(UmiThreadTryGetExitCode(thread, &code) == UMI_STATUS_UNAVAILABLE);
    CHECK(code == 123);
    CHECK(UmiThreadRelease(&thread) == UMI_STATUS_OK);
}

static void TestTls(int useQueue)
{
    Work w;
    Init(&w, 1);
    w.installKey = 1;
    CHECK(pthread_key_create(&w.key, Cleanup) == 0);
    if (useQueue) {
        UmiTaskQueue *queue = Queue(1U, 2U), *same = queue;
        UmiTask *task = Task(&w);
        CHECK(umi_task_queue_submit(queue, task) == UMI_STATUS_OK);
        CHECK(UmiTaskQueueRequestShutdown(queue, 0, 0) == UMI_STATUS_OK);
        WaitFlag(&w.cleanupEntered);
        CHECK(umi_task_queue_wait_idle(queue, 1U) == UMI_STATUS_OK);
        CHECK(Snapshot(queue).tasks.running == 0U);
        CHECK(UmiTaskQueueTryFinishShutdown(queue) == UMI_STATUS_BUSY);
        CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_BUSY && queue == same);
        atomic_store(&w.cleanupAllow, 1);
        CHECK(Finish(queue) == UMI_STATUS_OK);
        CHECK(atomic_load(&w.cleanupFinished));
        CHECK(UmiTaskQueueReleaseStopped(&queue) == UMI_STATUS_OK);
        umi_task_destroy(task);
    } else {
        UmiThread *thread = NULL;
        int code = 0;
        CHECK(umi_thread_start(ThreadWork, &w, &thread) == UMI_STATUS_OK);
        WaitFlag(&w.cleanupEntered);
        CHECK(UmiThreadTryGetExitCode(thread, &code) == UMI_STATUS_OK && code == -73);
        CHECK(UmiThreadTryJoin(thread) == UMI_STATUS_BUSY);
        atomic_store(&w.cleanupAllow, 1);
        CHECK(Join(thread) == UMI_STATUS_OK);
        CHECK(atomic_load(&w.cleanupFinished));
        CHECK(UmiThreadRelease(&thread) == UMI_STATUS_OK);
    }
    CHECK(pthread_key_delete(w.key) == 0);
}
#endif

int main(int argc, char **argv)
{
    const char *name;
    if (argc != 2) return 2;
    name = argv[1];
    if (strcmp(name, "null") == 0) TestNull();
    else {
        if (!UmiThreadCanTryJoin()) return 77;
        if (strncmp(name, "thread_", 7U) == 0) TestThread(name);
        else if (strncmp(name, "empty", 5U) == 0) TestEmpty(name);
        else if (strcmp(name, "worker_self") == 0) TestWorker();
        else if (strcmp(name, "other_queue") == 0) TestOther();
        else if (strcmp(name, "cancel_all") == 0) TestCancelAll();
        else if (strcmp(name, "stress") == 0) TestStress();
        else if (strcmp(name, "competing_join") == 0) TestCompetingJoin();
#if defined(__linux__)
        else if (strcmp(name, "no_result") == 0) TestNoResult();
        else if (strcmp(name, "tls_thread") == 0) TestTls(0);
        else if (strcmp(name, "tls_queue") == 0) TestTls(1);
#endif
        else if (strcmp(name, "drain") == 0 || strcmp(name, "cancel_pending") == 0 ||
            strcmp(name, "cancel_running") == 0 || strcmp(name, "ignore_cancel") == 0 ||
            strcmp(name, "escalate") == 0 || strcmp(name, "post_stop") == 0 ||
            strcmp(name, "caller_release") == 0 || strcmp(name, "legacy_finish") == 0 ||
            strcmp(name, "mixed_results") == 0) TestWork(name);
        else return 2;
    }
    printf("PASS %s\n", name);
    return 0;
}
