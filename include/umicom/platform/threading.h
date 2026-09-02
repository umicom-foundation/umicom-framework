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

#ifdef __cplusplus
}
#endif

#endif
