/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_17_context_panel_service.c
 *
 * PURPOSE:
 *   Validate context panel service sequence accounting, bounded fields and failure evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_panel_service.h"
int main(void)
{
    UmiContextPanelService state;
    umi_context_panel_service_init(&state);
    assert(umi_context_panel_service_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_panel_service_field(&state,0U),"alpha") == 0);
    assert(umi_context_panel_service_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_panel_service_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_panel_service_covers_sequence(&state,10U));
    assert(umi_context_panel_service_covers_sequence(&state,11U));
    assert(umi_context_panel_service_validate(&state) == UMI_STATUS_OK);
    return 0;
}
