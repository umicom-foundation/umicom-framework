/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/customisation.c
 *
 * PURPOSE:
 *   Populate shared application workspaces and enforce portable panel
 *   behaviour before the toolkit-neutral UI layout is changed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/customisation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/geometry.h"
#include "umicom/application/suite_layout/projection.h"

/* Copy stable IDs and display text without silently truncating either. */
static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    int written;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/* Choose a broad searchable category from portable capability metadata. */
static UmiUiWindowCategory panel_category(
    const UmiExperiencePanelDefinition *panel)
{
    const char *capability;

    if (panel == NULL) {
        return UMI_UI_WINDOW_CATEGORY_GENERAL;
    }
    capability = panel->required_capability != NULL
        ? panel->required_capability
        : "";
    if (strstr(capability, "ai") != NULL ||
        strstr(panel->panel_id, "assistant") != NULL) {
        return UMI_UI_WINDOW_CATEGORY_AI;
    }
    if (strstr(capability, "trading") != NULL ||
        strstr(capability, "market") != NULL) {
        return UMI_UI_WINDOW_CATEGORY_TRADING;
    }
    if (strstr(capability, "data") != NULL ||
        strstr(capability, "database") != NULL) {
        return UMI_UI_WINDOW_CATEGORY_DATA;
    }
    if (strstr(capability, "project") != NULL ||
        strstr(capability, "editor") != NULL ||
        strstr(capability, "debug") != NULL) {
        return UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    }
    if (strstr(panel->panel_id, "explorer") != NULL ||
        strstr(panel->panel_id, "navigator") != NULL) {
        return UMI_UI_WINDOW_CATEGORY_NAVIGATION;
    }
    return UMI_UI_WINDOW_CATEGORY_OPERATIONS;
}

/* Register one experience panel as a reusable New Window catalogue item. */
static UmiStatus register_panel(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiExperiencePanelDefinition *panel)
{
    UmiUiWindowDescriptor descriptor;
    UmiApplicationSuiteLayoutRect bounds;
    UmiUiPlacement placement;
    UmiStatus status;

    if (customisation == NULL || panel == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&descriptor, 0, sizeof(descriptor));
    status = copy_text(
        descriptor.tool_id, sizeof(descriptor.tool_id), panel->panel_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            descriptor.title, sizeof(descriptor.title), panel->title);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            descriptor.description,
            sizeof(descriptor.description),
            panel->summary != NULL ? panel->summary : panel->title);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            descriptor.icon_name,
            sizeof(descriptor.icon_name),
            "application-panel");
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Multiplicity is product metadata, while safe instance naming and layout
     * capacity remain Framework responsibilities. */
    descriptor.category = panel_category(panel);
    descriptor.supports_multiple =
        (panel->flags & UMI_EXPERIENCE_PANEL_MULTI_INSTANCE) != 0U;
    if (umi_ui_placement_parse(panel->default_region, &placement) !=
        UMI_STATUS_OK) {
        placement = UMI_UI_PLACEMENT_CENTRE;
    }
    bounds = umi_application_suite_layout_region_rect(placement);
    descriptor.default_width = bounds.width;
    descriptor.default_height = bounds.height;
    return umi_ui_window_catalogue_register(
        &customisation->windows, &descriptor);
}

/* Infer the linked subject carried by a context group from portable panel
 * metadata. The result improves routing hints without imposing product code. */
static UmiUiWindowContextKind panel_context_kind(
    const UmiExperiencePanelDefinition *panel)
{
    const char *group_id;
    const char *capability;

    if (panel == NULL) {
        return UMI_UI_WINDOW_CONTEXT_GENERIC;
    }
    group_id = panel->context_group_id != NULL ? panel->context_group_id : "";
    capability = panel->required_capability != NULL
        ? panel->required_capability
        : "";
    if (strstr(group_id, "trading") != NULL ||
        strstr(group_id, "research") != NULL ||
        strstr(capability, "trading") != NULL) {
        return UMI_UI_WINDOW_CONTEXT_INSTRUMENT;
    }
    if (strstr(group_id, "development") != NULL ||
        strstr(capability, "workspace") != NULL ||
        strstr(capability, "editor") != NULL) {
        return UMI_UI_WINDOW_CONTEXT_PROJECT;
    }
    if (strstr(group_id, "operations") != NULL ||
        strstr(capability, "debug") != NULL) {
        return UMI_UI_WINDOW_CONTEXT_RUN;
    }
    if (strstr(group_id, "data") != NULL ||
        strstr(capability, "data") != NULL) {
        return UMI_UI_WINDOW_CONTEXT_DOCUMENT;
    }
    return UMI_UI_WINDOW_CONTEXT_GENERIC;
}

/* Define every linked context once and register each stable panel identity as
 * a bidirectional participant. Repeated panel IDs across layouts are updated,
 * not duplicated, by the group assignment service. */
static UmiStatus seed_context_groups(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiApplicationExperienceDefinition *experience)
{
    size_t panel_index;

    if (customisation == NULL || experience == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (panel_index = 0U; panel_index < experience->panel_count;
         ++panel_index) {
        const UmiExperiencePanelDefinition *panel =
            &experience->panels[panel_index];
        UmiStatus status;

        /* Panels without the linked-context flag remain independent even if a
         * future definition happens to carry descriptive context text. */
        if ((panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) == 0U ||
            panel->context_group_id == NULL ||
            panel->context_group_id[0] == '\0') {
            continue;
        }
        if (umi_ui_window_group_find(
                &customisation->groups, panel->context_group_id) == NULL) {
            status = umi_ui_window_group_define(
                &customisation->groups,
                panel->context_group_id,
                panel->context_group_id,
                panel_context_kind(panel));
            if (status != UMI_STATUS_OK) {
                return status;
            }
        }
        status = umi_ui_window_group_assign(
            &customisation->groups,
            panel->context_group_id,
            panel->panel_id,
            UMI_UI_WINDOW_GROUP_BIDIRECTIONAL);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_suite_customisation_load_experience(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiApplicationExperienceDefinition *experience)
{
    UmiUiWorkspaceCustomisation *candidate;
    UmiUiWorkspaceLayout *projected;
    char default_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    size_t index;
    UmiStatus status;

    if (customisation == NULL || experience == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_experience_validate(experience);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Heap allocations avoid large Windows stack frames.  Candidate state is
     * published only after every panel and layout has succeeded. */
    candidate = (UmiUiWorkspaceCustomisation *)calloc(1U, sizeof(*candidate));
    projected = (UmiUiWorkspaceLayout *)calloc(1U, sizeof(*projected));
    if (candidate == NULL || projected == NULL) {
        free(projected);
        free(candidate);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_ui_workspace_customisation_init(candidate);

    for (index = 0U; index < experience->panel_count; ++index) {
        status = register_panel(candidate, &experience->panels[index]);
        if (status != UMI_STATUS_OK) {
            free(projected);
            free(candidate);
            return status;
        }
    }
    status = seed_context_groups(candidate, experience);
    if (status != UMI_STATUS_OK) {
        free(projected);
        free(candidate);
        return status;
    }
    for (index = 0U; index < experience->layout_count; ++index) {
        status = umi_application_suite_layout_project(
            experience, experience->layouts[index].layout_id, projected);
        if (status == UMI_STATUS_OK) {
            status = umi_ui_workspace_customisation_add_layout(
                candidate, projected);
        }
        if (status != UMI_STATUS_OK) {
            free(projected);
            free(candidate);
            return status;
        }
    }

    {
        int written = snprintf(
            default_layout_id,
            sizeof(default_layout_id),
            "%s.%s",
            experience->application_id,
            experience->default_layout_id);
        status = written < 0 ||
                 (size_t)written >= sizeof(default_layout_id)
            ? UMI_STATUS_CAPACITY_EXCEEDED
            : UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_customisation_activate(
            candidate, default_layout_id);
    }
    if (status == UMI_STATUS_OK) {
        /* A failure above leaves the caller's previous usable workspace
         * untouched; this assignment is the only publication point. */
        *customisation = *candidate;
    }
    free(projected);
    free(candidate);
    return status;
}

UmiStatus umi_application_suite_customisation_load_application(
    UmiUiWorkspaceCustomisation *customisation,
    const char *application_id)
{
    const UmiApplicationExperienceDefinition *experience;

    if (customisation == NULL || application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    experience = umi_application_experience_catalogue_find(application_id);
    return experience != NULL
        ? umi_application_suite_customisation_load_experience(
              customisation, experience)
        : UMI_STATUS_NOT_FOUND;
}

/* Find the canonical experience which owns the active qualified layout. */
static const UmiApplicationExperienceDefinition *active_experience(
    const UmiUiWorkspaceCustomisation *customisation)
{
    const UmiApplicationExperienceDefinition *best = NULL;
    size_t best_length = 0U;
    size_t index;

    if (customisation == NULL) {
        return NULL;
    }
    for (index = 0U;
         index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *candidate =
            umi_application_experience_catalogue_at(index);
        size_t length;

        if (candidate == NULL || candidate->application_id == NULL) {
            continue;
        }
        length = strlen(candidate->application_id);
        if (length < sizeof(customisation->active_layout_id) &&
            length > best_length &&
            strncmp(customisation->active_layout_id,
                    candidate->application_id,
                    length) == 0 &&
            customisation->active_layout_id[length] == '.') {
            best = candidate;
            best_length = length;
        }
    }
    return best;
}

/* Resolve panel policy from the active experience and window tool ID. */
static const UmiExperiencePanelDefinition *active_panel(
    const UmiUiWorkspaceCustomisation *customisation,
    const char *window_id)
{
    const UmiApplicationExperienceDefinition *experience =
        active_experience(customisation);
    const UmiUiWorkspaceLayout *layout =
        umi_ui_workspace_customisation_active_const(customisation);
    const UmiUiWorkspaceWindow *window;

    if (layout == NULL || window_id == NULL) {
        return NULL;
    }
    window = umi_ui_workspace_layout_find_window(layout, window_id);

    return experience != NULL && window != NULL
        ? umi_application_experience_panel_find(experience, window->tool_id)
        : NULL;
}

UmiStatus umi_application_suite_customisation_dock_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *placement_id,
    const char *stack_id)
{
    const UmiExperiencePanelDefinition *panel;

    if (customisation == NULL || window_id == NULL ||
        placement_id == NULL || stack_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = active_panel(customisation, window_id);
    if (panel == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    if ((panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return umi_ui_workspace_customisation_dock_window(
        customisation, window_id, placement_id, stack_id);
}

UmiStatus umi_application_suite_customisation_float_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    double x,
    double y,
    double width,
    double height)
{
    const UmiExperiencePanelDefinition *panel;

    if (customisation == NULL || window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = active_panel(customisation, window_id);
    if (panel == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    if ((panel->flags & UMI_EXPERIENCE_PANEL_FLOATABLE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return umi_ui_workspace_customisation_float_window(
        customisation, window_id, x, y, width, height);
}

UmiStatus umi_application_suite_customisation_set_auto_hidden(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    bool auto_hidden)
{
    const UmiExperiencePanelDefinition *panel;

    if (customisation == NULL || window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = active_panel(customisation, window_id);
    if (panel == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (auto_hidden &&
        (panel->flags & UMI_EXPERIENCE_PANEL_AUTO_HIDE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return umi_ui_workspace_customisation_set_auto_hidden(
        customisation, window_id, auto_hidden);
}

/* Enforce experience capability flags before the base atomic mutation. */
UmiStatus umi_application_suite_customisation_apply_panel_settings(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspacePanelSettings *settings)
{
    const UmiExperiencePanelDefinition *panel;

    if (customisation == NULL || settings == NULL ||
        settings->window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = active_panel(customisation, settings->window_id);
    if (panel == NULL) {
        const UmiUiWorkspaceLayout *layout =
            umi_ui_workspace_customisation_active_const(customisation);
        const UmiUiWorkspaceWindow *window =
            umi_ui_workspace_layout_find_window(layout, settings->window_id);

        /* A user may add a registered cross-application Framework panel which
         * is not part of the product's starting experience. Such a panel keeps
         * the conservative base customisation rules rather than becoming
         * immovable simply because it was not in the default recipe. */
        if (window != NULL &&
            umi_ui_window_catalogue_find(
                &customisation->windows, window->tool_id) != NULL) {
            return umi_ui_workspace_customisation_apply_panel_settings(
                customisation, settings);
        }
        return UMI_STATUS_NOT_FOUND;
    }

    /* Panel flags are capability boundaries. A visible menu must never grant
     * floating, docking, auto-hide or linking behavior the experience denied. */
    if (settings->floating &&
        (panel->flags & UMI_EXPERIENCE_PANEL_FLOATABLE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (!settings->floating &&
        (panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (settings->auto_hidden &&
        (panel->flags & UMI_EXPERIENCE_PANEL_AUTO_HIDE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (settings->context_group_id != NULL &&
        settings->context_group_id[0] != '\0' &&
        (panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return umi_ui_workspace_customisation_apply_panel_settings(
        customisation, settings);
}
