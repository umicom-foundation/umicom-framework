/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_surface_profile_explorer.c
 *
 * PURPOSE:
 *   Verify the reusable Explorer IDE surface profile.
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
#include "umicom/ide_integration/surface_profiles/explorer.h"

int main(void)
{
    const UmiIdeSurfaceDescriptor *surface =
        umi_ide_surface_profile_explorer();
    assert(surface != NULL);
    assert(strcmp(surface->surface_id, "umicom.ide.explorer") == 0);
    assert(surface->title[0] != '\0');
    assert(surface->default_region >= UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR);
    assert(surface->default_region <= UMI_IDE_SURFACE_REGION_OVERLAY);
    return 0;
}
