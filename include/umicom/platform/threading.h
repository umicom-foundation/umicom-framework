/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/threading.h
 *
 * PURPOSE:
 *   Provide portable C23 mutex, condition-variable and thread contracts used
 *   by Framework services without exposing Win32 or pthread types publicly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_THREADING_H
#define UMICOM_PLATFORM_THREADING_H

#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiMutex UmiMutex;
typedef struct UmiCondition UmiCondition;
typedef struct UmiThread UmiThread;
typedef int (*UmiThreadEntry)(void *user_data);

UmiStatus umi_mutex_create(UmiMutex **out_mutex);
void umi_mutex_destroy(UmiMutex *mutex);
UmiStatus umi_mutex_lock(UmiMutex *mutex);
UmiStatus umi_mutex_unlock(UmiMutex *mutex);

UmiStatus umi_condition_create(UmiCondition **out_condition);
void umi_condition_destroy(UmiCondition *condition);
UmiStatus umi_condition_wait(UmiCondition *condition, UmiMutex *mutex);
UmiStatus umi_condition_wait_for(UmiCondition *condition,
                                 UmiMutex *mutex,
                                 uint32_t timeout_ms);
UmiStatus umi_condition_signal(UmiCondition *condition);
UmiStatus umi_condition_broadcast(UmiCondition *condition);

UmiStatus umi_thread_start(UmiThreadEntry entry,
                           void *user_data,
                           UmiThread **out_thread);
UmiStatus umi_thread_join(UmiThread *thread, int *out_exit_code);
void umi_thread_destroy(UmiThread *thread);
void umi_thread_sleep_ms(uint32_t milliseconds);
uint64_t umi_thread_current_id(void);

#ifdef __cplusplus
}
#endif

#endif
