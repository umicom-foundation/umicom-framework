/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/surface_session.c
 *
 * PURPOSE:
 *   Manage live state for every panel in one projected application recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/surface_session.h"

#include <stdio.h>
#include <string.h>

#include "umicom/application/presentation/surface_behavior_catalogue.h"
#include "umicom/application/presentation/workspace_runtime_policy_catalogue.h"

static int state_valid(UmiApplicationPresentationSurfaceState state)
{
    return state >= UMI_APPLICATION_PRESENTATION_STATE_DORMANT &&
           state <= UMI_APPLICATION_PRESENTATION_STATE_PERMISSION_REQUIRED;
}

static void copy_text(char *destination, size_t capacity, const char *source)
{
    (void)snprintf(destination, capacity, "%s", source != NULL ? source : "");
}

UmiStatus umi_application_presentation_surface_session_init(
    const char *recipe_id,
    UmiApplicationPresentationSurfaceSession *out_session)
{
    size_t index;
    UmiStatus status;
    if (recipe_id == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_session, 0, sizeof(*out_session));
    out_session->focused_index = SIZE_MAX;
    status = umi_application_presentation_project(recipe_id, &out_session->plan);
    if (status != UMI_STATUS_OK) return status;
    out_session->workspace_policy =
        umi_application_presentation_workspace_runtime_policy_catalogue_find(
            recipe_id);
    if (out_session->workspace_policy == NULL) return UMI_STATUS_NOT_FOUND;
    out_session->item_count = out_session->plan.placement_count;
    for (index = 0U; index < out_session->item_count; ++index) {
        UmiApplicationPresentationSurfaceItem *item = &out_session->items[index];
        item->placement = &out_session->plan.placements[index];
        item->behavior =
            umi_application_presentation_surface_behavior_catalogue_find(
                item->placement->panel->component_id);
        if (item->behavior == NULL) {
            (void)memset(out_session, 0, sizeof(*out_session));
            return UMI_STATUS_NOT_FOUND;
        }
        item->state = UMI_APPLICATION_PRESENTATION_STATE_DORMANT;
        item->visible =
            out_session->workspace_policy->startup_policy ==
                    UMI_APPLICATION_PRESENTATION_STARTUP_ALL_VISIBLE
                ? 1
                : item->placement->slot->visible;
        item->revision = 1U;
        if (out_session->focused_index == SIZE_MAX && item->visible) {
            out_session->focused_index = index;
            item->focused = 1;
        }
    }
    if (out_session->workspace_policy->focus_policy ==
        UMI_APPLICATION_PRESENTATION_FOCUS_PRIMARY) {
        for (index = 0U; index < out_session->item_count; ++index) {
            if (out_session->items[index].visible &&
                out_session->items[index].placement->slot->region ==
                    UMI_APPLICATION_COMPONENT_REGION_PRIMARY) {
                size_t other;
                for (other = 0U; other < out_session->item_count; ++other)
                    out_session->items[other].focused = 0;
                out_session->items[index].focused = 1;
                out_session->focused_index = index;
                break;
            }
        }
    }
    out_session->revision = 1U;
    return UMI_STATUS_OK;
}

UmiApplicationPresentationSurfaceItem *
umi_application_presentation_surface_session_find(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id)
{
    size_t index;
    if (session == NULL || component_id == NULL) return NULL;
    for (index = 0U; index < session->item_count; ++index) {
        if (strcmp(session->items[index].placement->panel->component_id,
                   component_id) == 0) {
            return &session->items[index];
        }
    }
    return NULL;
}

const UmiApplicationPresentationSurfaceItem *
umi_application_presentation_surface_session_find_const(
    const UmiApplicationPresentationSurfaceSession *session,
    const char *component_id)
{
    size_t index;
    if (session == NULL || component_id == NULL) return NULL;
    for (index = 0U; index < session->item_count; ++index) {
        if (strcmp(session->items[index].placement->panel->component_id,
                   component_id) == 0) {
            return &session->items[index];
        }
    }
    return NULL;
}

UmiApplicationPresentationSurfaceItem *
umi_application_presentation_surface_session_at(
    UmiApplicationPresentationSurfaceSession *session,
    size_t index)
{
    return session != NULL && index < session->item_count
        ? &session->items[index]
        : NULL;
}

UmiStatus umi_application_presentation_surface_session_apply(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id,
    const UmiApplicationPresentationSurfaceUpdate *update)
{
    UmiApplicationPresentationSurfaceItem *item;
    if (session == NULL || component_id == NULL || update == NULL ||
        !state_valid(update->state) || update->progress_percent > 100U ||
        (update->has_progress != 0 && update->has_progress != 1) ||
        (update->dirty != 0 && update->dirty != 1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        session, component_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    item->state = update->state;
    copy_text(item->message, sizeof(item->message), update->message);
    copy_text(item->badge, sizeof(item->badge), update->badge);
    item->progress_percent = update->progress_percent;
    item->has_progress = update->has_progress;
    item->dirty = update->dirty;
    item->revision += 1U;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_presentation_surface_session_set_visible(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id,
    int visible)
{
    UmiApplicationPresentationSurfaceItem *item;
    int was_focused;
    size_t index;
    if (session == NULL || component_id == NULL ||
        (visible != 0 && visible != 1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        session, component_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    if (!visible && (item->placement->slot->locked ||
                     !item->placement->panel->allow_closing)) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (item->visible == visible) return UMI_STATUS_OK;
    was_focused = item->focused;
    item->visible = visible;
    if (!visible) {
        item->focused = 0;
        item->state = UMI_APPLICATION_PRESENTATION_STATE_DORMANT;
        if (was_focused) {
            session->focused_index = SIZE_MAX;
            for (index = 0U; index < session->item_count; ++index) {
                if (session->items[index].visible) {
                    session->items[index].focused = 1;
                    session->items[index].revision += 1U;
                    session->focused_index = index;
                    break;
                }
            }
        }
    } else if (session->focused_index >= session->item_count) {
        item->focused = 1;
        session->focused_index = (size_t)(item - session->items);
    }
    item->revision += 1U;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_presentation_surface_session_focus(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    size_t index;
    if (session == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        session, component_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    if (!item->visible) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < session->item_count; ++index) {
        session->items[index].focused = 0;
        if (&session->items[index] == item) session->focused_index = index;
    }
    item->focused = 1;
    item->revision += 1U;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_presentation_surface_session_snapshot(
    const UmiApplicationPresentationSurfaceSession *session,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot)
{
    size_t index;
    if (session == NULL || out_snapshot == NULL || session->plan.recipe == NULL ||
        session->plan.window == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->recipe_id = session->plan.recipe->recipe_id;
    out_snapshot->window_id = session->plan.window->window_id;
    out_snapshot->panel_count = session->item_count;
    out_snapshot->workspace_policy = session->workspace_policy;
    out_snapshot->revision = session->revision;
    for (index = 0U; index < session->item_count; ++index) {
        const UmiApplicationPresentationSurfaceItem *item = &session->items[index];
        if (item->visible) out_snapshot->visible_count += 1U;
        if (item->state == UMI_APPLICATION_PRESENTATION_STATE_READY) {
            out_snapshot->ready_count += 1U;
        }
        if (item->state == UMI_APPLICATION_PRESENTATION_STATE_WARNING ||
            item->state == UMI_APPLICATION_PRESENTATION_STATE_ERROR ||
            item->state == UMI_APPLICATION_PRESENTATION_STATE_OFFLINE ||
            item->state == UMI_APPLICATION_PRESENTATION_STATE_PERMISSION_REQUIRED) {
            out_snapshot->attention_count += 1U;
        }
        if (item->dirty) out_snapshot->dirty_count += 1U;
        if (item->behavior->refresh_policy ==
                UMI_APPLICATION_PRESENTATION_REFRESH_INTERVAL ||
            item->behavior->refresh_policy ==
                UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING) {
            out_snapshot->scheduled_refresh_count += 1U;
        }
        if (item->behavior->refresh_policy ==
            UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING) {
            out_snapshot->streaming_count += 1U;
        }
        if (item->behavior->command_mode ==
            UMI_APPLICATION_PRESENTATION_COMMAND_GUARDED) {
            out_snapshot->guarded_command_count += 1U;
        }
        if (item->behavior->publish_context || item->behavior->accept_context) {
            out_snapshot->context_enabled_count += 1U;
        }
        if (item->focused) {
            out_snapshot->focused_component_id =
                item->placement->panel->component_id;
        }
    }
    return UMI_STATUS_OK;
}
