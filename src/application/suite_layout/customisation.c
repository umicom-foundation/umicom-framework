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

    /* Copying requires a real source and writable storage. Returning early
     * prevents a missing label from becoming an apparently valid empty ID. */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    /* A stable ID must fit completely because truncation can make it refer to
     * another tool or saved layout. */
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/* Choose a broad searchable category from portable capability metadata. */
static UmiUiWindowCategory panel_category(
    const UmiExperiencePanelDefinition *panel)
{
    const char *capability;

    /* Missing panel metadata has no safe specialist category. */
    if (panel == NULL) {
        return UMI_UI_WINDOW_CATEGORY_GENERAL;
    }
    capability = panel->required_capability != NULL
        ? panel->required_capability
        : "";
    /* Assistant capabilities are grouped so a New Window menu can keep
     * related knowledge and agent tools together. */
    if (strstr(capability, "ai") != NULL ||
        strstr(panel->panel_id, "assistant") != NULL) {
        return UMI_UI_WINDOW_CATEGORY_AI;
    }
    /* Market-facing tools use the trading category regardless of product. */
    if (strstr(capability, "trading") != NULL ||
        strstr(capability, "market") != NULL) {
        return UMI_UI_WINDOW_CATEGORY_TRADING;
    }
    /* Database and general data tools share one discoverable category. */
    if (strstr(capability, "data") != NULL ||
        strstr(capability, "database") != NULL) {
        return UMI_UI_WINDOW_CATEGORY_DATA;
    }
    /* Project, editing and debugging capabilities form development tools. */
    if (strstr(capability, "project") != NULL ||
        strstr(capability, "editor") != NULL ||
        strstr(capability, "debug") != NULL) {
        return UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    }
    /* Explorer and navigator names describe movement through information. */
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

    /* Registration needs both the destination service and canonical panel. */
    if (customisation == NULL || panel == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&descriptor, 0, sizeof(descriptor));
    status = copy_text(
        descriptor.tool_id, sizeof(descriptor.tool_id), panel->panel_id);
    /* Copy each field only while the descriptor remains complete and valid. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            descriptor.title, sizeof(descriptor.title), panel->title);
    }
    /* The description is optional in metadata, so the title is its fallback. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            descriptor.description,
            sizeof(descriptor.description),
            panel->summary != NULL ? panel->summary : panel->title);
    }
    /* A neutral icon keeps this shared catalogue independent of one product. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            descriptor.icon_name,
            sizeof(descriptor.icon_name),
            "application-panel");
    }
    /* Never register a partly copied catalogue record. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Multiplicity is product metadata, while safe instance naming and layout
     * capacity remain Framework responsibilities. */
    descriptor.category = panel_category(panel);
    descriptor.supports_multiple =
        (panel->flags & UMI_EXPERIENCE_PANEL_MULTI_INSTANCE) != 0U;
    /* Older or incomplete metadata falls back to the main content region;
     * later validation still reports invalid canonical layout recipes. */
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

    /* Missing metadata cannot carry a specialist shared context. */
    if (panel == NULL) {
        return UMI_UI_WINDOW_CONTEXT_GENERIC;
    }
    group_id = panel->context_group_id != NULL ? panel->context_group_id : "";
    capability = panel->required_capability != NULL
        ? panel->required_capability
        : "";
    /* Trading and research panels normally share the selected instrument. */
    if (strstr(group_id, "trading") != NULL ||
        strstr(group_id, "research") != NULL ||
        strstr(capability, "trading") != NULL) {
        return UMI_UI_WINDOW_CONTEXT_INSTRUMENT;
    }
    /* Development panels normally share the active project or workspace. */
    if (strstr(group_id, "development") != NULL ||
        strstr(capability, "workspace") != NULL ||
        strstr(capability, "editor") != NULL) {
        return UMI_UI_WINDOW_CONTEXT_PROJECT;
    }
    /* Operational and debugging panels normally share a run or process. */
    if (strstr(group_id, "operations") != NULL ||
        strstr(capability, "debug") != NULL) {
        return UMI_UI_WINDOW_CONTEXT_RUN;
    }
    /* Data tools normally share the active document or record selection. */
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

    /* Group seeding needs the target service and its canonical source. */
    if (customisation == NULL || experience == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit every panel once; group assignment updates repeated group IDs. */
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
        /* Define each shared group the first time one of its panels appears. */
        if (umi_ui_window_group_find(
                &customisation->groups, panel->context_group_id) == NULL) {
            status = umi_ui_window_group_define(
                &customisation->groups,
                panel->context_group_id,
                panel->context_group_id,
                panel_context_kind(panel));
            /* A group must exist before any participant can join it. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
        }
        status = umi_ui_window_group_assign(
            &customisation->groups,
            panel->context_group_id,
            panel->panel_id,
            UMI_UI_WINDOW_GROUP_BIDIRECTIONAL);
        /* Return assignment failure instead of leaving a partially linked
         * experience that different panels would interpret differently. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

/* Build all editable state in temporary storage and publish it atomically. */
UmiStatus umi_application_suite_customisation_load_experience(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiApplicationExperienceDefinition *experience)
{
    UmiUiWorkspaceCustomisation *candidate;
    UmiUiWorkspaceLayout *projected;
    char default_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    size_t index;
    UmiStatus status;

    /* The caller supplies storage and Framework supplies canonical metadata. */
    if (customisation == NULL || experience == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_experience_validate(experience);
    /* Invalid panel or layout relationships must not replace usable state. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Heap allocations avoid large Windows stack frames.  Candidate state is
     * published only after every panel and layout has succeeded. */
    candidate = (UmiUiWorkspaceCustomisation *)calloc(1U, sizeof(*candidate));
    projected = (UmiUiWorkspaceLayout *)calloc(1U, sizeof(*projected));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (candidate == NULL || projected == NULL) {
        free(projected);
        free(candidate);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_ui_workspace_customisation_init(candidate);

    /* Register all available tools before layouts refer to those tools. */
    for (index = 0U; index < experience->panel_count; ++index) {
        status = register_panel(candidate, &experience->panels[index]);
        /* Release both temporary objects on the first registration failure. */
        if (status != UMI_STATUS_OK) {
            free(projected);
            free(candidate);
            return status;
        }
    }
    status = seed_context_groups(candidate, experience);
    /* Context groups are part of the same atomic load operation. */
    if (status != UMI_STATUS_OK) {
        free(projected);
        free(candidate);
        return status;
    }
    /* Project and add every named starting arrangement, not just the default. */
    for (index = 0U; index < experience->layout_count; ++index) {
        status = umi_application_suite_layout_project(
            experience, experience->layouts[index].layout_id, projected);
        /* Only a complete projection may enter the editable layout store. */
        if (status == UMI_STATUS_OK) {
            status = umi_ui_workspace_customisation_add_layout(
                candidate, projected);
        }
        /* Any failed alternative rejects the candidate as a whole. */
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
    /* Activate the default only after its qualified ID fits completely. */
    if (status == UMI_STATUS_OK) {
        status = umi_ui_workspace_customisation_activate(
            candidate, default_layout_id);
    }
    /* Publish only after the default layout has also activated successfully. */
    if (status == UMI_STATUS_OK) {
        /* A failure above leaves the caller's previous usable workspace
         * untouched; this assignment is the only publication point. */
        *customisation = *candidate;
    }
    free(projected);
    free(candidate);
    return status;
}

/* Resolve an application ID and delegate to the canonical experience loader. */
UmiStatus umi_application_suite_customisation_load_application(
    UmiUiWorkspaceCustomisation *customisation,
    const char *application_id)
{
    const UmiApplicationExperienceDefinition *experience;

    /* Both the target and stable catalogue key are required. */
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

    /* A missing workspace has no active qualified layout to inspect. */
    if (customisation == NULL) {
        return NULL;
    }
    /* Choose the longest matching application prefix so nested IDs remain
     * unambiguous if the catalogue grows. */
    for (index = 0U;
         index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *candidate =
            umi_application_experience_catalogue_at(index);
        size_t length;

        /* Skip unavailable catalogue slots rather than dereferencing them. */
        if (candidate == NULL || candidate->application_id == NULL) {
            continue;
        }
        length = strlen(candidate->application_id);
        /* A match must end at the separator before the layout identifier. */
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

    /* Policy cannot be resolved without active state and an instance key. */
    if (layout == NULL || window_id == NULL) {
        return NULL;
    }
    window = umi_ui_workspace_layout_find_window(layout, window_id);

    return experience != NULL && window != NULL
        ? umi_application_experience_panel_find(experience, window->tool_id)
        : NULL;
}

/* Check canonical panel permissions before delegating the docking mutation. */
UmiStatus umi_application_suite_customisation_dock_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *placement_id,
    const char *stack_id)
{
    const UmiExperiencePanelDefinition *panel;

    /* Docking needs a source instance and complete destination identity. */
    if (customisation == NULL || window_id == NULL ||
        placement_id == NULL || stack_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = active_panel(customisation, window_id);
    /* Unknown windows cannot borrow permissions from another panel. */
    if (panel == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* The experience flag is the reusable capability boundary for docking. */
    if ((panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return umi_ui_workspace_customisation_dock_window(
        customisation, window_id, placement_id, stack_id);
}

/* Check canonical panel permissions before delegating the floating mutation. */
UmiStatus umi_application_suite_customisation_float_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    double x,
    double y,
    double width,
    double height)
{
    const UmiExperiencePanelDefinition *panel;

    /* Floating cannot identify a target without both service and instance. */
    if (customisation == NULL || window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = active_panel(customisation, window_id);
    /* Unknown instances receive no floating permission. */
    if (panel == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Detachment is allowed only when declared by the canonical experience. */
    if ((panel->flags & UMI_EXPERIENCE_PANEL_FLOATABLE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return umi_ui_workspace_customisation_float_window(
        customisation, window_id, x, y, width, height);
}

/* Check canonical panel permissions before changing its auto-hide state. */
UmiStatus umi_application_suite_customisation_set_auto_hidden(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    bool auto_hidden)
{
    const UmiExperiencePanelDefinition *panel;

    /* The requested panel must belong to a live customisation service. */
    if (customisation == NULL || window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = active_panel(customisation, window_id);
    /* Unknown windows cannot opt into an undeclared behaviour. */
    if (panel == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Showing an already visible panel is always safe; hiding requires the
     * explicit auto-hide capability. */
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

    /* Atomic validation needs the service, request and target instance ID. */
    if (customisation == NULL || settings == NULL ||
        settings->window_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    panel = active_panel(customisation, settings->window_id);
    /* Cross-application catalogue tools use conservative shared rules when
     * they are not part of the product's canonical starting experience. */
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
    /* Docking a currently floating panel still requires dockable capability. */
    if (!settings->floating &&
        (panel->flags & UMI_EXPERIENCE_PANEL_DOCKABLE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Auto-hide must be declared explicitly because it changes visibility. */
    if (settings->auto_hidden &&
        (panel->flags & UMI_EXPERIENCE_PANEL_AUTO_HIDE) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* A non-empty link group is accepted only for context-aware panels. */
    if (settings->context_group_id != NULL &&
        settings->context_group_id[0] != '\0' &&
        (panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) == 0U) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return umi_ui_workspace_customisation_apply_panel_settings(
        customisation, settings);
}
