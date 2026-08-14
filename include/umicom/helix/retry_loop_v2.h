/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/retry_loop_v2.h
 * PURPOSE: Bound automated build, test and review retries in Helix v2.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: A bounded counter turns repeated automation into an explicit
 * policy instead of an infinite loop that keeps changing the workspace. */
#ifndef INCLUDE_UMICOM_HELIX_RETRY_LOOP_V2_H
#define INCLUDE_UMICOM_HELIX_RETRY_LOOP_V2_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixRetryLoopV2 {
    uint32_t maximum_attempts;
    uint32_t attempts_started;
    uint32_t failures;
    int completed;
} UmiHelixRetryLoopV2;

UmiStatus umi_helix_retry_loop_v2_init(UmiHelixRetryLoopV2 *loop,
                                      uint32_t maximum_attempts);
UmiStatus umi_helix_retry_loop_v2_begin(UmiHelixRetryLoopV2 *loop);
UmiStatus umi_helix_retry_loop_v2_record(UmiHelixRetryLoopV2 *loop,
                                       UmiStatus result);
int umi_helix_retry_loop_v2_can_retry(const UmiHelixRetryLoopV2 *loop);

#ifdef __cplusplus
}
#endif

#endif
