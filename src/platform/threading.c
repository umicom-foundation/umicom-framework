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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct UmiMutex { CRITICAL_SECTION value; };
struct UmiCondition { CONDITION_VARIABLE value; };
struct UmiThread {
    HANDLE handle;
    DWORD identifier;
    int exit_code;
    int joined;
    UmiThreadEntry entry;
    void *user_data;
};

/*
 * Provide the thread entry win32 operation used by this module and its client
 * applications.
 */
static DWORD WINAPI umi_thread_entry_win32(LPVOID value)
{
    UmiThread *thread = (UmiThread *)value;
    thread->exit_code = thread->entry(thread->user_data);
    return (DWORD)thread->exit_code;
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
    if (WaitForSingleObject(thread->handle, INFINITE) != WAIT_OBJECT_0) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    thread->joined = 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exit_code != NULL) *out_exit_code = thread->exit_code;
    return UMI_STATUS_OK;
}

/* Release or reset state held by thread so the same storage can be reused safely. */
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
    int exit_code;
    UmiThreadEntry entry;
    void *user_data;
};

/*
 * Provide the thread entry posix operation used by this module and its client
 * applications.
 */
static void *umi_thread_entry_posix(void *value)
{
    UmiThread *thread = (UmiThread *)value;
    thread->exit_code = thread->entry(thread->user_data);
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
    if (pthread_join(thread->handle, NULL) != 0) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    thread->joined = 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exit_code != NULL) *out_exit_code = thread->exit_code;
    return UMI_STATUS_OK;
}

/* Release or reset state held by thread so the same storage can be reused safely. */
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

/* Provide the thread sleep ms operation used by this module and its client applications. */
void umi_thread_sleep_ms(uint32_t milliseconds)
{
    struct timespec duration;
    duration.tv_sec = (time_t)(milliseconds / 1000U);
    duration.tv_nsec = (long)(milliseconds % 1000U) * 1000000L;
    (void)nanosleep(&duration, NULL);
}

/* Provide the thread current id operation used by this module and its client applications. */
uint64_t umi_thread_current_id(void)
{
    return (uint64_t)(uintptr_t)pthread_self();
}

#endif
