/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_29_context_statistics.c
 *
 * PURPOSE:
 *   Validate context statistics sequence accounting, bounded fields and failure evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_statistics.h"
int main(void)
{
    UmiContextStatistics state;
    umi_context_statistics_init(&state);
    assert(umi_context_statistics_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_statistics_field(&state,0U),"alpha") == 0);
    assert(umi_context_statistics_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_statistics_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_statistics_covers_sequence(&state,10U));
    assert(umi_context_statistics_covers_sequence(&state,11U));
    assert(umi_context_statistics_validate(&state) == UMI_STATUS_OK);
    return 0;
}
