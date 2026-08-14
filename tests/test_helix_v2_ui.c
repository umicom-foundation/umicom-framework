/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_v2_ui.c
 * PURPOSE: Verify toolkit-neutral Helix v2 operational panes.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/helix_ui_v2.h"
int main(void)
{
    UmiHelixOrchestratorV2 runtime;
    UmiHelixOrchestratorConfigV2 config =
        umi_helix_orchestrator_v2_config_default();
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    assert(umi_helix_orchestrator_v2_init(&runtime, &config) == UMI_STATUS_OK);
    assert(umi_helix_ui_v2_overview("helix.overview", &runtime, &view)
           == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(view, "helix.agents", &value)
           == UMI_STATUS_OK);
    assert(value.integer_value == 5);
    umi_ui_view_model_destroy(view);
    return 0;
}
