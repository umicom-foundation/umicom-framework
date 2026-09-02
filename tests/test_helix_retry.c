/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_retry.c
 * PURPOSE: Verify bounded retry accounting and completion.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/retry_loop.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiHelixRetryLoop loop;
    assert(umi_helix_retry_loop_init(&loop, 2U) == UMI_STATUS_OK);
    assert(umi_helix_retry_loop_begin(&loop) == UMI_STATUS_OK);
    assert(umi_helix_retry_loop_record(&loop, UMI_STATUS_IO_ERROR)
           == UMI_STATUS_OK);
    assert(umi_helix_retry_loop_can_retry(&loop));
    assert(umi_helix_retry_loop_begin(&loop) == UMI_STATUS_OK);
    assert(umi_helix_retry_loop_record(&loop, UMI_STATUS_OK) == UMI_STATUS_OK);
    assert(!umi_helix_retry_loop_can_retry(&loop));
    return 0;
}
