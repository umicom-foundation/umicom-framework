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

    /* Current experience panels are singleton surfaces.  A future explicit
     * multiplicity field can change this without application-side code. */
    descriptor.category = panel_category(panel);
    descriptor.supports_multiple = false;
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
