/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_v2_retry.c
 * PURPOSE: Verify bounded retry accounting and completion.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/retry_loop_v2.h"
int main(void)
{
    UmiHelixRetryLoopV2 loop;
    assert(umi_helix_retry_loop_v2_init(&loop, 2U) == UMI_STATUS_OK);
    assert(umi_helix_retry_loop_v2_begin(&loop) == UMI_STATUS_OK);
    assert(umi_helix_retry_loop_v2_record(&loop, UMI_STATUS_IO_ERROR)
           == UMI_STATUS_OK);
    assert(umi_helix_retry_loop_v2_can_retry(&loop));
    assert(umi_helix_retry_loop_v2_begin(&loop) == UMI_STATUS_OK);
    assert(umi_helix_retry_loop_v2_record(&loop, UMI_STATUS_OK) == UMI_STATUS_OK);
    assert(!umi_helix_retry_loop_v2_can_retry(&loop));
    return 0;
}
