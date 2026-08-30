/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/retry_loop_v2.c
 * PURPOSE: Implement bounded and observable Helix v2 retry state.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/retry_loop_v2.h"

#include <string.h>

UmiStatus umi_helix_retry_loop_v2_init(UmiHelixRetryLoopV2 *loop,
                                      uint32_t maximum_attempts)
{
    if (loop == NULL || maximum_attempts == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(loop, 0, sizeof(*loop));
    loop->maximum_attempts = maximum_attempts;
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_retry_loop_v2_begin(UmiHelixRetryLoopV2 *loop)
{
    if (loop == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (loop->completed || loop->attempts_started >= loop->maximum_attempts) {
        return UMI_STATUS_INVALID_STATE;
    }
    loop->attempts_started++;
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_retry_loop_v2_record(UmiHelixRetryLoopV2 *loop,
                                       UmiStatus result)
{
    if (loop == NULL || loop->attempts_started == 0U || loop->completed) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (result == UMI_STATUS_OK) loop->completed = 1;
    else loop->failures++;
    return UMI_STATUS_OK;
}

int umi_helix_retry_loop_v2_can_retry(const UmiHelixRetryLoopV2 *loop)
{
    return loop != NULL && !loop->completed &&
           loop->attempts_started < loop->maximum_attempts;
}
