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

#include "umicom/application/presentation/surface_behavior_catalogue.h"
#include "umicom/application/presentation/workspace_runtime_policy_catalogue.h"

/* Provide the dispatch update operation used by this module and its client applications. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_application_presentation_surface_session_apply(
            &runtime->session, item->placement->panel->component_id, &update);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (notify_host && runtime->host_bound && runtime->started) {
        UmiStatus host_status = runtime->host.operations->update(
            runtime->host.context, item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = host_status;
    }
    umi_application_presentation_surface_journal_record(
        &runtime->journal, event, item->placement->panel->component_id, status);
    return status;
}

/*
 * Provide the add seconds safely operation used by this module and its client
 * applications.
 */
static uint32_t add_seconds_safely(uint32_t current, uint32_t elapsed)
{
    return UINT32_MAX - current < elapsed ? UINT32_MAX : current + elapsed;
}

/*
 * Provide the multiply seconds safely operation used by this module and its client
 * applications.
 */
static uint32_t multiply_seconds_safely(uint32_t seconds, uint32_t multiplier)
{
    return multiplier != 0U && seconds > UINT32_MAX / multiplier
        ? UINT32_MAX
        : seconds * multiplier;
}

/*
 * Initialise application presentation surface runtime from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_presentation_surface_runtime_init(
    const char *recipe_id,
    UmiApplicationPresentationSurfaceRuntime *out_runtime)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (recipe_id == NULL || out_runtime == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_runtime, 0, sizeof(*out_runtime));
    status = umi_application_presentation_surface_session_init(
        recipe_id, &out_runtime->session);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_application_presentation_surface_controller_registry_init(
        &out_runtime->controllers);
    umi_application_presentation_surface_journal_init(&out_runtime->journal);
    return UMI_STATUS_OK;
}

/*
 * Provide the application presentation surface runtime bind host operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_bind_host(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const UmiApplicationPresentationSurfaceHost *host)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || host == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (runtime->started) return UMI_STATUS_INVALID_STATE;
    status = umi_application_presentation_surface_host_validate(host);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    runtime->host = *host;
    runtime->host_bound = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the application presentation surface runtime register controller operation used
 * by this module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_register_controller(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    UmiApplicationPresentationSurfaceController controller,
    void *context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || component_id == NULL || controller == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (runtime->started) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_application_presentation_surface_session_find(
            &runtime->session, component_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_application_presentation_surface_controller_register(
        &runtime->controllers, component_id, controller, context);
}

/*
 * Provide the application presentation surface runtime register controller for all
 * operation used by this module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_register_controller_for_all(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationPresentationSurfaceController controller,
    void *context)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (runtime->started) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (runtime->session.item_count >
        UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY -
            runtime->controllers.count) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Check the complete operation before mutating the registry. This keeps a
       failed bulk registration from leaving only some panels registered. */
    for (index = 0U; index < runtime->session.item_count; ++index) {
        const UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_application_presentation_surface_controller_find(
                &runtime->controllers,
                item->placement->panel->component_id) != NULL) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->session.item_count; ++index) {
        const UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[index];
        UmiStatus status =
            umi_application_presentation_surface_controller_register(
                &runtime->controllers,
                item->placement->panel->component_id,
                controller,
                context);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application presentation surface runtime start operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_start(
    UmiApplicationPresentationSurfaceRuntime *runtime)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || !runtime->host_bound || runtime->started) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = runtime->host.operations->begin(
        runtime->host.context, runtime->session.plan.window);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->session.item_count; ++index) {
        UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[index];
        /* Apply this operation only while the related capability or state is available. */
        if (!item->visible) continue;
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_MOUNT,
                                 NULL, 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = runtime->host.operations->mount(
                runtime->host.context, item);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (index > 0U) {
            UmiApplicationPresentationSurfaceItem *mounted =
                &runtime->session.items[--index];
            /* Apply this operation only while the related capability or state is available. */
            if (mounted->visible) {
                (void)runtime->host.operations->unmount(
                    runtime->host.context, mounted);
            }
        }
        runtime->host.operations->end(runtime->host.context);
        return status;
    }
    runtime->started = 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (runtime->session.focused_index < runtime->session.item_count) {
        UmiApplicationPresentationSurfaceItem *focused =
            &runtime->session.items[runtime->session.focused_index];
        status = runtime->host.operations->focus(runtime->host.context, focused);
    }
    return status;
}

/*
 * Provide the application presentation surface runtime stop operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_stop(
    UmiApplicationPresentationSurfaceRuntime *runtime)
{
    size_t index;
    UmiStatus first_error = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!runtime->started) return UMI_STATUS_OK;
    index = runtime->session.item_count;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[--index];
        UmiStatus status;
        /* Apply this operation only while the related capability or state is available. */
        if (!item->visible) continue;
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT,
                                 NULL, 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            status = runtime->host.operations->unmount(
                runtime->host.context, item);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (first_error == UMI_STATUS_OK && status != UMI_STATUS_OK) {
            first_error = status;
        }
    }
    runtime->host.operations->end(runtime->host.context);
    runtime->started = 0;
    return first_error;
}

/*
 * Provide the application presentation surface runtime activate operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_activate(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (item->visible) return UMI_STATUS_OK;
    status = umi_application_presentation_surface_session_set_visible(
        &runtime->session, component_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_ACTIVATE,
                                 NULL, 0);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && runtime->started) {
        status = runtime->host.operations->mount(runtime->host.context, item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK && item->focused) {
            status = runtime->host.operations->focus(
                runtime->host.context, item);
        }
    }
    return status;
}

/*
 * Provide the application presentation surface runtime deactivate operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_deactivate(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!item->visible) return UMI_STATUS_OK;
    status = umi_application_presentation_surface_session_set_visible(
        &runtime->session, component_id, 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = dispatch_update(runtime, item,
                             UMI_APPLICATION_PRESENTATION_EVENT_DEACTIVATE,
                             NULL, 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && runtime->started) {
        status = runtime->host.operations->unmount(runtime->host.context, item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK &&
            runtime->session.focused_index < runtime->session.item_count) {
            status = runtime->host.operations->focus(
                runtime->host.context,
                &runtime->session.items[runtime->session.focused_index]);
        }
    }
    return status;
}

/*
 * Provide the application presentation surface runtime focus operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_focus(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_presentation_surface_session_focus(
        &runtime->session, component_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    status = dispatch_update(runtime, item,
                             UMI_APPLICATION_PRESENTATION_EVENT_FOCUS,
                             NULL, 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        item->behavior->refresh_policy ==
            UMI_APPLICATION_PRESENTATION_REFRESH_ON_FOCUS) {
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_REFRESH,
                                 NULL, 0);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && runtime->started) {
        status = runtime->host.operations->focus(runtime->host.context, item);
    }
    return status;
}

/*
 * Provide the application presentation surface runtime refresh operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_refresh(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || component_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    return dispatch_update(runtime, item,
                           UMI_APPLICATION_PRESENTATION_EVENT_REFRESH,
                           NULL, 1);
}

/*
 * Provide the application presentation surface runtime refresh all operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_refresh_all(
    UmiApplicationPresentationSurfaceRuntime *runtime)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->session.item_count; ++index) {
        UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[index];
        UmiStatus status;
        /* Apply this operation only while the related capability or state is available. */
        if (!item->visible) continue;
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_REFRESH,
                                 NULL, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application presentation surface runtime command operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_command(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    const char *command_id)
{
    UmiApplicationPresentationSurfaceItem *item;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || component_id == NULL || command_id == NULL ||
        command_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (runtime->background &&
        !runtime->session.workspace_policy->allow_background_commands) {
        status = UMI_STATUS_INVALID_STATE;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (item->behavior->command_mode ==
               UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY) {
        status = UMI_STATUS_PERMISSION_DENIED;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (item->behavior->command_mode ==
                   UMI_APPLICATION_PRESENTATION_COMMAND_GUARDED &&
               umi_application_presentation_surface_controller_find(
                   &runtime->controllers, component_id) == NULL) {
        status = UMI_STATUS_PERMISSION_DENIED;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return dispatch_update(runtime, item,
                               UMI_APPLICATION_PRESENTATION_EVENT_COMMAND,
                               command_id, 1);
    }
    umi_application_presentation_surface_journal_record(
        &runtime->journal, UMI_APPLICATION_PRESENTATION_EVENT_COMMAND,
        component_id, status);
    return status;
}

/*
 * Provide the application presentation surface runtime context changed operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_context_changed(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const char *component_id,
    const char *context_value)
{
    UmiApplicationPresentationSurfaceItem *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || component_id == NULL || context_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_application_presentation_surface_session_find(
        &runtime->session, component_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!runtime->session.workspace_policy->share_context ||
        !item->behavior->accept_context) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return dispatch_update(runtime, item,
                           UMI_APPLICATION_PRESENTATION_EVENT_CONTEXT_CHANGED,
                           context_value, 1);
}

/*
 * Provide the application presentation surface runtime advance operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_advance(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    uint32_t elapsed_seconds)
{
    size_t index;
    uint32_t multiplier;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || elapsed_seconds == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!runtime->started) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (runtime->background &&
        runtime->session.workspace_policy->background_policy ==
            UMI_APPLICATION_PRESENTATION_BACKGROUND_PAUSED) {
        return UMI_STATUS_OK;
    }
    multiplier = runtime->background &&
                         runtime->session.workspace_policy->background_policy ==
                             UMI_APPLICATION_PRESENTATION_BACKGROUND_REDUCED
        ? 4U
        : 1U;
    runtime->background_elapsed_seconds = add_seconds_safely(
        runtime->background_elapsed_seconds, elapsed_seconds);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->session.item_count; ++index) {
        UmiApplicationPresentationSurfaceItem *item =
            &runtime->session.items[index];
        uint32_t due;
        UmiStatus status;
        /* Apply this operation only while the related capability or state is available. */
        if (!item->visible ||
            (item->behavior->refresh_policy !=
                 UMI_APPLICATION_PRESENTATION_REFRESH_INTERVAL &&
             item->behavior->refresh_policy !=
                 UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING)) {
            continue;
        }
        due = multiply_seconds_safely(
            item->behavior->refresh_interval_seconds, multiplier);
        item->elapsed_refresh_seconds = add_seconds_safely(
            item->elapsed_refresh_seconds, elapsed_seconds);
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->elapsed_refresh_seconds < due) continue;
        /* Keep the remainder so uneven timer ticks do not slowly move a
         * panel away from its declared refresh rhythm. One refresh per call
         * deliberately prevents a long-suspended application causing a
         * burst of stale refresh work when it becomes active again. */
        item->elapsed_refresh_seconds %= due;
        status = dispatch_update(runtime, item,
                                 UMI_APPLICATION_PRESENTATION_EVENT_REFRESH,
                                 NULL, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application presentation surface runtime set background operation used by
 * this module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_set_background(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    int background)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || (background != 0 && background != 1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (runtime->background == background) return UMI_STATUS_OK;
    runtime->background = background;
    runtime->background_elapsed_seconds = 0U;
    return UMI_STATUS_OK;
}

/*
 * Provide the application presentation surface runtime checkpoint due operation used by
 * this module and its client applications.
 */
int umi_application_presentation_surface_runtime_checkpoint_due(
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    uint32_t elapsed_since_checkpoint_seconds,
    int changed)
{
    const UmiApplicationPresentationWorkspaceRuntimePolicy *policy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || (changed != 0 && changed != 1)) return 0;
    policy = runtime->session.workspace_policy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->checkpoint_policy ==
        UMI_APPLICATION_PRESENTATION_CHECKPOINT_ON_CHANGE) {
        return changed;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->checkpoint_policy ==
        UMI_APPLICATION_PRESENTATION_CHECKPOINT_PERIODIC) {
        return elapsed_since_checkpoint_seconds >=
               policy->checkpoint_interval_seconds;
    }
    return 0;
}

/*
 * Provide the application presentation surface runtime restore operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_restore(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    const UmiApplicationPresentationSurfaceCheckpoint *checkpoint)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || checkpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (runtime->started) return UMI_STATUS_INVALID_STATE;
    return umi_application_presentation_surface_checkpoint_restore(
        &runtime->session, checkpoint);
}

/*
 * Provide the application presentation surface runtime snapshot operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_runtime_snapshot(
    const UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_presentation_surface_session_snapshot(
        &runtime->session, out_snapshot);
}
