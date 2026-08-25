/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_window_placement.c
 * PURPOSE: Validate the Framework-owned window placement contract.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/window_placement.h"

int main(void)
{
    UmiDesktopWindowPlacementSnapshot value;
    UmiDesktopWindowPlacementSnapshot copy;
    umi_desktop_window_placement_init(&value, "window_placement.primary");
    assert(umi_desktop_window_placement_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_window_placement_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_window_placement_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_window_placement_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_window_placement_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_window_placement_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    UmiDesktopControlRect bounds = {0,0,100,100}; UmiDesktopControlRect geometry = {-10,90,50,50};
    assert(umi_desktop_window_placement_clamp(&geometry, &bounds) == UMI_STATUS_OK); assert(geometry.x == 0 && geometry.y == 50);
    return 0;
}
