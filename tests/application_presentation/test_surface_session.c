/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_surface_session.c
 *
 * PURPOSE:
 *   Verify a workspace recipe becomes a mutable, focusable panel session while
 *   locked Framework panels remain protected from accidental closure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/presentation/presentation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationPresentationSurfaceSession session;
    UmiApplicationPresentationSurfaceSnapshot snapshot;
    UmiApplicationPresentationSurfaceItem *editor;
    const UmiApplicationComponentRecipe *recipe =
        umi_application_component_recipe_catalogue_find(
            "org.umicom.workspace.studio.standard");
    assert(recipe != NULL);
    assert(umi_application_presentation_surface_session_init(
               "org.umicom.workspace.studio.standard", &session) == UMI_STATUS_OK);
    assert(session.item_count == recipe->slot_count);
    editor = umi_application_presentation_surface_session_find(
        &session, "umicom.development.editor");
    assert(editor != NULL && editor->visible && editor->focused);
    assert(umi_application_presentation_surface_session_set_visible(
               &session, "umicom.development.editor", 0) ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_application_presentation_surface_session_focus(
               &session, "umicom.ai.chat") == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_session_snapshot(
               &session, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.panel_count == recipe->slot_count);
    assert(snapshot.focused_component_id != NULL);
    return 0;
}
