/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workbench.c
 *
 * PURPOSE:
 *   Implement the authoritative workbench aggregate and its command, navigation and session-state coordination.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The workbench owns reusable models but not product business data. Activating an activity selects its view container; commands still execute through the canonical Runtime command registry.
 */

#include "umicom/ui/workbench.h"

#include <ctype.h>
#include <stdio.h>
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
    UmiUiWorkspaceProfileModel *workspace_profiles;
    UmiUiAppearanceModel *appearance;
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

/* Provide the destroy all operation used by this module and its client applications. */
static void destroy_all(UmiUiWorkbench *workbench)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    umi_ui_appearance_model_destroy(workbench->appearance);
    umi_ui_workspace_profile_model_destroy(workbench->workspace_profiles);
    umi_ui_perspective_model_destroy(workbench->perspectives);
    umi_ui_editor_model_destroy(workbench->editors);
    umi_ui_document_view_model_destroy(workbench->documents);
    umi_ui_pane_model_destroy(workbench->panes);
    umi_mutex_destroy(workbench->mutex);
    free(workbench);
}

/*
 * Initialise ui workbench from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_workbench_create(const char *id,
                                  UmiCommandRegistry *commands,
                                  UmiUiWorkbench **out_workbench)
{
    UmiUiWorkbench *workbench;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_workbench == NULL || commands == NULL || !umi_ui_id_is_valid(id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_workbench = NULL;

    workbench = (UmiUiWorkbench *)calloc(1U, sizeof(*workbench));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    (void)umi_ui_copy_text(workbench->id, sizeof(workbench->id), id);
    workbench->commands = commands;
    workbench->revision = 1U;
    umi_ui_workbench_state_init(&workbench->state);

    status = umi_mutex_create(&workbench->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_pane_model_create(&workbench->panes);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_document_view_model_create(&workbench->documents);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_editor_model_create(&workbench->editors);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_perspective_model_create(&workbench->perspectives);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_profile_model_create(&workbench->workspace_profiles);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_appearance_model_create(&workbench->appearance);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_layout_create(&workbench->layout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_action_model_create(&workbench->actions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_menu_model_create(&workbench->menus);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_toolbar_model_create(&workbench->toolbars);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_status_model_create(&workbench->status);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_selection_create(&workbench->selection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_notification_centre_create(&workbench->notifications);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_dialog_service_create(&workbench->dialogs);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_contribution_model_create(&workbench->contributions);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_binding_registry_create(&workbench->bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_event_queue_create(&workbench->events);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_view_factory_registry_create(&workbench->factories);

    /* Batch 23 reusable workbench services. */
    if (status == UMI_STATUS_OK) status = umi_ui_activity_model_create(&workbench->activities);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_view_container_model_create(&workbench->view_containers);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_context_store_create(&workbench->context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_keybinding_registry_create(&workbench->keybindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_breadcrumb_model_create(&workbench->breadcrumbs);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_ui_explorer_model_create(&workbench->explorer);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        destroy_all(workbench);
        return status;
    }

    *out_workbench = workbench;
    return UMI_STATUS_OK;
}

/* Release or reset state held by ui workbench so the same storage can be reused safely. */
void umi_ui_workbench_destroy(UmiUiWorkbench *workbench)
{
    destroy_all(workbench);
}

/*
 * Provide the ui workbench activate perspective operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_activate_perspective(UmiUiWorkbench *workbench,
                                                const char *id)
{
    UmiUiPerspectiveSnapshot item;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_perspective_model_find(workbench->perspectives, id, &item) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ui_perspective_model_count(workbench->perspectives); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the ui workbench activate document operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_activate_document(UmiUiWorkbench *workbench,
                                             const char *id)
{
    UmiUiDocumentViewSnapshot item;
    char target_group[UMI_UI_ID_CAPACITY];
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_document_view_model_find(workbench->documents, id, &item) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }
    (void)umi_ui_copy_text(
        target_group, sizeof(target_group),
        item.group_id[0] != '\0'
            ? item.group_id
            : UMI_UI_PRIMARY_EDITOR_GROUP_ID);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ui_document_view_model_count(workbench->documents); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_ui_document_view_model_at(workbench->documents, index, &item) == UMI_STATUS_OK) {
            const char *item_group = item.group_id[0] != '\0'
                ? item.group_id
                : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(item_group, target_group) == 0) {
                item.active = strcmp(item.view_id, id) == 0;
                (void)umi_ui_document_view_model_upsert(
                    workbench->documents, &item);
            }
        }
    }

    (void)umi_mutex_lock(workbench->mutex);
    (void)umi_ui_copy_text(workbench->active_document,
                           sizeof(workbench->active_document), id);
    (void)umi_ui_copy_text(workbench->state.active_document,
                           sizeof(workbench->state.active_document), id);
    (void)umi_ui_copy_text(workbench->state.active_editor_group,
                           sizeof(workbench->state.active_editor_group),
                           target_group);
    workbench->state.revision = umi_ui_next_revision(workbench->state.revision);
    workbench->revision = umi_ui_next_revision(workbench->revision);
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui workbench activate activity operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_activate_activity(UmiUiWorkbench *workbench,
                                             const char *activity_id)
{
    UmiUiActivitySnapshot activity;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || activity_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ui_activity_model_find(workbench->activities,
                                        activity_id, &activity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_activity_model_set_active(workbench->activities, activity_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_view_container_model_set_active(workbench->view_containers,
                                                    activity.container_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * A left-side view container controls which left workbench panes are visible.
     * Right and bottom panes remain independent utilities and are not destroyed.
     */
    {
        UmiUiViewContainerSnapshot container;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ui_view_container_model_find(workbench->view_containers,
                                             activity.container_id,
                                             &container) == UMI_STATUS_OK &&
            container.placement == UMI_UI_PLACEMENT_LEFT) {
            size_t pane_index;
            /* Visit each bounded item once so every record receives the same rule. */
            for (pane_index = 0U;
                 pane_index < umi_ui_pane_model_count(workbench->panes);
                 ++pane_index) {
                UmiUiPaneSnapshot pane;
                size_t view_index;
                int belongs = 0;
                /* Apply this branch only when its contract condition is satisfied. */
                if (umi_ui_pane_model_at(workbench->panes,
                                         pane_index,
                                         &pane) != UMI_STATUS_OK ||
                    pane.placement != UMI_UI_PLACEMENT_LEFT) {
                    continue;
                }
                /* Visit each bounded item once so every record receives the same rule. */
                for (view_index = 0U; view_index < container.view_count; ++view_index) {
                    /* Use the stable identifier comparison to choose the matching record or policy. */
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

/*
 * Provide the ui workbench activate workspace profile operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workbench_activate_workspace_profile(
    UmiUiWorkbench *workbench,
    const char *profile_id)
{
    UmiUiWorkspaceProfileSnapshot profile;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || profile_id == NULL || profile_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_workspace_profile_model_find(workbench->workspace_profiles,
                                                 profile_id,
                                                 &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_profile_model_set_active(
        workbench->workspace_profiles, profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Reapply every saved tool-window position before the frontend refreshes.
     * Unknown pane IDs are deliberately ignored so a layout remains usable
     * when an optional extension is not installed. */
    {
        size_t pane_index;
        /* Apply this branch only when its contract condition is satisfied. */
        if (profile.pane_count > 0U) {
            size_t existing_index;
            /* Visit each bounded item once so every record receives the same rule. */
            for (existing_index = 0U;
                 existing_index < umi_ui_pane_model_count(workbench->panes);
                 ++existing_index) {
                UmiUiPaneSnapshot existing;
                /* Apply this branch only when its contract condition is satisfied. */
                if (umi_ui_pane_model_at(workbench->panes,
                                         existing_index,
                                         &existing) == UMI_STATUS_OK &&
                    existing.movable &&
                    existing.placement != UMI_UI_PLACEMENT_CENTRE) {
                    existing.visible = 0;
                    (void)umi_ui_pane_model_upsert(workbench->panes,
                                                   &existing);
                }
            }
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (pane_index = 0U; pane_index < profile.pane_count; ++pane_index) {
            UmiUiPaneSnapshot pane;
            const UmiUiWorkspacePanePlacement *saved =
                &profile.panes[pane_index];
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_ui_pane_model_find(workbench->panes,
                                       saved->pane_id,
                                       &pane) == UMI_STATUS_OK) {
                pane.placement = saved->placement;
                pane.order = saved->order;
                pane.visible = saved->visible;
                (void)umi_ui_pane_model_upsert(workbench->panes, &pane);
            }
        }
    }

    /*
     * Apply the complete chrome profile atomically from the workbench's point
     * of view. Frontend adapters observe the resulting state on their normal
     * refresh path, so there is no toolkit-specific profile implementation.
     */
    (void)umi_mutex_lock(workbench->mutex);
    (void)umi_ui_copy_text(workbench->state.active_workspace_profile,
                           sizeof(workbench->state.active_workspace_profile),
                           profile.profile_id);
    workbench->state.sidebar_visible = profile.sidebar_visible;
    workbench->state.auxiliary_sidebar_visible =
        profile.auxiliary_sidebar_visible;
    workbench->state.bottom_panel_visible = profile.bottom_panel_visible;
    workbench->state.sidebar_size = profile.sidebar_size;
    workbench->state.auxiliary_sidebar_size =
        profile.auxiliary_sidebar_size;
    workbench->state.bottom_panel_size = profile.bottom_panel_size;
    workbench->state.editor_split_mode = profile.editor_split_mode;
    workbench->state.editor_split_ratio = profile.editor_split_ratio;
    workbench->state.revision = umi_ui_next_revision(workbench->state.revision);
    workbench->revision = umi_ui_next_revision(workbench->revision);
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the capture profile state operation used by this module and its client
 * applications.
 */
static UmiStatus capture_profile_state(
    UmiUiWorkbench *workbench,
    UmiUiWorkspaceProfileSnapshot *profile)
{
    UmiUiWorkbenchState state;
    size_t index;
    UmiStatus status;

    status = umi_ui_workbench_state_snapshot(workbench, &state);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    profile->sidebar_visible = state.sidebar_visible;
    profile->auxiliary_sidebar_visible = state.auxiliary_sidebar_visible;
    profile->bottom_panel_visible = state.bottom_panel_visible;
    profile->sidebar_size = state.sidebar_size;
    profile->auxiliary_sidebar_size = state.auxiliary_sidebar_size;
    profile->bottom_panel_size = state.bottom_panel_size;
    profile->editor_split_mode = state.editor_split_mode;
    profile->editor_split_ratio = state.editor_split_ratio;
    profile->pane_count = 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ui_pane_model_count(workbench->panes);
         ++index) {
        UmiUiPaneSnapshot pane;
        UmiUiWorkspacePanePlacement *saved;
        status = umi_ui_pane_model_at(workbench->panes, index, &pane);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this operation only while the related capability or state is available. */
        if (!pane.visible || pane.placement == UMI_UI_PLACEMENT_FLOATING ||
            pane.placement == UMI_UI_PLACEMENT_CENTRE) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (profile->pane_count >= UMI_UI_WORKSPACE_PROFILE_MAX_PANES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        saved = &profile->panes[profile->pane_count++];
        (void)memset(saved, 0, sizeof(*saved));
        (void)umi_ui_copy_text(saved->pane_id,
                               sizeof(saved->pane_id),
                               pane.pane_id);
        saved->placement = pane.placement;
        saved->order = pane.order;
        saved->visible = pane.visible;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the make custom profile id operation used by this module and its client
 * applications.
 */
static UmiStatus make_custom_profile_id(
    UmiUiWorkspaceProfileModel *model,
    const char *label,
    char *out_id,
    size_t capacity)
{
    char base[UMI_UI_ID_CAPACITY] = "custom-";
    size_t read_index;
    size_t write_index = strlen(base);
    unsigned suffix;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || label == NULL || label[0] == '\0' ||
        out_id == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (read_index = 0U; label[read_index] != '\0' &&
         write_index + 1U < sizeof(base); ++read_index) {
        unsigned char value = (unsigned char)label[read_index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (isalnum(value)) {
            base[write_index++] = (char)tolower(value);
        } else /* Apply this branch only when its contract condition is satisfied. */ if (write_index > strlen("custom-") &&
                   base[write_index - 1U] != '-') {
            base[write_index++] = '-';
        }
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (write_index > strlen("custom-") &&
           base[write_index - 1U] == '-') {
        --write_index;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (write_index == strlen("custom-")) {
        (void)snprintf(base, sizeof(base), "%s", "custom-layout");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        base[write_index] = '\0';
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (suffix = 1U; suffix < 10000U; ++suffix) {
        UmiUiWorkspaceProfileSnapshot existing;
        int written = suffix == 1U
            ? snprintf(out_id, capacity, "%s", base)
            : snprintf(out_id, capacity, "%s-%u", base, suffix);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ui_workspace_profile_model_find(model,
                                                out_id,
                                                &existing) ==
            UMI_STATUS_NOT_FOUND) {
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Provide the ui workbench save workspace profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_save_workspace_profile(
    UmiUiWorkbench *workbench,
    const char *label,
    const char *description,
    char *out_profile_id,
    size_t capacity)
{
    UmiUiWorkspaceProfileSnapshot profile = {0};
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || label == NULL || description == NULL ||
        out_profile_id == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = make_custom_profile_id(workbench->workspace_profiles,
                                    label,
                                    profile.profile_id,
                                    sizeof(profile.profile_id));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ui_copy_text(profile.label, sizeof(profile.label), label) ||
        !umi_ui_copy_text(profile.description, sizeof(profile.description),
                          description) ||
        !umi_ui_copy_text(profile.icon_name, sizeof(profile.icon_name),
                          "view-grid-symbolic")) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    profile.order = 1000 + (int32_t)
        umi_ui_workspace_profile_model_count(workbench->workspace_profiles);
    profile.built_in = 0;
    profile.locked = 0;
    status = capture_profile_state(workbench, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workspace_profile_model_upsert(
        workbench->workspace_profiles, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_activate_workspace_profile(
        workbench, profile.profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)umi_ui_workspace_profile_model_remove(
            workbench->workspace_profiles, profile.profile_id);
        return status;
    }
    return umi_ui_copy_text(out_profile_id, capacity, profile.profile_id)
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Provide the ui workbench update workspace profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_update_workspace_profile(
    UmiUiWorkbench *workbench,
    const char *profile_id,
    const char *label,
    const char *description)
{
    UmiUiWorkspaceProfileSnapshot profile;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || profile_id == NULL || label == NULL ||
        description == NULL || label[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_workspace_profile_model_find(
        workbench->workspace_profiles, profile_id, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile.built_in || profile.locked) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_ui_copy_text(profile.label, sizeof(profile.label), label) ||
        !umi_ui_copy_text(profile.description, sizeof(profile.description),
                          description)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = capture_profile_state(workbench, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_workspace_profile_model_upsert(
        workbench->workspace_profiles, &profile);
}

/*
 * Provide the ui workbench set workspace profile locked operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workbench_set_workspace_profile_locked(
    UmiUiWorkbench *workbench,
    const char *profile_id,
    int locked)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ui_workspace_profile_model_set_locked(
        workbench->workspace_profiles, profile_id, locked);
}

/*
 * Provide the ui workbench remove workspace profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_remove_workspace_profile(
    UmiUiWorkbench *workbench,
    const char *profile_id)
{
    UmiUiWorkspaceProfileSnapshot profile;
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || profile_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_workspace_profile_model_find(
        workbench->workspace_profiles, profile_id, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile.built_in || profile.locked) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (profile.active) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U;
             index < umi_ui_workspace_profile_model_count(
                 workbench->workspace_profiles);
             ++index) {
            UmiUiWorkspaceProfileSnapshot fallback;
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_ui_workspace_profile_model_at(
                    workbench->workspace_profiles,
                    index,
                    &fallback) == UMI_STATUS_OK &&
                strcmp(fallback.profile_id, profile_id) != 0 &&
                fallback.built_in) {
                status = umi_ui_workbench_activate_workspace_profile(
                    workbench, fallback.profile_id);
                break;
            }
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_ui_workspace_profile_model_remove(
        workbench->workspace_profiles, profile_id);
}

/*
 * Provide the ui workbench dock pane operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_dock_pane(
    UmiUiWorkbench *workbench,
    const char *pane_id,
    UmiUiPlacement placement,
    int32_t order)
{
    UmiUiPaneSnapshot pane;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || pane_id == NULL ||
        (placement != UMI_UI_PLACEMENT_LEFT &&
         placement != UMI_UI_PLACEMENT_RIGHT &&
         placement != UMI_UI_PLACEMENT_BOTTOM)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_pane_model_find(workbench->panes, pane_id, &pane);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!pane.movable) return UMI_STATUS_PERMISSION_DENIED;
    pane.placement = placement;
    pane.order = order;
    pane.visible = 1;
    status = umi_ui_pane_model_upsert(workbench->panes, &pane);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(workbench->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (placement == UMI_UI_PLACEMENT_LEFT) {
        workbench->state.sidebar_visible = 1;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (placement == UMI_UI_PLACEMENT_RIGHT) {
        workbench->state.auxiliary_sidebar_visible = 1;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        workbench->state.bottom_panel_visible = 1;
    }
    workbench->state.revision = umi_ui_next_revision(
        workbench->state.revision);
    workbench->revision = umi_ui_next_revision(workbench->revision);
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui workbench workspace profile modified operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_workbench_workspace_profile_modified(
    UmiUiWorkbench *workbench,
    const char *profile_id,
    int *out_modified)
{
    UmiUiWorkspaceProfileSnapshot profile;
    UmiUiWorkbenchState state;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || profile_id == NULL || out_modified == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_modified = 0;
    status = umi_ui_workspace_profile_model_find(
        workbench->workspace_profiles, profile_id, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_workbench_state_snapshot(workbench, &state);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this operation only while the related capability or state is available. */
    if (profile.sidebar_visible != state.sidebar_visible ||
        profile.auxiliary_sidebar_visible !=
            state.auxiliary_sidebar_visible ||
        profile.bottom_panel_visible != state.bottom_panel_visible ||
        profile.sidebar_size != state.sidebar_size ||
        profile.auxiliary_sidebar_size != state.auxiliary_sidebar_size ||
        profile.bottom_panel_size != state.bottom_panel_size) {
        *out_modified = 1;
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile.editor_split_mode != state.editor_split_mode ||
        profile.editor_split_ratio != state.editor_split_ratio) {
        *out_modified = 1;
        return UMI_STATUS_OK;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile.pane_count; ++index) {
        UmiUiPaneSnapshot pane;
        const UmiUiWorkspacePanePlacement *saved = &profile.panes[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ui_pane_model_find(workbench->panes,
                                   saved->pane_id,
                                   &pane) != UMI_STATUS_OK ||
            pane.placement != saved->placement ||
            pane.order != saved->order ||
            pane.visible != saved->visible) {
            *out_modified = 1;
            break;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (*out_modified == 0) {
        size_t pane_index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (pane_index = 0U;
             pane_index < umi_ui_pane_model_count(workbench->panes);
             ++pane_index) {
            UmiUiPaneSnapshot pane;
            size_t saved_index;
            int found = 0;
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_ui_pane_model_at(workbench->panes,
                                     pane_index,
                                     &pane) != UMI_STATUS_OK ||
                !pane.visible || !pane.movable ||
                pane.placement == UMI_UI_PLACEMENT_CENTRE ||
                pane.placement == UMI_UI_PLACEMENT_FLOATING) {
                continue;
            }
            /* Visit each bounded item once so every record receives the same rule. */
            for (saved_index = 0U; saved_index < profile.pane_count;
                 ++saved_index) {
                /* Use the stable identifier comparison to choose the matching record or policy. */
                if (strcmp(profile.panes[saved_index].pane_id,
                           pane.pane_id) == 0) {
                    found = 1;
                    break;
                }
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if (!found) {
                *out_modified = 1;
                break;
            }
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ui workbench execute action operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_execute_action(UmiUiWorkbench *workbench,
                                          const char *action_id,
                                          const char *argument,
                                          char *out_message,
                                          size_t capacity)
{
    UmiUiActionSnapshot action;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || action_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ui_action_model_find(workbench->actions, action_id, &action) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!action.enabled || !action.visible) return UMI_STATUS_PERMISSION_DENIED;
    return umi_command_registry_execute(workbench->commands,
                                        action.command_id,
                                        argument != NULL && argument[0] != '\0'
                                            ? argument
                                            : action.argument,
                                        out_message,
                                        capacity);
}

/*
 * Provide the ui workbench resolve keybinding operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_resolve_keybinding(
    UmiUiWorkbench *workbench,
    const char *chord,
    UmiUiKeybindingResolution *out_resolution)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ui_keybinding_registry_resolve(workbench->keybindings,
                                              workbench->context,
                                              chord,
                                              out_resolution);
}

/*
 * Provide the ui workbench quick access operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_quick_access(
    UmiUiWorkbench *workbench,
    const char *query,
    UmiUiQuickAccessResults *out_results)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ui_quick_access_search_actions(workbench->commands,
                                              workbench->actions,
                                              query,
                                              out_results);
}

/*
 * Provide the ui workbench set breadcrumb path operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_workbench_set_breadcrumb_path(UmiUiWorkbench *workbench,
                                               const char *path)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_breadcrumb_set_path(workbench->breadcrumbs, path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)umi_mutex_lock(workbench->mutex);
        workbench->revision = umi_ui_next_revision(workbench->revision);
        (void)umi_mutex_unlock(workbench->mutex);
    }
    return status;
}

/*
 * Provide the ui workbench snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_snapshot(const UmiUiWorkbench *workbench,
                                    UmiUiWorkbenchSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(workbench->mutex);
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)umi_ui_copy_text(out_snapshot->workbench_id,
                           sizeof(out_snapshot->workbench_id), workbench->id);
    (void)umi_ui_copy_text(out_snapshot->active_workspace_profile,
                           sizeof(out_snapshot->active_workspace_profile),
                           workbench->state.active_workspace_profile);
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
    out_snapshot->workspace_profile_count =
        umi_ui_workspace_profile_model_count(workbench->workspace_profiles);
    out_snapshot->appearance_profile_count =
        umi_ui_appearance_model_count(workbench->appearance);
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

/*
 * Provide the ui workbench state snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_workbench_state_snapshot(const UmiUiWorkbench *workbench,
                                          UmiUiWorkbenchState *out_state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || out_state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(workbench->mutex);
    *out_state = workbench->state;
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

/*
 * Perform ui workbench state through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ui_workbench_state_apply(UmiUiWorkbench *workbench,
                                       const UmiUiWorkbenchState *state)
{
    UmiStatus status = UMI_STATUS_OK;
    int32_t split_ratio;
    const char *active_editor_group;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    split_ratio = state->editor_split_ratio == 0
        ? UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT
        : state->editor_split_ratio;
    active_editor_group = state->active_editor_group[0] != '\0'
        ? state->active_editor_group
        : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
    /* Apply this branch only when its contract condition is satisfied. */
    if (state->editor_split_mode < UMI_UI_EDITOR_SPLIT_SINGLE ||
        state->editor_split_mode > UMI_UI_EDITOR_SPLIT_ROWS ||
        split_ratio < UMI_UI_EDITOR_SPLIT_RATIO_MIN ||
        split_ratio > UMI_UI_EDITOR_SPLIT_RATIO_MAX ||
        !umi_ui_id_is_valid(active_editor_group)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this operation only while the related capability or state is available. */
    if (state->active_workspace_profile[0] != '\0') {
        status = umi_ui_workbench_activate_workspace_profile(
            workbench, state->active_workspace_profile);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (state->active_activity[0] != '\0') {
        status = umi_ui_workbench_activate_activity(workbench,
                                                   state->active_activity);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (state->active_perspective[0] != '\0') {
        status = umi_ui_workbench_activate_perspective(workbench,
                                                       state->active_perspective);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (state->active_document[0] != '\0') {
        status = umi_ui_workbench_activate_document(workbench,
                                                    state->active_document);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) return status;
    }

    (void)umi_mutex_lock(workbench->mutex);
    workbench->state.sidebar_visible = state->sidebar_visible;
    workbench->state.auxiliary_sidebar_visible = state->auxiliary_sidebar_visible;
    workbench->state.bottom_panel_visible = state->bottom_panel_visible;
    workbench->state.sidebar_size = state->sidebar_size;
    workbench->state.auxiliary_sidebar_size = state->auxiliary_sidebar_size;
    workbench->state.bottom_panel_size = state->bottom_panel_size;
    workbench->state.editor_split_mode = state->editor_split_mode;
    workbench->state.editor_split_ratio = split_ratio;
    (void)umi_ui_copy_text(workbench->state.active_editor_group,
                           sizeof(workbench->state.active_editor_group),
                           active_editor_group);
    /* Apply this operation only while the related capability or state is available. */
    if (state->active_workspace_profile[0] != '\0') {
        (void)umi_ui_copy_text(workbench->state.active_workspace_profile,
                               sizeof(workbench->state.active_workspace_profile),
                               state->active_workspace_profile);
    }
    workbench->state.revision = umi_ui_next_revision(workbench->state.revision);
    workbench->revision = umi_ui_next_revision(workbench->revision);
    (void)umi_mutex_unlock(workbench->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui workbench commands operation used by this module and its client
 * applications.
 */
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
ACCESSOR(UmiUiWorkspaceProfileModel, umi_ui_workbench_workspace_profiles, workspace_profiles)
ACCESSOR(UmiUiAppearanceModel, umi_ui_workbench_appearance, appearance)
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
