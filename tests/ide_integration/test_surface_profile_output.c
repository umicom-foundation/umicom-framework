/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_surface_profile_output.c
 *
 * PURPOSE:
 *   Verify the reusable Output IDE surface profile.
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
#include "umicom/ide_integration/surface_profiles/output.h"

int main(void)
{
    const UmiIdeSurfaceDescriptor *surface =
        umi_ide_surface_profile_output();
    assert(surface != NULL);
    assert(strcmp(surface->surface_id, "umicom.ide.output") == 0);
    assert(surface->title[0] != '\0');
    assert(surface->default_region >= UMI_IDE_SURFACE_REGION_PRIMARY_SIDEBAR);
    assert(surface->default_region <= UMI_IDE_SURFACE_REGION_OVERLAY);
    return 0;
}
