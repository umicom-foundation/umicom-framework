/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_30_context_throttle.c
 *
 * PURPOSE:
 *   Validate context throttle sequence accounting, bounded fields and failure evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_throttle.h"
int main(void)
{
    UmiContextThrottle state;
    umi_context_throttle_init(&state);
    assert(umi_context_throttle_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_throttle_field(&state,0U),"alpha") == 0);
    assert(umi_context_throttle_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_throttle_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_throttle_covers_sequence(&state,10U));
    assert(umi_context_throttle_covers_sequence(&state,11U));
    assert(umi_context_throttle_validate(&state) == UMI_STATUS_OK);
    return 0;
}
