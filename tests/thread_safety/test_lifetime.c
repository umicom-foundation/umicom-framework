/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/thread_safety/test_lifetime.c
 * PURPOSE:
 *   Exercise thread ownership through public contracts; checks stay active in Release. Each
 *   CTest case starts a fresh process. No files, application or network is used.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Exercise thread ownership through public contracts; checks stay active in Release.
 * Each CTest case starts a fresh process. No files, application or network is used.
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/threading.h"
#include <limits.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <pthread.h>
#include <signal.h>
#include <time.h>
#endif

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); exit(1); } } while (0)
#define OK(expr) CHECK((expr) == UMI_STATUS_OK)

typedef struct Gate {
    atomic_int entered;
    atomic_int go;
    atomic_int leaving;
    UmiThread *thread;
    int value;
    char published[96];
} Gate;
static Gate gate;

static void Wait(atomic_int *flag)
{
    for (unsigned i = 0; i < 10000U; ++i) {
        if (atomic_load_explicit(flag, memory_order_acquire)) return;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0 /* worker did not reach its checkpoint */);
}
static void StartGate(Gate *g, int value)
{
    memset(g, 0, sizeof(*g));
    atomic_init(&g->entered, 0); atomic_init(&g->go, 0); atomic_init(&g->leaving, 0);
    g->value = value;
}
static int Gated(void *context)
{
    Gate *g = context;
    const int result = g->value;
    atomic_store_explicit(&g->entered, 1, memory_order_release);
    Wait(&g->go);
    (void)memcpy(g->published, "A background Notes index is ready.", 34U);
    atomic_store_explicit(&g->leaving, 1, memory_order_release);
    return result;
}
static int Immediate(void *context) { return *(const int *)context; }
static UmiStatus ResultEventually(UmiThread *thread, int *result)
{
    UmiStatus status = UMI_STATUS_BUSY;
    for (unsigned i = 0; i < 10000U && status == UMI_STATUS_BUSY; ++i) {
        status = UmiThreadTryGetExitCode(thread, result);
        if (status == UMI_STATUS_BUSY) umi_thread_sleep_ms(1U);
    }
    return status;
}
static void Invalid(void)
{
    UmiThread *thread = NULL; int result = 73;
    CHECK(umi_thread_start(NULL, NULL, &thread) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_thread_start(Immediate, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_thread_join(NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiThreadTryGetExitCode(NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiThreadRelease(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(result == 73);
    OK(UmiThreadRelease(&thread)); OK(UmiThreadRelease(&thread));
    umi_thread_destroy(NULL);
}
static void Result(int value)
{
    UmiThread *thread = NULL; int result = 75;
    OK(umi_thread_start(Immediate, &value, &thread));
    OK(umi_thread_join(thread, &result)); CHECK(result == value);
    result = 75; OK(UmiThreadTryGetExitCode(thread, &result)); CHECK(result == value);
    CHECK(umi_thread_join(thread, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    OK(UmiThreadRelease(&thread)); CHECK(thread == NULL); OK(UmiThreadRelease(&thread));
}
static void Poll(void)
{
    int result = 73;
    StartGate(&gate, -19);
    OK(umi_thread_start(Gated, &gate, &gate.thread)); Wait(&gate.entered);
    CHECK(UmiThreadTryGetExitCode(gate.thread, &result) == UMI_STATUS_BUSY);
    CHECK(result == 73);
    CHECK(UmiThreadTryGetExitCode(gate.thread, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    atomic_store(&gate.go, 1);
    OK(ResultEventually(gate.thread, &result)); CHECK(result == -19);
    /* Acquire of the completion publishes the worker's ordinary result data. */
    CHECK(strcmp(gate.published, "A background Notes index is ready.") == 0);
    OK(umi_thread_join(gate.thread, &result)); CHECK(result == -19);
    OK(UmiThreadRelease(&gate.thread));
}
static void ReleaseRunning(int legacy)
{
    StartGate(&gate, 29);
    OK(umi_thread_start(Gated, &gate, &gate.thread)); Wait(&gate.entered);
    if (legacy) { umi_thread_destroy(gate.thread); gate.thread = NULL; }
    else OK(UmiThreadRelease(&gate.thread));
    CHECK(gate.thread == NULL); CHECK(!atomic_load(&gate.leaving));
    atomic_store(&gate.go, 1); Wait(&gate.leaving);
    /* Static caller storage outlives the detached worker. Fault tests separately
     * observe exact Framework allocation release; this is the ASan race case. */
    umi_thread_sleep_ms(20U);
}
static int SelfJoin(void *context)
{
    Gate *g = context; int result = 97;
    Wait(&g->go);
    CHECK(umi_thread_join(g->thread, &result) == UMI_STATUS_INVALID_STATE);
    CHECK(result == 97);
    return 41;
}
static void Self(void)
{
    StartGate(&gate, 0);
    OK(umi_thread_start(SelfJoin, &gate, &gate.thread));
    atomic_store(&gate.go, 1);
    int result = 0; OK(umi_thread_join(gate.thread, &result)); CHECK(result == 41);
    OK(UmiThreadRelease(&gate.thread));
}
static void Many(void)
{
    for (int i = 0; i < 1000; ++i) Result(i - 500);
}
static Gate detached[256];
static void ManyDetached(void)
{
    for (unsigned i = 0; i < 256U; ++i) {
        StartGate(&detached[i], (int)i);
        OK(umi_thread_start(Gated, &detached[i], &detached[i].thread));
        OK(UmiThreadRelease(&detached[i].thread));
        atomic_store(&detached[i].go, 1);
    }
    for (unsigned i = 0; i < 256U; ++i) Wait(&detached[i].leaving);
    umi_thread_sleep_ms(30U);
}
typedef struct Queue { UmiMutex *mutex; UmiCondition *condition; int count; } Queue;
static int Produce(void *context)
{
    Queue *q = context;
    for (int i = 0; i < 1000; ++i) {
        OK(umi_mutex_lock(q->mutex)); ++q->count;
        OK(umi_condition_signal(q->condition)); OK(umi_mutex_unlock(q->mutex));
    }
    return 1000;
}
static void Condition(void)
{
    Queue q = {0}; UmiThread *thread = NULL; int result = 0;
    OK(umi_mutex_create(&q.mutex)); OK(umi_condition_create(&q.condition));
    OK(umi_thread_start(Produce, &q, &thread));
    OK(umi_mutex_lock(q.mutex));
    while (q.count < 1000) {
        UmiStatus status = umi_condition_wait_for(q.condition, q.mutex, 500U);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_TIMEOUT);
    }
    OK(umi_mutex_unlock(q.mutex)); OK(umi_thread_join(thread, &result)); CHECK(result == 1000);
    OK(UmiThreadRelease(&thread)); umi_condition_destroy(q.condition); umi_mutex_destroy(q.mutex);
}
static void ZeroWait(void)
{
    UmiMutex *mutex = NULL; UmiCondition *condition = NULL;
    OK(umi_mutex_create(&mutex)); OK(umi_condition_create(&condition));
    OK(umi_mutex_lock(mutex));
    CHECK(umi_condition_wait_for(condition, mutex, 0U) == UMI_STATUS_TIMEOUT);
    OK(umi_mutex_unlock(mutex));
    umi_condition_destroy(condition); umi_mutex_destroy(mutex); umi_thread_sleep_ms(0U);
}
static void MutexChecks(void)
{
    CHECK(umi_mutex_create(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_mutex_lock(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_mutex_unlock(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_condition_create(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_condition_signal(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_condition_broadcast(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_condition_wait(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_condition_wait_for(NULL, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
    umi_mutex_destroy(NULL); umi_condition_destroy(NULL);
}
#ifndef _WIN32
static int NativeExit(void *context) { (void)context; pthread_exit(NULL); return 9; }
static void ExitWithoutResult(void)
{
    UmiThread *thread = NULL; int result = 77;
    OK(umi_thread_start(NativeExit, NULL, &thread));
    CHECK(ResultEventually(thread, &result) == UMI_STATUS_UNAVAILABLE); CHECK(result == 77);
    CHECK(umi_thread_join(thread, &result) == UMI_STATUS_UNAVAILABLE); CHECK(result == 77);
    CHECK(umi_thread_join(thread, &result) == UMI_STATUS_INVALID_ARGUMENT);
    OK(UmiThreadRelease(&thread));
}
static pthread_t cancellable;
static int NativeCancellation(void *context)
{
    Gate *g = context; cancellable = pthread_self(); atomic_store(&g->entered, 1);
    for (;;) umi_thread_sleep_ms(1U);
    return 0;
}
static void CancelNative(void)
{
    int result = 55; StartGate(&gate, 0);
    OK(umi_thread_start(NativeCancellation, &gate, &gate.thread)); Wait(&gate.entered);
    CHECK(pthread_cancel(cancellable) == 0);
    CHECK(umi_thread_join(gate.thread, &result) == UMI_STATUS_UNAVAILABLE); CHECK(result == 55);
    OK(UmiThreadRelease(&gate.thread));
}
static pthread_t sleeper;
static volatile sig_atomic_t signalSeen;
static void SignalHandler(int number) { (void)number; signalSeen = 1; }
static int SendSignal(void *context)
{
    (void)context; umi_thread_sleep_ms(20U); CHECK(pthread_kill(sleeper, SIGUSR1) == 0); return 0;
}
static void SleepInterrupted(void)
{
    struct sigaction action; memset(&action, 0, sizeof action);
    action.sa_handler = SignalHandler; sigemptyset(&action.sa_mask);
    CHECK(sigaction(SIGUSR1, &action, NULL) == 0);
    struct timespec before, after; UmiThread *thread = NULL; sleeper = pthread_self();
    OK(umi_thread_start(SendSignal, NULL, &thread));
    CHECK(clock_gettime(CLOCK_MONOTONIC, &before) == 0);
    umi_thread_sleep_ms(120U);
    CHECK(clock_gettime(CLOCK_MONOTONIC, &after) == 0);
    double milliseconds = (double)(after.tv_sec - before.tv_sec) * 1000.0 +
                          (double)(after.tv_nsec - before.tv_nsec) / 1000000.0;
    OK(umi_thread_join(thread, NULL)); OK(UmiThreadRelease(&thread));
    CHECK(signalSeen); CHECK(milliseconds >= 115.0);
    printf("Interrupted 120 ms sleep observed %.3f ms.\n", milliseconds);
}
/* Completion is not join: deliberately hold a TLS destructor after the entry
 * callback has returned. Poll must expose the result without claiming teardown. */
static pthread_key_t cleanupKey;
static atomic_int cleanupEntered, cleanupGo;
static void TlsCleanup(void *value)
{
    (void)value; atomic_store(&cleanupEntered, 1); Wait(&cleanupGo);
}
static int ReturnBeforeTls(void *context)
{
    CHECK(pthread_setspecific(cleanupKey, context) == 0); return 71;
}
static void CompletionBeforeTeardown(void)
{
    UmiThread *thread = NULL; int result = 0;
    CHECK(pthread_key_create(&cleanupKey, TlsCleanup) == 0);
    OK(umi_thread_start(ReturnBeforeTls, &gate, &thread)); Wait(&cleanupEntered);
    OK(UmiThreadTryGetExitCode(thread, &result)); CHECK(result == 71);
    atomic_store(&cleanupGo, 1);
    OK(umi_thread_join(thread, &result)); CHECK(result == 71);
    OK(UmiThreadRelease(&thread)); CHECK(pthread_key_delete(cleanupKey) == 0);
}
#endif
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!strcmp(argv[1], "invalid")) Invalid();
    else if (!strcmp(argv[1], "zero")) Result(0);
    else if (!strcmp(argv[1], "positive")) Result(17);
    else if (!strcmp(argv[1], "negative")) Result(-17);
    else if (!strcmp(argv[1], "int_min")) Result(INT_MIN);
    else if (!strcmp(argv[1], "int_max")) Result(INT_MAX);
    else if (!strcmp(argv[1], "poll")) Poll();
    else if (!strcmp(argv[1], "release_running")) ReleaseRunning(0);
    else if (!strcmp(argv[1], "destroy_running")) ReleaseRunning(1);
    else if (!strcmp(argv[1], "self_join")) Self();
    else if (!strcmp(argv[1], "join_stress")) Many();
    else if (!strcmp(argv[1], "release_stress")) ManyDetached();
    else if (!strcmp(argv[1], "condition")) Condition();
    else if (!strcmp(argv[1], "zero_wait")) ZeroWait();
    else if (!strcmp(argv[1], "mutex_invalid")) MutexChecks();
#ifndef _WIN32
    else if (!strcmp(argv[1], "native_exit")) ExitWithoutResult();
    else if (!strcmp(argv[1], "native_cancel")) CancelNative();
    else if (!strcmp(argv[1], "signal_sleep")) SleepInterrupted();
    else if (!strcmp(argv[1], "tls_completion")) CompletionBeforeTeardown();
#endif
    else CHECK(0 /* unknown test */);
    return 0;
}
