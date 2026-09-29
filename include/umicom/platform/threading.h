/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/threading.h
 *
 * PURPOSE:
 *   Provide portable C23 mutex, condition-variable and thread contracts used
 *   by Framework services without exposing Win32 or pthread types publicly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_THREADING_H
#define UMICOM_PLATFORM_THREADING_H

#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the mutex data shared with callers of this public contract.
 */
typedef struct UmiMutex UmiMutex;
/**
 * Represent the condition data shared with callers of this public contract.
 */
typedef struct UmiCondition UmiCondition;
/**
 * Represent the thread data shared with callers of this public contract.
 */
typedef struct UmiThread UmiThread;
typedef int (*UmiThreadEntry)(void *user_data);

/**
 * Initialise mutex from caller-provided values so later operations receive a known state.
 */
UmiStatus umi_mutex_create(UmiMutex **out_mutex);
/**
 * Release or reset state held by mutex so the same storage can be reused safely.
 */
void umi_mutex_destroy(UmiMutex *mutex);
/**
 * Provide the mutex lock operation used by this module and its client applications.
 */
UmiStatus umi_mutex_lock(UmiMutex *mutex);
/**
 * Provide the mutex unlock operation used by this module and its client applications.
 */
UmiStatus umi_mutex_unlock(UmiMutex *mutex);

/**
 * Initialise condition from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_condition_create(UmiCondition **out_condition);
/**
 * Release or reset state held by condition so the same storage can be reused safely.
 */
void umi_condition_destroy(UmiCondition *condition);
/**
 * Provide the condition wait operation used by this module and its client applications.
 */
UmiStatus umi_condition_wait(UmiCondition *condition, UmiMutex *mutex);
/**
 * Provide the condition wait for operation used by this module and its client
 * applications.
 */
UmiStatus umi_condition_wait_for(UmiCondition *condition,
                                 UmiMutex *mutex,
                                 uint32_t timeout_ms);
/**
 * Provide the condition signal operation used by this module and its client applications.
 */
UmiStatus umi_condition_signal(UmiCondition *condition);
/**
 * Provide the condition broadcast operation used by this module and its client
 * applications.
 */
UmiStatus umi_condition_broadcast(UmiCondition *condition);

/**
 * Provide the thread start operation used by this module and its client applications.
 */
UmiStatus umi_thread_start(UmiThreadEntry entry,
                           void *user_data,
                           UmiThread **out_thread);
/**
 * Provide the thread join operation used by this module and its client applications.
 */
UmiStatus umi_thread_join(UmiThread *thread, int *out_exit_code);
/**
 * Release or reset state held by thread so the same storage can be reused safely.
 */
void umi_thread_destroy(UmiThread *thread);
/**
 * Provide the thread sleep ms operation used by this module and its client applications.
 */
void umi_thread_sleep_ms(uint32_t milliseconds);
/**
 * Provide the thread current id operation used by this module and its client applications.
 */
uint64_t umi_thread_current_id(void);

/** Observe an entry callback without waiting. OK copies its integer result;
 * BUSY means the callback has not returned. UNAVAILABLE means POSIX cleanup ran
 * without an integer result. On failure, outExitCode is unchanged.
 *
 * This is NOT a join: native TLS destructors may still be executing. Keep the
 * handle alive during this call. Join before releasing caller data that any
 * thread-exit cleanup can still use. */
UmiStatus UmiThreadTryGetExitCode(const UmiThread *thread, int *outExitCode);

/** Release the controller's one owning handle, clearing it on success. NULL
 * storage is invalid; an already-empty handle is a successful no-op. A native
 * release error leaves the handle owned by the caller so it can be investigated.
 *
 * An unjoined POSIX thread is detached; a Windows handle is closed. This does
 * not stop, cancel or join the callback. Framework retains its own control block
 * until the worker releases it. user_data and anything it points to remain the
 * caller's responsibility, and must live until their last worker use.
 *
 * Controller operations on one handle must be serialised. Do not destroy,
 * release, poll or join through another pointer while release is running. Native
 * asynchronous cancellation, ExitThread and TerminateThread are outside this
 * portable contract. Prefer cooperative cancellation followed by join.
 *
 * umi_thread_destroy remains a void compatibility wrapper around this operation.
 * umi_thread_join refuses self-join with INVALID_STATE. A joined POSIX callback
 * that left via native exit/cancellation has no int result: join returns
 * UNAVAILABLE, leaves its output unchanged, and consumes the join obligation. */
UmiStatus UmiThreadRelease(UmiThread **inOutThread);

/** Return whether this build supplies a genuinely nonblocking native join.
 * Windows and non-Android Linux are supported. Other platforms return zero;
 * no implementation substitutes a potentially blocking join. This is a build
 * capability, not a guarantee that any particular thread has finished. */
int UmiThreadCanTryJoin(void);

/** Join only when native thread teardown has finished. BUSY leaves the owned
 * handle joinable and unchanged. OK consumes its join obligation, not its
 * owner reference: release it separately. Rejoining is INVALID_ARGUMENT;
 * self-join is INVALID_STATE on supported platforms. Native failures return
 * INTERNAL_ERROR; unsupported builds return NOT_IMPLEMENTED.
 * OK does not mean the callback succeeded or returned an int. Query its result
 * separately using UmiThreadTryGetExitCode. Like blocking join, owner-side
 * join/observe/release calls must be serialised, and must own a live reference. */
UmiStatus UmiThreadTryJoin(UmiThread *thread);

#ifdef __cplusplus
}
#endif

#endif
