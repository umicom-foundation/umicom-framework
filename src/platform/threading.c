/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/threading.c
 *
 * PURPOSE:
 *   Implement the threading behavior for
 *   Umicom Framework.
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

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/threading.c
 *
 * PURPOSE:
 *   Implement portable Framework synchronisation and thread lifecycle using
 *   Win32 primitives on Windows and pthreads on POSIX systems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/threading.h"

#include <stdlib.h>
#include <stdatomic.h>

/* The controller and the worker each own one reference to the private control
 * block. Native detach/handle closure does not end a running callback. Keep the
 * block alive until both owners have released it; caller data is still borrowed.
 * A completion observation publishes the callback result, not native thread
 * teardown. Join remains the operation that waits for that teardown. */
static void ThreadReleaseReference(UmiThread *thread);
static void ThreadWorkerCleanup(void *context);

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct UmiMutex { CRITICAL_SECTION value; };
struct UmiCondition { CONDITION_VARIABLE value; };
struct UmiThread {
    HANDLE handle;
    DWORD identifier;
    atomic_uint references;
    atomic_int completion; /* 0: executing; 1: returned an int; 2: no result. */
    int exit_code;
    int joined;
    UmiThreadEntry entry;
    void *user_data;
};

/*
 * Provide the thread entry win32 operation used by this module and its client
 * applications.
 */
/* Publishing a result must not write to storage already released by the controller.
 * The worker now retains its own lifetime reference and copies the exit result
 * before releasing that reference.
 * The prior implementation is retained for engineering review. */
#if 0
static DWORD WINAPI umi_thread_entry_win32(LPVOID value)
{
    UmiThread *thread = (UmiThread *)value;
    thread->exit_code = thread->entry(thread->user_data);
    return (DWORD)thread->exit_code;
}
#endif

static DWORD WINAPI umi_thread_entry_win32(LPVOID value)
{
    UmiThread *thread = (UmiThread *)value;
    const int result = thread->entry(thread->user_data);
    thread->exit_code = result;
    atomic_store_explicit(&thread->completion, 1, memory_order_release);
    ThreadWorkerCleanup(thread);
    /* Cleanup may free the block. Return the local result, never read it again. */
    return (DWORD)result;
}

/* Initialise mutex from caller-provided values so later operations receive a known state. */
UmiStatus umi_mutex_create(UmiMutex **out_mutex)
{
    UmiMutex *mutex;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_mutex = NULL;
    mutex = (UmiMutex *)calloc(1U, sizeof(*mutex));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mutex == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    InitializeCriticalSection(&mutex->value);
    *out_mutex = mutex;
    return UMI_STATUS_OK;
}

/* Release or reset state held by mutex so the same storage can be reused safely. */
void umi_mutex_destroy(UmiMutex *mutex)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mutex == NULL) return;
    DeleteCriticalSection(&mutex->value);
    free(mutex);
}

/* Provide the mutex lock operation used by this module and its client applications. */
UmiStatus umi_mutex_lock(UmiMutex *mutex)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    EnterCriticalSection(&mutex->value);
    return UMI_STATUS_OK;
}

/* Provide the mutex unlock operation used by this module and its client applications. */
UmiStatus umi_mutex_unlock(UmiMutex *mutex)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    LeaveCriticalSection(&mutex->value);
    return UMI_STATUS_OK;
}

/*
 * Initialise condition from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_condition_create(UmiCondition **out_condition)
{
    UmiCondition *condition;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_condition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_condition = NULL;
    condition = (UmiCondition *)calloc(1U, sizeof(*condition));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    InitializeConditionVariable(&condition->value);
    *out_condition = condition;
    return UMI_STATUS_OK;
}

/* Release or reset state held by condition so the same storage can be reused safely. */
void umi_condition_destroy(UmiCondition *condition)
{
    free(condition);
}

/* Provide the condition wait operation used by this module and its client applications. */
UmiStatus umi_condition_wait(UmiCondition *condition, UmiMutex *mutex)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL || mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return SleepConditionVariableCS(&condition->value,
                                    &mutex->value,
                                    INFINITE)
        ? UMI_STATUS_OK
        : UMI_STATUS_INTERNAL_ERROR;
}

/*
 * Provide the condition wait for operation used by this module and its client
 * applications.
 */
UmiStatus umi_condition_wait_for(UmiCondition *condition,
                                 UmiMutex *mutex,
                                 uint32_t timeout_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL || mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (SleepConditionVariableCS(&condition->value,
                                 &mutex->value,
                                 (DWORD)timeout_ms)) {
        return UMI_STATUS_OK;
    }
    return GetLastError() == ERROR_TIMEOUT
        ? UMI_STATUS_TIMEOUT
        : UMI_STATUS_INTERNAL_ERROR;
}

/* Provide the condition signal operation used by this module and its client applications. */
UmiStatus umi_condition_signal(UmiCondition *condition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    WakeConditionVariable(&condition->value);
    return UMI_STATUS_OK;
}

/*
 * Provide the condition broadcast operation used by this module and its client
 * applications.
 */
UmiStatus umi_condition_broadcast(UmiCondition *condition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    WakeAllConditionVariable(&condition->value);
    return UMI_STATUS_OK;
}

/* Provide the thread start operation used by this module and its client applications. */
UmiStatus umi_thread_start(UmiThreadEntry entry,
                           void *user_data,
                           UmiThread **out_thread)
{
    UmiThread *thread;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || out_thread == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_thread = NULL;
    thread = (UmiThread *)calloc(1U, sizeof(*thread));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (thread == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    atomic_init(&thread->references, 2U);
    atomic_init(&thread->completion, 0);
    thread->entry = entry;
    thread->user_data = user_data;
    thread->handle = CreateThread(NULL, 0U, umi_thread_entry_win32,
                                  thread, 0U, &thread->identifier);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (thread->handle == NULL) {
        free(thread);
        return UMI_STATUS_UNAVAILABLE;
    }
    *out_thread = thread;
    return UMI_STATUS_OK;
}

/* Provide the thread join operation used by this module and its client applications. */
UmiStatus umi_thread_join(UmiThread *thread, int *out_exit_code)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (thread == NULL || thread->handle == NULL || thread->joined) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    /* Waiting on oneself cannot complete; refuse it before an infinite wait. */
    if (GetCurrentThreadId() == thread->identifier) return UMI_STATUS_INVALID_STATE;
    if (WaitForSingleObject(thread->handle, INFINITE) != WAIT_OBJECT_0) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    thread->joined = 1;
    /* Joining reclaimed the native join obligation even when no int returned. */
    if (atomic_load_explicit(&thread->completion, memory_order_acquire) != 1)
        return UMI_STATUS_UNAVAILABLE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exit_code != NULL) *out_exit_code = thread->exit_code;
    return UMI_STATUS_OK;
}

/* Release or reset state held by thread so the same storage can be reused safely. */
/* Unconditional free could race the still-running entry wrapper. The checked
 * release closes/detaches the native handle and drops only the controller
 * reference. Destruction remains nonblocking and does not cancel the callback.
 * The prior implementation is retained for engineering review. */
#if 0
void umi_thread_destroy(UmiThread *thread)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (thread == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (thread->handle != NULL) CloseHandle(thread->handle);
    free(thread);
}
#endif

void umi_thread_destroy(UmiThread *thread)
{
    /* Preserve the void compatibility API. New controller code can inspect a
     * native release failure using UmiThreadRelease instead. */
    UmiThread *owned = thread;
    (void)UmiThreadRelease(&owned);
}

/* Provide the thread sleep ms operation used by this module and its client applications. */
void umi_thread_sleep_ms(uint32_t milliseconds)
{
    Sleep((DWORD)milliseconds);
}

/* Provide the thread current id operation used by this module and its client applications. */
uint64_t umi_thread_current_id(void)
{
    return (uint64_t)GetCurrentThreadId();
}

#else

#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

struct UmiMutex { pthread_mutex_t value; };
struct UmiCondition { pthread_cond_t value; };
struct UmiThread {
    pthread_t handle;
    int joined;
    atomic_uint references;
    atomic_int completion; /* 0: executing; 1: returned an int; 2: no result. */
    int exit_code;
    UmiThreadEntry entry;
    void *user_data;
};

/*
 * Provide the thread entry posix operation used by this module and its client
 * applications.
 */
/* Detaching only releases the native join obligation; it does not finish the
 * callback. A worker-owned reference now protects the result storage until the
 * wrapper has finished with it, including POSIX cleanup paths.
 * The prior implementation is retained for engineering review. */
#if 0
static void *umi_thread_entry_posix(void *value)
{
    UmiThread *thread = (UmiThread *)value;
    thread->exit_code = thread->entry(thread->user_data);
    return NULL;
}
#endif

static void *umi_thread_entry_posix(void *value)
{
    UmiThread *thread = (UmiThread *)value;
    /* The cleanup also releases Framework memory if a foreign POSIX callback
     * exits through pthread_exit or deferred cancellation. That is not a normal
     * integer result, and does not make forced cancellation generally safe. */
    pthread_cleanup_push(ThreadWorkerCleanup, thread);
    thread->exit_code = thread->entry(thread->user_data);
    atomic_store_explicit(&thread->completion, 1, memory_order_release);
    pthread_cleanup_pop(1);
    return NULL;
}

/* Initialise mutex from caller-provided values so later operations receive a known state. */
UmiStatus umi_mutex_create(UmiMutex **out_mutex)
{
    UmiMutex *mutex;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_mutex = NULL;
    mutex = (UmiMutex *)calloc(1U, sizeof(*mutex));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mutex == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (pthread_mutex_init(&mutex->value, NULL) != 0) {
        free(mutex);
        return UMI_STATUS_INTERNAL_ERROR;
    }
    *out_mutex = mutex;
    return UMI_STATUS_OK;
}

/* Release or reset state held by mutex so the same storage can be reused safely. */
void umi_mutex_destroy(UmiMutex *mutex)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mutex == NULL) return;
    (void)pthread_mutex_destroy(&mutex->value);
    free(mutex);
}

/* Provide the mutex lock operation used by this module and its client applications. */
UmiStatus umi_mutex_lock(UmiMutex *mutex)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return pthread_mutex_lock(&mutex->value) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

/* Provide the mutex unlock operation used by this module and its client applications. */
UmiStatus umi_mutex_unlock(UmiMutex *mutex)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return pthread_mutex_unlock(&mutex->value) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

/*
 * Initialise condition from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_condition_create(UmiCondition **out_condition)
{
    UmiCondition *condition;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_condition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_condition = NULL;
    condition = (UmiCondition *)calloc(1U, sizeof(*condition));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (pthread_cond_init(&condition->value, NULL) != 0) {
        free(condition);
        return UMI_STATUS_INTERNAL_ERROR;
    }
    *out_condition = condition;
    return UMI_STATUS_OK;
}

/* Release or reset state held by condition so the same storage can be reused safely. */
void umi_condition_destroy(UmiCondition *condition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL) return;
    (void)pthread_cond_destroy(&condition->value);
    free(condition);
}

/* Provide the condition wait operation used by this module and its client applications. */
UmiStatus umi_condition_wait(UmiCondition *condition, UmiMutex *mutex)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL || mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return pthread_cond_wait(&condition->value, &mutex->value) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

/*
 * Provide the condition wait for operation used by this module and its client
 * applications.
 */
UmiStatus umi_condition_wait_for(UmiCondition *condition,
                                 UmiMutex *mutex,
                                 uint32_t timeout_ms)
{
    struct timespec deadline;
    int result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL || mutex == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (clock_gettime(CLOCK_REALTIME, &deadline) != 0) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    deadline.tv_sec += (time_t)(timeout_ms / 1000U);
    deadline.tv_nsec += (long)(timeout_ms % 1000U) * 1000000L;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (deadline.tv_nsec >= 1000000000L) {
        deadline.tv_sec += 1;
        deadline.tv_nsec -= 1000000000L;
    }
    result = pthread_cond_timedwait(&condition->value, &mutex->value, &deadline);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result == 0) return UMI_STATUS_OK;
    return result == ETIMEDOUT ? UMI_STATUS_TIMEOUT : UMI_STATUS_INTERNAL_ERROR;
}

/* Provide the condition signal operation used by this module and its client applications. */
UmiStatus umi_condition_signal(UmiCondition *condition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return pthread_cond_signal(&condition->value) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

/*
 * Provide the condition broadcast operation used by this module and its client
 * applications.
 */
UmiStatus umi_condition_broadcast(UmiCondition *condition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (condition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return pthread_cond_broadcast(&condition->value) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

/* Provide the thread start operation used by this module and its client applications. */
UmiStatus umi_thread_start(UmiThreadEntry entry,
                           void *user_data,
                           UmiThread **out_thread)
{
    UmiThread *thread;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || out_thread == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_thread = NULL;
    thread = (UmiThread *)calloc(1U, sizeof(*thread));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (thread == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    atomic_init(&thread->references, 2U);
    atomic_init(&thread->completion, 0);
    thread->entry = entry;
    thread->user_data = user_data;
    /* Apply this branch only when its contract condition is satisfied. */
    if (pthread_create(&thread->handle, NULL,
                       umi_thread_entry_posix, thread) != 0) {
        free(thread);
        return UMI_STATUS_UNAVAILABLE;
    }
    *out_thread = thread;
    return UMI_STATUS_OK;
}

/* Provide the thread join operation used by this module and its client applications. */
UmiStatus umi_thread_join(UmiThread *thread, int *out_exit_code)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (thread == NULL || thread->joined) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    /* POSIX does not define joining the calling thread as a valid operation. */
    if (pthread_equal(pthread_self(), thread->handle)) return UMI_STATUS_INVALID_STATE;
    if (pthread_join(thread->handle, NULL) != 0) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    thread->joined = 1;
    /* Joining reclaimed the native join obligation even when no int returned. */
    if (atomic_load_explicit(&thread->completion, memory_order_acquire) != 1)
        return UMI_STATUS_UNAVAILABLE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exit_code != NULL) *out_exit_code = thread->exit_code;
    return UMI_STATUS_OK;
}

/* Release or reset state held by thread so the same storage can be reused safely. */
/* Unconditional free could race the still-running entry wrapper. The checked
 * release closes/detaches the native handle and drops only the controller
 * reference. Destruction remains nonblocking and does not cancel the callback.
 * The prior implementation is retained for engineering review. */
#if 0
void umi_thread_destroy(UmiThread *thread)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (thread == NULL) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!thread->joined) (void)pthread_detach(thread->handle);
    free(thread);
}
#endif

void umi_thread_destroy(UmiThread *thread)
{
    /* Preserve the void compatibility API. New controller code can inspect a
     * native release failure using UmiThreadRelease instead. */
    UmiThread *owned = thread;
    (void)UmiThreadRelease(&owned);
}

/* Provide the thread sleep ms operation used by this module and its client applications. */
void umi_thread_sleep_ms(uint32_t milliseconds)
{
    struct timespec duration;
    duration.tv_sec = (time_t)(milliseconds / 1000U);
    duration.tv_nsec = (long)(milliseconds % 1000U) * 1000000L;
/* A signal could return from the old sleep before its requested interval elapsed.
 * Resume the POSIX-reported remainder; stop rather than loop on other errors.
 * The prior implementation is retained for engineering review. */
#if 0
    (void)nanosleep(&duration, NULL);
#endif

    /* A signal is not completion of the requested delay. Resume its remainder. */
    while (nanosleep(&duration, &duration) != 0 && errno == EINTR) { }
}

/* Provide the thread current id operation used by this module and its client applications. */
uint64_t umi_thread_current_id(void)
{
    return (uint64_t)(uintptr_t)pthread_self();
}

#endif


/* Private lifetime accounting is shared by both native adapters. The last
 * release acquires the other owner's writes before reclaiming the allocation. */
static void ThreadReleaseReference(UmiThread *thread)
{
    if (atomic_fetch_sub_explicit(&thread->references, 1U,
                                  memory_order_acq_rel) == 1U)
        free(thread);
}

static void ThreadWorkerCleanup(void *context)
{
    UmiThread *thread = (UmiThread *)context;
    if (atomic_load_explicit(&thread->completion, memory_order_relaxed) == 0)
        atomic_store_explicit(&thread->completion, 2, memory_order_release);
    ThreadReleaseReference(thread);
}

UmiStatus UmiThreadTryGetExitCode(const UmiThread *thread, int *outExitCode)
{
    int completion;
    if (thread == NULL || outExitCode == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    completion = atomic_load_explicit(&thread->completion, memory_order_acquire);
    if (completion == 0) return UMI_STATUS_BUSY;
    if (completion != 1) return UMI_STATUS_UNAVAILABLE;
    *outExitCode = thread->exit_code;
    return UMI_STATUS_OK;
}

UmiStatus UmiThreadRelease(UmiThread **inOutThread)
{
    UmiThread *thread;
    if (inOutThread == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    thread = *inOutThread;
    if (thread == NULL) return UMI_STATUS_OK;
#ifdef _WIN32
    if (thread->handle != NULL && !CloseHandle(thread->handle))
        return UMI_STATUS_IO_ERROR;
    thread->handle = NULL;
#else
    if (!thread->joined && pthread_detach(thread->handle) != 0)
        return UMI_STATUS_INTERNAL_ERROR;
#endif
    /* Clear the caller's handle before the final reference can free the block.
     * This is ownership transfer, not synchronisation with other callers. */
    *inOutThread = NULL;
    ThreadReleaseReference(thread);
    return UMI_STATUS_OK;
}
