/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/retry_loop.c
 * PURPOSE: Implement bounded and observable Helix v2 retry state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/retry_loop.h"

#include <string.h>

/*
 * Initialise helix retry loop from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_retry_loop_init(UmiHelixRetryLoop *loop,
                                      uint32_t maximum_attempts)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (loop == NULL || maximum_attempts == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(loop, 0, sizeof(*loop));
    loop->maximum_attempts = maximum_attempts;
    return UMI_STATUS_OK;
}

/*
 * Provide the helix retry loop begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_retry_loop_begin(UmiHelixRetryLoop *loop)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (loop == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (loop->completed || loop->attempts_started >= loop->maximum_attempts) {
        return UMI_STATUS_INVALID_STATE;
    }
    loop->attempts_started++;
    return UMI_STATUS_OK;
}

/*
 * Provide the helix retry loop record operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_retry_loop_record(UmiHelixRetryLoop *loop,
                                       UmiStatus result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (loop == NULL || loop->attempts_started == 0U || loop->completed) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result == UMI_STATUS_OK) loop->completed = 1;
    /* Use this fallback path when the earlier condition does not apply. */
    else loop->failures++;
    return UMI_STATUS_OK;
}

/*
 * Provide the helix retry loop can retry operation used by this module and its client
 * applications.
 */
int umi_helix_retry_loop_can_retry(const UmiHelixRetryLoop *loop)
{
    return loop != NULL && !loop->completed &&
           loop->attempts_started < loop->maximum_attempts;
}
