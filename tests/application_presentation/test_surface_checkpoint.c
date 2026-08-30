/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_surface_checkpoint.c
 *
 * PURPOSE:
 *   Verify portable panel visibility and focus can be restored into a fresh
 *   session for the same workspace recipe.
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

#include "umicom/application/presentation/presentation.h"

int main(void)
{
    UmiApplicationPresentationSurfaceSession source;
    UmiApplicationPresentationSurfaceSession restored;
    UmiApplicationPresentationSurfaceCheckpoint checkpoint;
    assert(umi_application_presentation_surface_session_init(
               "org.umicom.workspace.trader.standard", &source) == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_session_focus(
               &source, "umicom.trading.risk") == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_checkpoint_capture(
               &source, &checkpoint) == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_session_init(
               "org.umicom.workspace.trader.standard", &restored) == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_checkpoint_restore(
               &restored, &checkpoint) == UMI_STATUS_OK);
    assert(restored.focused_index < restored.item_count);
    assert(strcmp(restored.items[restored.focused_index].placement->panel->component_id,
                  "umicom.trading.risk") == 0);
    return 0;
}
