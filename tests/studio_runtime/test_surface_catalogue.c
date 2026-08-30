/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime surface catalogue.
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
#include "umicom/studio_runtime/surface_catalogue.h"
int main(void)
{
    size_t i,j;
    assert(umi_studio_surface_catalogue_count() == 16U);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_EDITOR) != NULL);
    for(i=0U;i<umi_studio_surface_catalogue_count();++i) {
        const UmiStudioRuntimeSurfaceBinding *a =
            umi_studio_surface_catalogue_at(i);
        assert(a != NULL);
        for(j=i+1U;j<umi_studio_surface_catalogue_count();++j)
            assert(strcmp(a->ide_surface_id,
                umi_studio_surface_catalogue_at(j)->ide_surface_id)!=0);
    }
    return 0;
}

