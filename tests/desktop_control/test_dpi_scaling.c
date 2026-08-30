/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_control/test_dpi_scaling.c
 * PURPOSE: Validate the Framework-owned dpi scaling contract.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/desktop/control/dpi_scaling.h"

int main(void)
{
    UmiDesktopDpiScalingSnapshot value;
    UmiDesktopDpiScalingSnapshot copy;
    umi_desktop_dpi_scaling_init(&value, "dpi_scaling.primary");
    assert(umi_desktop_dpi_scaling_validate(&value) == UMI_STATUS_OK);
    assert(umi_desktop_dpi_scaling_set_subject(&value, "desk.subject") == UMI_STATUS_OK);
    assert(umi_desktop_dpi_scaling_set_detail(&value, "Framework-owned control state") == UMI_STATUS_OK);
    assert(umi_desktop_dpi_scaling_set_state(&value, 2U) == UMI_STATUS_OK);
    assert(umi_desktop_dpi_scaling_set_priority(&value, 50U) == UMI_STATUS_OK);
    copy = value;
    assert(umi_desktop_dpi_scaling_same_identity(&value, &copy));
    assert(strcmp(value.subject_id, "desk.subject") == 0);
    UmiDesktopControlRect input = {10,20,100,50}; UmiDesktopControlRect output;
    assert(umi_desktop_dpi_scaling_scale_rect(&input, 100U, 200U, &output) == UMI_STATUS_OK); assert(output.width == 200);
    return 0;
}
