/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workbench.c
 *
 * PURPOSE:
 *   Implement the authoritative workbench aggregate and its command, navigation and session-state coordination.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The workbench owns reusable models but not product business data. Activating an activity selects its view container; commands still execute through the canonical Runtime command registry.
 */

#include "umicom/ui/workbench.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

struct UmiUiWorkbench {
    char id[UMI_UI_ID_CAPACITY];
    char active_perspective[UMI_UI_ID_CAPACITY];
    char active_document[UMI_UI_ID_CAPACITY];
    char active_activity[UMI_UI_ID_CAPACITY];
    char active_view_container[UMI_UI_ID_CAPACITY];
    uint64_t revision;
    UmiCommandRegistry *commands;

    UmiUiPaneModel *panes;
    UmiUiDocumentViewModel *documents;
    UmiUiEditorModel *editors;
    UmiUiPerspectiveModel *perspectives;
    UmiUiLayout *layout;
    UmiUiActionModel *actions;
    UmiUiMenuModel *menus;
    UmiUiToolbarModel *toolbars;
    UmiUiStatusModel *status;
    UmiUiSelectionModel *selection;
    UmiUiNotificationCentre *notifications;
    UmiUiDialogService *dialogs;
    UmiUiContributionModel *contributions;
    UmiUiBindingRegistry *bindings;
    UmiUiEventQueue *events;
    UmiUiViewFactoryRegistry *factories;

    UmiUiActivityModel *activities;
    UmiUiViewContainerModel *view_containers;
    UmiUiContextStore *context;
    UmiUiKeybindingRegistry *keybindings;
    UmiUiBreadcrumbModel *breadcrumbs;
    UmiUiExplorerModel *explorer;
    UmiUiWorkbenchState state;

    UmiMutex *mutex;
};

static void destroy_all(UmiUiWorkbench *workbench)
{
    if (workbench == NULL) return;

    /* Destroy the Batch 23 services before the older aggregate members. */
    umi_ui_explorer_model_destroy(workbench->explorer);
    umi_ui_breadcrumb_model_destroy(workbench->breadcrumbs);
    umi_ui_keybinding_registry_destroy(workbench->keybindings);
    umi_ui_context_store_destroy(workbench->context);
    umi_ui_view_container_model_destroy(workbench->view_containers);
    umi_ui_activity_model_destroy(workbench->activities);

    umi_ui_view_factory_registry_destroy(workbench->factories);
    umi_ui_event_queue_destroy(workbench->events);
    umi_ui_binding_registry_destroy(workbench->bindings);
    umi_ui_contribution_model_destroy(workbench->contributions);
    umi_ui_dialog_service_destroy(workbench->dialogs);
    umi_ui_notification_centre_destroy(workbench->notifications);
    umi_ui_selection_destroy(workbench->selection);
    umi_ui_status_model_destroy(workbench->status);
    umi_ui_toolbar_model_destroy(workbench->toolbars);
    umi_ui_menu_model_destroy(workbench->menus);
    umi_ui_action_model_destroy(workbench->actions);
    umi_ui_layout_destroy(workbench->layout);
    umi_ui_perspective_model_destroy(workbench->perspectives);
    umi_ui_editor_model_destroy(workbench->editors);
    umi_ui_document_view_model_destroy(workbench->documents);
    umi_ui_pane_model_destroy(workbench->panes);
    umi_mutex_destroy(workbench->mutex);
    free(workbench);
}

UmiStatus umi_ui_workbench_create(const char *id,
                                  UmiCommandRegistry *commands,
                                  UmiUiWorkbench **out_workbench)
{
    UmiUiWorkbench *workbench;
    UmiStatus status;

    if (out_workbench == NULL || commands == NULL || !umi_ui_id_is_valid(id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_workbench = NULL;

    workbench = (UmiUiWorkbench *)calloc(1U, sizeof(*workbench));
    if (workbench == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    (void)umi_ui_copy_text(workbench->id, sizeof(workbench->id), id);
    workbench->commands = commands;
    workbench->revision = 1U;
    umi_ui_workbench_state_init(&workbench->state);

    status = umi_mutex_create(&workbench->mutex);
    if (status == UMI_STATUS_OK) status = umi_ui_pane_model_create(&workbench->panes);
    if (status == UMI_STATUS_OK) status = umi_ui_document_view_model_create(&workbench->documents);
    if (status == UMI_STATUS_OK) status = umi_ui_editor_model_create(&workbench->editors);
    if (status == UMI_STATUS_OK) status = umi_ui_perspective_model_create(&workbench->perspectives);
    if (status == UMI_STATUS_OK) status = umi_ui_layout_create(&workbench->layout);
    if (status == UMI_STATUS_OK) status = umi_ui_action_model_create(&workbench->actions);
    if (status == UMI_STATUS_OK) status = umi_ui_menu_model_create(&workbench->menus);
    if (status == UMI_STATUS_OK) status = umi_ui_toolbar_model_create(&workbench->toolbars);
    if (status == UMI_STATUS_OK) status = umi_ui_status_model_create(&workbench->status);
    if (status == UMI_STATUS_OK) status = umi_ui_selection_create(&workbench->selection);
    if (status == UMI_STATUS_OK) status = umi_ui_notification_centre_create(&workbench->notifications);
    if (status == UMI_STATUS_OK) status = umi_ui_dialog_service_create(&workbench->dialogs);
    if (status == UMI_STATUS_OK) status = umi_ui_contribution_model_create(&workbench->contributions);
    if (status == UMI_STATUS_OK) status = umi_ui_binding_registry_create(&workbench->bindings);
    if (status == UMI_STATUS_OK) status = umi_ui_event_queue_create(&workbench->events);
    if (status == UMI_STATUS_OK) status = umi_ui_view_factory_registry_create(&workbench->factories);

    /* Batch 23 reusable workbench services. */
    if (status == UMI_STATUS_OK) status = umi_ui_activity_model_create(&workbench->activities);
    if (status == UMI_STATUS_OK) status = umi_ui_view_container_model_create(&workbench->view_containers);
    if (status == UMI_STATUS_OK) status = umi_ui_context_store_create(&workbench->context);
    if (status == UMI_STATUS_OK) status = umi_ui_keybinding_registry_create(&workbench->keybindings);
    if (status == UMI_STATUS_OK) status = umi_ui_breadcrumb_model_create(&workbench->breadcrumbs);
    if (status == UMI_STATUS_OK) status = umi_ui_explorer_model_create(&workbench->explorer);

    if (status != UMI_STATUS_OK) {
        destroy_all(workbench);
        return status;
    }

    *out_workbench = workbench;
    return UMI_STATUS_OK;
}

void umi_ui_workbench_destroy(UmiUiWorkbench *workbench)
{
    destroy_all(workbench);
}

UmiStatus umi_ui_workbench_activate_perspective(UmiUiWorkbench *workbench,
                                                const char *id)
{
    UmiUiPerspectiveSnapshot item;
    size_t index;
    if (workbench == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_ui_perspective_model_find(workbench->perspectives, id, &item) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }

    for (index = 0U; index < umi_ui_perspective_model_count(workbench->perspectives); ++index) {
        if (umi_ui_perspective_model_at(workbench->perspectives, index, &item) == UMI_STATUS_OK) {
            item.active = strcmp(item.perspective_id, id) == 0;
            (void)umi_ui_perspective_model_upsert(workbench->perspectives, &item);
        }
    }

    (void)umi_mutex_lock(workbench->mutex);
    (void)umi_ui_copy_text(workbench->active_perspective,
                           sizeof(workbench->active_perspective), id);
    (void)umi_ui_copy_text(workbench->state.active_perspective,
                           sizeof(workbench->state.active_perspective), id);
    workbench->state.revision = umi_ui_next_revision(workbench->state.revision);
    workbench->revision = umi_ui_next_revision(workbench->revision);
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workbench_activate_document(UmiUiWorkbench *workbench,
                                             const char *id)
{
    UmiUiDocumentViewSnapshot item;
    size_t index;
    if (workbench == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_ui_document_view_model_find(workbench->documents, id, &item) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }

    for (index = 0U; index < umi_ui_document_view_model_count(workbench->documents); ++index) {
        if (umi_ui_document_view_model_at(workbench->documents, index, &item) == UMI_STATUS_OK) {
            item.active = strcmp(item.view_id, id) == 0;
            (void)umi_ui_document_view_model_upsert(workbench->documents, &item);
        }
    }

    (void)umi_mutex_lock(workbench->mutex);
    (void)umi_ui_copy_text(workbench->active_document,
                           sizeof(workbench->active_document), id);
    (void)umi_ui_copy_text(workbench->state.active_document,
                           sizeof(workbench->state.active_document), id);
    workbench->state.revision = umi_ui_next_revision(workbench->state.revision);
    workbench->revision = umi_ui_next_revision(workbench->revision);
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workbench_activate_activity(UmiUiWorkbench *workbench,
                                             const char *activity_id)
{
    UmiUiActivitySnapshot activity;
    UmiStatus status;
    if (workbench == NULL || activity_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ui_activity_model_find(workbench->activities,
                                        activity_id, &activity);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_activity_model_set_active(workbench->activities, activity_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_view_container_model_set_active(workbench->view_containers,
                                                    activity.container_id);
    if (status != UMI_STATUS_OK) return status;

    /*
     * A left-side view container controls which left workbench panes are visible.
     * Right and bottom panes remain independent utilities and are not destroyed.
     */
    {
        UmiUiViewContainerSnapshot container;
        if (umi_ui_view_container_model_find(workbench->view_containers,
                                             activity.container_id,
                                             &container) == UMI_STATUS_OK &&
            container.placement == UMI_UI_PLACEMENT_LEFT) {
            size_t pane_index;
            for (pane_index = 0U;
                 pane_index < umi_ui_pane_model_count(workbench->panes);
                 ++pane_index) {
                UmiUiPaneSnapshot pane;
                size_t view_index;
                int belongs = 0;
                if (umi_ui_pane_model_at(workbench->panes,
                                         pane_index,
                                         &pane) != UMI_STATUS_OK ||
                    pane.placement != UMI_UI_PLACEMENT_LEFT) {
                    continue;
                }
                for (view_index = 0U; view_index < container.view_count; ++view_index) {
                    if (strcmp(container.view_ids[view_index], pane.pane_id) == 0 ||
                        strcmp(container.view_ids[view_index], pane.view_type) == 0) {
                        belongs = 1;
                        break;
                    }
                }
                pane.visible = belongs;
                (void)umi_ui_pane_model_upsert(workbench->panes, &pane);
            }
        }
    }

    (void)umi_mutex_lock(workbench->mutex);
    (void)umi_ui_copy_text(workbench->active_activity,
                           sizeof(workbench->active_activity), activity_id);
    (void)umi_ui_copy_text(workbench->active_view_container,
                           sizeof(workbench->active_view_container),
                           activity.container_id);
    (void)umi_ui_copy_text(workbench->state.active_activity,
                           sizeof(workbench->state.active_activity), activity_id);
    (void)umi_ui_copy_text(workbench->state.active_view_container,
                           sizeof(workbench->state.active_view_container),
                           activity.container_id);
    workbench->state.revision = umi_ui_next_revision(workbench->state.revision);
    workbench->revision = umi_ui_next_revision(workbench->revision);
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workbench_execute_action(UmiUiWorkbench *workbench,
                                          const char *action_id,
                                          const char *argument,
                                          char *out_message,
                                          size_t capacity)
{
    UmiUiActionSnapshot action;
    if (workbench == NULL || action_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_ui_action_model_find(workbench->actions, action_id, &action) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (!action.enabled || !action.visible) return UMI_STATUS_PERMISSION_DENIED;
    return umi_command_registry_execute(workbench->commands,
                                        action.command_id,
                                        argument,
                                        out_message,
                                        capacity);
}

UmiStatus umi_ui_workbench_resolve_keybinding(
    UmiUiWorkbench *workbench,
    const char *chord,
    UmiUiKeybindingResolution *out_resolution)
{
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ui_keybinding_registry_resolve(workbench->keybindings,
                                              workbench->context,
                                              chord,
                                              out_resolution);
}

UmiStatus umi_ui_workbench_quick_access(
    UmiUiWorkbench *workbench,
    const char *query,
    UmiUiQuickAccessResults *out_results)
{
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ui_quick_access_search_commands(workbench->commands,
                                               query,
                                               out_results);
}

UmiStatus umi_ui_workbench_set_breadcrumb_path(UmiUiWorkbench *workbench,
                                               const char *path)
{
    UmiStatus status;
    if (workbench == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_breadcrumb_set_path(workbench->breadcrumbs, path);
    if (status == UMI_STATUS_OK) {
        (void)umi_mutex_lock(workbench->mutex);
        workbench->revision = umi_ui_next_revision(workbench->revision);
        (void)umi_mutex_unlock(workbench->mutex);
    }
    return status;
}

UmiStatus umi_ui_workbench_snapshot(const UmiUiWorkbench *workbench,
                                    UmiUiWorkbenchSnapshot *out_snapshot)
{
    if (workbench == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(workbench->mutex);
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)umi_ui_copy_text(out_snapshot->workbench_id,
                           sizeof(out_snapshot->workbench_id), workbench->id);
    (void)umi_ui_copy_text(out_snapshot->active_perspective,
                           sizeof(out_snapshot->active_perspective),
                           workbench->active_perspective);
    (void)umi_ui_copy_text(out_snapshot->active_document_view,
                           sizeof(out_snapshot->active_document_view),
                           workbench->active_document);
    (void)umi_ui_copy_text(out_snapshot->active_activity,
                           sizeof(out_snapshot->active_activity),
                           workbench->active_activity);
    (void)umi_ui_copy_text(out_snapshot->active_view_container,
                           sizeof(out_snapshot->active_view_container),
                           workbench->active_view_container);

    out_snapshot->pane_count = umi_ui_pane_model_count(workbench->panes);
    out_snapshot->document_count = umi_ui_document_view_model_count(workbench->documents);
    out_snapshot->perspective_count = umi_ui_perspective_model_count(workbench->perspectives);
    out_snapshot->action_count = umi_ui_action_model_count(workbench->actions);
    out_snapshot->notification_count = umi_ui_notification_count(workbench->notifications, 0);
    out_snapshot->activity_count = umi_ui_activity_model_count(workbench->activities);
    out_snapshot->view_container_count =
        umi_ui_view_container_model_count(workbench->view_containers);
    out_snapshot->keybinding_count =
        umi_ui_keybinding_registry_count(workbench->keybindings);
    out_snapshot->context_key_count = umi_ui_context_count(workbench->context);
    out_snapshot->explorer_node_count = umi_ui_explorer_model_count(workbench->explorer);
    out_snapshot->revision = workbench->revision;
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workbench_state_snapshot(const UmiUiWorkbench *workbench,
                                          UmiUiWorkbenchState *out_state)
{
    if (workbench == NULL || out_state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(workbench->mutex);
    *out_state = workbench->state;
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_workbench_state_apply(UmiUiWorkbench *workbench,
                                       const UmiUiWorkbenchState *state)
{
    UmiStatus status = UMI_STATUS_OK;
    if (workbench == NULL || state == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (state->active_activity[0] != '\0') {
        status = umi_ui_workbench_activate_activity(workbench,
                                                   state->active_activity);
        if (status != UMI_STATUS_OK) return status;
    }
    if (state->active_perspective[0] != '\0') {
        status = umi_ui_workbench_activate_perspective(workbench,
                                                       state->active_perspective);
        if (status != UMI_STATUS_OK) return status;
    }
    if (state->active_document[0] != '\0') {
        status = umi_ui_workbench_activate_document(workbench,
                                                    state->active_document);
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    }

    (void)umi_mutex_lock(workbench->mutex);
    workbench->state.sidebar_visible = state->sidebar_visible;
    workbench->state.auxiliary_sidebar_visible = state->auxiliary_sidebar_visible;
    workbench->state.bottom_panel_visible = state->bottom_panel_visible;
    workbench->state.sidebar_size = state->sidebar_size;
    workbench->state.auxiliary_sidebar_size = state->auxiliary_sidebar_size;
    workbench->state.bottom_panel_size = state->bottom_panel_size;
    workbench->state.revision = umi_ui_next_revision(workbench->state.revision);
    workbench->revision = umi_ui_next_revision(workbench->revision);
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

UmiCommandRegistry *umi_ui_workbench_commands(UmiUiWorkbench *workbench)
{
    return workbench != NULL ? workbench->commands : NULL;
}

#define ACCESSOR(type, name, field) \
    type *name(UmiUiWorkbench *workbench) { \
        return workbench != NULL ? workbench->field : NULL; \
    }

ACCESSOR(UmiUiPaneModel, umi_ui_workbench_panes, panes)
ACCESSOR(UmiUiDocumentViewModel, umi_ui_workbench_documents, documents)
ACCESSOR(UmiUiEditorModel, umi_ui_workbench_editors, editors)
ACCESSOR(UmiUiPerspectiveModel, umi_ui_workbench_perspectives, perspectives)
ACCESSOR(UmiUiLayout, umi_ui_workbench_layout, layout)
ACCESSOR(UmiUiActionModel, umi_ui_workbench_actions, actions)
ACCESSOR(UmiUiMenuModel, umi_ui_workbench_menus, menus)
ACCESSOR(UmiUiToolbarModel, umi_ui_workbench_toolbars, toolbars)
ACCESSOR(UmiUiStatusModel, umi_ui_workbench_status, status)
ACCESSOR(UmiUiSelectionModel, umi_ui_workbench_selection, selection)
ACCESSOR(UmiUiNotificationCentre, umi_ui_workbench_notifications, notifications)
ACCESSOR(UmiUiDialogService, umi_ui_workbench_dialogs, dialogs)
ACCESSOR(UmiUiContributionModel, umi_ui_workbench_contributions, contributions)
ACCESSOR(UmiUiBindingRegistry, umi_ui_workbench_bindings, bindings)
ACCESSOR(UmiUiEventQueue, umi_ui_workbench_events, events)
ACCESSOR(UmiUiViewFactoryRegistry, umi_ui_workbench_view_factories, factories)
ACCESSOR(UmiUiActivityModel, umi_ui_workbench_activities, activities)
ACCESSOR(UmiUiViewContainerModel, umi_ui_workbench_view_containers, view_containers)
ACCESSOR(UmiUiContextStore, umi_ui_workbench_context, context)
ACCESSOR(UmiUiKeybindingRegistry, umi_ui_workbench_keybindings, keybindings)
ACCESSOR(UmiUiBreadcrumbModel, umi_ui_workbench_breadcrumbs, breadcrumbs)
ACCESSOR(UmiUiExplorerModel, umi_ui_workbench_explorer, explorer)

#undef ACCESSOR
