/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_11_context_merge.c
 *
 * PURPOSE:
 *   Validate context merge sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_merge.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiContextMerge state;
    umi_context_merge_init(&state);
    assert(umi_context_merge_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_merge_field(&state,0U),"alpha") == 0);
    assert(umi_context_merge_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_merge_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_merge_covers_sequence(&state,10U));
    assert(umi_context_merge_covers_sequence(&state,11U));
    assert(umi_context_merge_validate(&state) == UMI_STATUS_OK);
    return 0;
}
