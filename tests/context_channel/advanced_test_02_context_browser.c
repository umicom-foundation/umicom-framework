/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_02_context_browser.c
 *
 * PURPOSE:
 *   Validate context browser sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_browser.h"
int main(void)
{
    UmiContextBrowser state;
    umi_context_browser_init(&state);
    assert(umi_context_browser_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_browser_field(&state,0U),"alpha") == 0);
    assert(umi_context_browser_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_browser_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_browser_covers_sequence(&state,10U));
    assert(umi_context_browser_covers_sequence(&state,11U));
    assert(umi_context_browser_validate(&state) == UMI_STATUS_OK);
    return 0;
}
