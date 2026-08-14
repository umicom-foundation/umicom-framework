/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/retry_loop.h
 * PURPOSE: Bound automated build, test and review retries in Helix v2.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: A bounded counter turns repeated automation into an explicit
 * policy instead of an infinite loop that keeps changing the workspace. */
#ifndef INCLUDE_UMICOM_HELIX_RETRY_LOOP_H
#define INCLUDE_UMICOM_HELIX_RETRY_LOOP_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixRetryLoop {
    uint32_t maximum_attempts;
    uint32_t attempts_started;
    uint32_t failures;
    int completed;
} UmiHelixRetryLoop;

UmiStatus umi_helix_retry_loop_init(UmiHelixRetryLoop *loop,
                                      uint32_t maximum_attempts);
UmiStatus umi_helix_retry_loop_begin(UmiHelixRetryLoop *loop);
UmiStatus umi_helix_retry_loop_record(UmiHelixRetryLoop *loop,
                                       UmiStatus result);
int umi_helix_retry_loop_can_retry(const UmiHelixRetryLoop *loop);

#ifdef __cplusplus
}
#endif

#endif
