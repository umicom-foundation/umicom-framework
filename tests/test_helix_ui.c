/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_ui.c
 * PURPOSE: Verify toolkit-neutral Helix v2 operational panes.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/helix_ui.h"
int main(void)
{
    UmiHelixOrchestrator runtime;
    UmiHelixOrchestratorConfig config =
        umi_helix_orchestrator_config_default();
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    assert(umi_helix_orchestrator_init(&runtime, &config) == UMI_STATUS_OK);
    assert(umi_helix_ui_overview("helix.overview", &runtime, &view)
           == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(view, "helix.agents", &value)
           == UMI_STATUS_OK);
    assert(value.integer_value == 5);
    umi_ui_view_model_destroy(view);
    return 0;
}
