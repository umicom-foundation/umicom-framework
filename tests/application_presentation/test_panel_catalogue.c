/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_panel_catalogue.c
 *
 * PURPOSE:
 *   Verify every canonical component has one reusable panel presentation
 *   contract and that important Studio metadata remains discoverable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/presentation.h"

#include <assert.h>
#include <string.h>

#include "umicom/application/component_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationPresentationPanelSpec *editor;
    const UmiApplicationPresentationPanelSpec *live_preview;

    assert(umi_application_presentation_panel_catalogue_count() ==
           umi_application_component_catalogue_count());
    editor = umi_application_presentation_panel_catalogue_find(
        "umicom.development.editor");
    assert(editor != NULL);
    assert(editor->surface_kind == UMI_APPLICATION_PRESENTATION_SURFACE_EDITOR);
    assert(editor->chrome == UMI_APPLICATION_PRESENTATION_CHROME_DOCUMENT);
    assert(editor->preferred_region == UMI_APPLICATION_COMPONENT_REGION_PRIMARY);
    assert(editor->minimum_width >= 640U);
    assert(editor->compact_policy == UMI_APPLICATION_PRESENTATION_COMPACT_KEEP);
    assert(strcmp(umi_application_presentation_compact_policy_text(
                      editor->compact_policy),
                  "keep") == 0);
    assert(strcmp(editor->icon_id, "umicom.icon.development.editor") == 0);
    assert(umi_application_presentation_panel_spec_validate(editor) == UMI_STATUS_OK);
    /* The active-document preview must remain a complete Framework component,
     * including the panel record used by Studio's New Window catalogue. */
    live_preview = umi_application_presentation_panel_catalogue_find(
        "umicom.development.live-preview");
    assert(live_preview != NULL);
    assert(live_preview->preferred_region == UMI_APPLICATION_COMPONENT_REGION_RIGHT);
    assert(live_preview->compact_policy == UMI_APPLICATION_PRESENTATION_COMPACT_AUTO_HIDE);
    return 0;
}
