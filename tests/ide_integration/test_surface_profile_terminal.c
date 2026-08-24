/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_surface_profile_terminal.c
 *
 * PURPOSE:
 *   Verify the reusable Terminal IDE surface profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/surface_profiles/terminal.h"

int main(void)
{
    const UmiIdeSurfaceDescriptor *surface =
        umi_ide_surface_profile_terminal();
    assert(surface != NULL);
    assert(strcmp(surface->surface_id, "umicom.ide.terminal") == 0);
    assert(surface->title[0] != '\0');
    assert(surface->default_region >= UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR);
    assert(surface->default_region <= UMI_IDE_SURFACE_REGION_OVERLAY);
    return 0;
}
