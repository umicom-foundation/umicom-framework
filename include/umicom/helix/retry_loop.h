/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/retry_loop.h
 * PURPOSE: Bound automated build, test and review retries in Helix v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* A bounded counter turns repeated automation into an explicit
 * policy instead of an infinite loop that keeps changing the workspace. */
#ifndef INCLUDE_UMICOM_HELIX_RETRY_LOOP_H
#define INCLUDE_UMICOM_HELIX_RETRY_LOOP_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix retry loop data shared with callers of this public contract.
 */
typedef struct UmiHelixRetryLoop {
    uint32_t maximum_attempts;
    uint32_t attempts_started;
    uint32_t failures;
    int completed;
} UmiHelixRetryLoop;

/**
 * Initialise helix retry loop from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_retry_loop_init(UmiHelixRetryLoop *loop,
                                      uint32_t maximum_attempts);
/**
 * Provide the helix retry loop begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_retry_loop_begin(UmiHelixRetryLoop *loop);
/**
 * Provide the helix retry loop record operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_retry_loop_record(UmiHelixRetryLoop *loop,
                                       UmiStatus result);
/**
 * Provide the helix retry loop can retry operation used by this module and its client
 * applications.
 */
int umi_helix_retry_loop_can_retry(const UmiHelixRetryLoop *loop);

#ifdef __cplusplus
}
#endif

#endif
