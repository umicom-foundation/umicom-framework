/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/retry_loop.c
 * PURPOSE: Implement bounded and observable Helix v2 retry state.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/retry_loop.h"

#include <string.h>

UmiStatus umi_helix_retry_loop_init(UmiHelixRetryLoop *loop,
                                      uint32_t maximum_attempts)
{
    if (loop == NULL || maximum_attempts == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(loop, 0, sizeof(*loop));
    loop->maximum_attempts = maximum_attempts;
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_retry_loop_begin(UmiHelixRetryLoop *loop)
{
    if (loop == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (loop->completed || loop->attempts_started >= loop->maximum_attempts) {
        return UMI_STATUS_INVALID_STATE;
    }
    loop->attempts_started++;
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_retry_loop_record(UmiHelixRetryLoop *loop,
                                       UmiStatus result)
{
    if (loop == NULL || loop->attempts_started == 0U || loop->completed) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (result == UMI_STATUS_OK) loop->completed = 1;
    else loop->failures++;
    return UMI_STATUS_OK;
}

int umi_helix_retry_loop_can_retry(const UmiHelixRetryLoop *loop)
{
    return loop != NULL && !loop->completed &&
           loop->attempts_started < loop->maximum_attempts;
}
