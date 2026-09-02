/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_layout_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for Studio runtime layout catalogue.
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
#include "umicom/studio_runtime/layout_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeLayoutPresetDefinition *design;
    const UmiStudioRuntimeLayoutPresetDefinition *code_preview;
    size_t index;
    int has_code = 0;
    int has_mixed = 0;
    int has_preview = 0;

    assert(umi_studio_layout_catalogue_count()==10U);
    assert(umi_studio_layout_catalogue_find(
        "umicom.studio.layout.default")!=NULL);
    assert(strcmp(umi_studio_layout_catalogue_at(0)->title,
                  "Default Development")==0);
    assert(umi_studio_layout_catalogue_find(
        "umicom.studio.layout.learning")!=NULL);
    assert(umi_studio_layout_catalogue_find(
        "umicom.studio.layout.visual-design")!=NULL);
    design = umi_studio_layout_catalogue_find(
        "umicom.studio.layout.visual-design");
    /* Confirm the saved layout opens every central authoring mode. */
    for (index = 0U; index < design->visible_surface_count; ++index) {
        if (design->visible_surfaces[index] == UMI_STUDIO_SURFACE_DESIGN_CODE) {
            has_code = 1;
        }
        if (design->visible_surfaces[index] == UMI_STUDIO_SURFACE_DESIGN_MIXED) {
            has_mixed = 1;
        }
        if (design->visible_surfaces[index] == UMI_STUDIO_SURFACE_DESIGN_PREVIEW) {
            has_preview = 1;
        }
    }
    assert(has_code && has_mixed && has_preview);
    code_preview = umi_studio_layout_catalogue_find(
        "umicom.studio.layout.code-preview");
    /* The named authoring layout must keep both sides of the workflow visible. */
    assert(code_preview != NULL);
    assert(umi_studio_layout_preset_contains_surface(
        code_preview, UMI_STUDIO_SURFACE_EDITOR));
    assert(umi_studio_layout_preset_contains_surface(
        code_preview, UMI_STUDIO_SURFACE_LIVE_PREVIEW));
    assert(umi_studio_layout_preset_validate(code_preview) == UMI_STATUS_OK);
    return 0;
}
