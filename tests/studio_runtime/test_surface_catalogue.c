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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t i,j;
    assert(umi_studio_surface_catalogue_count() == 26U);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_EDITOR) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_LEARNING) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_DOCUMENTATION) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_VISUAL_DESIGNER) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_COMPONENT_PALETTE) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_COMPONENT_STRUCTURE) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_OBJECT_INSPECTOR) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_DESIGN_CODE) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_DESIGN_MIXED) != NULL);
    assert(umi_studio_surface_catalogue_for_kind(
        UMI_STUDIO_SURFACE_DESIGN_PREVIEW) != NULL);
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<umi_studio_surface_catalogue_count();++i) {
        const UmiStudioRuntimeSurfaceBinding *a =
            umi_studio_surface_catalogue_at(i);
        assert(a != NULL);
        /* Visit each bounded item once so every record receives the same rule. */
        for(j=i+1U;j<umi_studio_surface_catalogue_count();++j)
            assert(strcmp(a->ide_surface_id,
                umi_studio_surface_catalogue_at(j)->ide_surface_id)!=0);
    }
    return 0;
}
