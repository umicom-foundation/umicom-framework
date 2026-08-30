/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/surface_runtime.c
 *
 * PURPOSE:
 *   Coordinate portable application sessions, product controllers and
 *   frontend hosts while keeping product code independent of UI toolkits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/surface_runtime.h"

#include <stdio.h>
#include <string.h>

static UmiStatus dispatch_update(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationPresentationSurfaceItem *item,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    int notify_host)
{
    UmiApplicationPresentationSurfaceUpdate update;
    UmiStatus status;
    status = umi_application_presentation_surface_controller_dispatch(
        &runtime->controllers, item->placement, event, payload, &update);
    if (status == UMI_STATUS_OK) {
        status = umi_application_presentation_surface_session_apply(
            &runtime->session, item->placement->panel->component_id, &update);
    }
    if (status != UMI_STATUS_OK) {
        (void)memset(&update, 0, sizeof(update));
        update.state = UMI_APPLICATION_PRESENTATION_STATE_ERROR;
        (void)snprintf(update.message, sizeof(update.message),
                       "%s failed with status %d",
                       umi_application_presentation_surface_event_text(event),
                       (int)status);
        (void)umi_application_presentation_surface_session_apply(
            &runtime->session, item->placement->panel->component_id, &update);
    }
    if (notify_host && runtime->host_bound && runtime->started) {
        UmiStatus host_status = runtime->host.operations->update(
            runtime->host.context, item);
        if (status == UMI_STATUS_OK) status = host_status;
    }
    umi_application_presentation_surface_journal_record(
        &runtime->journal, event, item->placement->panel->component_id, status);
    return status;
}

UmiStatus umi_application_presentation_surface_runtime_init(
    const char *recipe_id,
    UmiApplicationPresentationSurfaceRuntime *out_runtime)
{
    UmiStatus status;
    if (recipe_id == NULL || out_runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_runtime, 0, sizeof(*out_runtime));
    status = umi_application_presentation_surface_session_init(
        recipe_id, &out_runtime->session);
    if (status != UMI_STATUS_OK) return status;
    umi_application_presentation_surface_controller_registry_init(
        &out_runtime->controllers);
    umi_application_presentation_surface_journal_init(&out_runtime->journal);
    return UMI_STATUS_OK;
}

UmiStatus umi_application_presentation_surface_runtime_bind_host(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const UmiApplicationPresentationSurfaceHost *host)
{
    UmiStatus status;
    if (runtime == NULL || host == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (runtime->started) return UMI_STATUS_INVALID_STATE;
    status = umi_application_presentation_surface_host_validate(host);
    if (status != UMI_STATUS_OK) return status;
    runtime->host = *host;
    runtime->host_bound = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_presentation_surface_runtime_register_controller(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    UmiApplicationPresentationSurfaceController controller,
    void *context)
{
    if (runtime == NULL || component_id == NULL || controller == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (runtime->started) return UMI_STATUS_INVALID_STATE;
    if (umi_application_presentation_surface_session_find(
            &runtime->session, component_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_application_presentation_surface_controller_register(
        &runtime->controllers, component_id, controller, context);
}

UmiStatus umi_application_presentation_surface_runtime_start(
    UmiApplicationPresentationSurfaceRuntime *runtime)
{
    size_t index;
    UmiStatus status;
    if (runtime == NULL || !runtime->host_bound || runtime->started) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = runtime->host.operations->begin(
        runtime->host.context, runtime->session.plan.window);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < runtime->session.item_count; ++index) {
        UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[index];
        if (!item->visible) continue;
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_MOUNT,
                                 NULL, 0);
        if (status == UMI_STATUS_OK) {
            status = runtime->host.operations->mount(
                runtime->host.context, item);
        }
        if (status != UMI_STATUS_OK) break;
    }
    if (status != UMI_STATUS_OK) {
        while (index > 0U) {
            UmiApplicationPresentationSurfaceItem *mounted =
                &runtime->session.items[--index];
            if (mounted->visible) {
                (void)runtime->host.operations->unmount(
                    runtime->host.context, mounted);
            }
        }
        runtime->host.operations->end(runtime->host.context);
        return status;
    }
    runtime->started = 1;
    if (runtime->session.focused_index < runtime->session.item_count) {
        UmiApplicationPresentationSurfaceItem *focused =
            &runtime->session.items[runtime->session.focused_index];
        status = runtime->host.operations->focus(runtime->host.context, focused);
    }
    return status;
}

UmiStatus umi_application_presentation_surface_runtime_stop(
    UmiApplicationPresentationSurfaceRuntime *runtime)
{
    size_t index;
    UmiStatus first_error = UMI_STATUS_OK;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!runtime->started) return UMI_STATUS_OK;
    index = runtime->session.item_count;
    while (index > 0U) {
        UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[--index];
        UmiStatus status;
        if (!item->visible) continue;
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT,
                                 NULL, 0);
        if (status == UMI_STATUS_OK) {
            status = runtime->host.operations->unmount(
                runtime->host.context, item);
        }
        if (first_error == UMI_STATUS_OK && status != UMI_STATUS_OK) {
            first_error = status;
        }
    }
    runtime->host.operations->end(runtime->host.context);
    runtime->started = 0;
    return first_error;
}

UmiStatus umi_application_presentation_surface_runtime_activate(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    UmiStatus status;
    if (runtime == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    if (item->visible) return UMI_STATUS_OK;
    status = umi_application_presentation_surface_session_set_visible(
        &runtime->session, component_id, 1);
    if (status == UMI_STATUS_OK) {
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_ACTIVATE,
                                 NULL, 0);
    }
    if (status == UMI_STATUS_OK && runtime->started) {
        status = runtime->host.operations->mount(runtime->host.context, item);
        if (status == UMI_STATUS_OK && item->focused) {
            status = runtime->host.operations->focus(
                runtime->host.context, item);
        }
    }
    return status;
}

UmiStatus umi_application_presentation_surface_runtime_deactivate(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    UmiStatus status;
    if (runtime == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    if (!item->visible) return UMI_STATUS_OK;
    status = umi_application_presentation_surface_session_set_visible(
        &runtime->session, component_id, 0);
    if (status != UMI_STATUS_OK) return status;
    status = dispatch_update(runtime, item,
                             UMI_APPLICATION_PRESENTATION_EVENT_DEACTIVATE,
                             NULL, 0);
    if (status == UMI_STATUS_OK && runtime->started) {
        status = runtime->host.operations->unmount(runtime->host.context, item);
        if (status == UMI_STATUS_OK &&
            runtime->session.focused_index < runtime->session.item_count) {
            status = runtime->host.operations->focus(
                runtime->host.context,
                &runtime->session.items[runtime->session.focused_index]);
        }
    }
    return status;
}

UmiStatus umi_application_presentation_surface_runtime_focus(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    UmiStatus status;
    if (runtime == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_presentation_surface_session_focus(
        &runtime->session, component_id);
    if (status != UMI_STATUS_OK) return status;
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    status = dispatch_update(runtime, item,
                             UMI_APPLICATION_PRESENTATION_EVENT_FOCUS,
                             NULL, 0);
    if (status == UMI_STATUS_OK && runtime->started) {
        status = runtime->host.operations->focus(runtime->host.context, item);
    }
    return status;
}

UmiStatus umi_application_presentation_surface_runtime_refresh(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    if (runtime == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    return dispatch_update(runtime, item,
                           UMI_APPLICATION_PRESENTATION_EVENT_REFRESH,
                           NULL, 1);
}

UmiStatus umi_application_presentation_surface_runtime_refresh_all(
    UmiApplicationPresentationSurfaceRuntime *runtime)
{
    size_t index;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < runtime->session.item_count; ++index) {
        UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[index];
        UmiStatus status;
        if (!item->visible) continue;
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_REFRESH,
                                 NULL, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_presentation_surface_runtime_command(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    const char *command_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    if (runtime == NULL || component_id == NULL || command_id == NULL ||
        command_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    return dispatch_update(runtime, item,
                           UMI_APPLICATION_PRESENTATION_EVENT_COMMAND,
                           command_id, 1);
}

UmiStatus umi_application_presentation_surface_runtime_restore(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const UmiApplicationPresentationSurfaceCheckpoint *checkpoint)
{
    if (runtime == NULL || checkpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (runtime->started) return UMI_STATUS_INVALID_STATE;
    return umi_application_presentation_surface_checkpoint_restore(
        &runtime->session, checkpoint);
}

UmiStatus umi_application_presentation_surface_runtime_snapshot(
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot)
{
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_presentation_surface_session_snapshot(
        &runtime->session, out_snapshot);
}
