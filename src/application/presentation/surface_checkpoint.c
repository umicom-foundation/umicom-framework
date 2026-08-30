/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/surface_checkpoint.c
 *
 * PURPOSE:
 *   Capture and restore the portable parts of an application surface session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/surface_checkpoint.h"

#include <string.h>

UmiStatus umi_application_presentation_surface_checkpoint_capture(
    const UmiApplicationPresentationSurfaceSession *session,
    UmiApplicationPresentationSurfaceCheckpoint *out_checkpoint)
{
    size_t index;
    if (session == NULL || out_checkpoint == NULL || session->plan.recipe == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_checkpoint, 0, sizeof(*out_checkpoint));
    out_checkpoint->recipe_id = session->plan.recipe->recipe_id;
    out_checkpoint->item_count = session->item_count;
    out_checkpoint->source_revision = session->revision;
    for (index = 0U; index < session->item_count; ++index) {
        out_checkpoint->items[index] =
            (UmiApplicationPresentationSurfaceCheckpointItem){
                session->items[index].placement->panel->component_id,
                session->items[index].visible,
                session->items[index].focused,
                session->items[index].dirty};
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_presentation_surface_checkpoint_restore(
    UmiApplicationPresentationSurfaceSession *session,
    const UmiApplicationPresentationSurfaceCheckpoint *checkpoint)
{
    size_t index;
    const char *focus_id = NULL;
    if (session == NULL || checkpoint == NULL || session->plan.recipe == NULL ||
        checkpoint->recipe_id == NULL ||
        strcmp(session->plan.recipe->recipe_id, checkpoint->recipe_id) != 0 ||
        checkpoint->item_count > UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < checkpoint->item_count; ++index) {
        const UmiApplicationPresentationSurfaceCheckpointItem *saved =
            &checkpoint->items[index];
        UmiApplicationPresentationSurfaceItem *item =
            umi_application_presentation_surface_session_find(
                session, saved->component_id);
        UmiStatus status;
        if (item == NULL) continue;
        status = umi_application_presentation_surface_session_set_visible(
            session, saved->component_id, saved->visible);
        if (status != UMI_STATUS_OK) return status;
        item->dirty = saved->dirty != 0;
        if (saved->focused && saved->visible) focus_id = saved->component_id;
    }
    if (focus_id != NULL) {
        return umi_application_presentation_surface_session_focus(
            session, focus_id);
    }
    return UMI_STATUS_OK;
}
