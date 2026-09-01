/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/projection.c
 *
 * PURPOSE:
 *   Project application panels into tab-group geometry using the existing UI workspace layout contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/projection.h"

#include <stdio.h>
#include <string.h>
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/geometry.h"

/* Copy one required string into bounded workspace storage without truncation. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /* Missing source or writable capacity cannot produce a complete field. */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(destination, capacity, "%s", source);
    /* Identifiers and labels are accepted only when the complete text fits. */
    if (written < 0 || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

/* Materialise one canonical layout as owned, editable workspace records. */
UmiStatus umi_application_suite_layout_project(
    const UmiApplicationExperienceDefinition *experience,
    const char *layout_id,
    UmiUiWorkspaceLayout *out_layout)
{
    const UmiExperienceLayoutDefinition *definition;
    char qualified_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char reason[256U];
    size_t index;
    UmiStatus status;
    int written;
    /* Projection needs canonical metadata, a stable key and writable output. */
    if (experience == NULL || layout_id == NULL || out_layout == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_experience_validate(experience);
    /* Invalid source relationships must not become saved workspace state. */
    if (status != UMI_STATUS_OK) return status;
    definition = umi_application_experience_layout_find(experience, layout_id);
    /* Only layouts owned by the supplied experience can be projected. */
    if (definition == NULL) return UMI_STATUS_NOT_FOUND;
    written = snprintf(qualified_id, sizeof(qualified_id), "%s.%s",
                       experience->application_id, definition->layout_id);
    /* The qualified ID is persisted and therefore may not be shortened. */
    if (written < 0 || (size_t)written >= sizeof(qualified_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_ui_workspace_layout_init(out_layout, qualified_id, definition->title);
    /* Preserve initialisation errors before mutating the new layout. */
    if (status != UMI_STATUS_OK) return status;
    /* Workspace layouts initialise locked. Projection is a controlled build
     * phase, so unlock before adding windows; callers may lock the completed
     * layout afterwards through the existing Framework API. */
    status = umi_ui_workspace_layout_set_locked(out_layout, false);
    if (status != UMI_STATUS_OK) return status;
    /* Each panel recipe becomes exactly one initial workspace window. */
    for (index = 0U; index < definition->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            umi_application_experience_panel_find(experience,
                                                  definition->panel_ids[index]);
        UmiUiWorkspaceWindow window;
        UmiUiPlacement placement;
        UmiApplicationSuiteLayoutRect rect;
        /* A missing panel means the canonical recipe is incomplete. */
        if (panel == NULL) return UMI_STATUS_NOT_FOUND;
        status = umi_ui_placement_parse(panel->default_region, &placement);
        /* Unknown or unsupported regions cannot be rendered consistently. */
        if (status != UMI_STATUS_OK ||
            !umi_application_suite_layout_region_supported(placement))
            return UMI_STATUS_INVALID_STATE;
        rect = umi_application_suite_layout_region_rect(placement);
        (void)memset(&window, 0, sizeof(window));
        status = copy_text(window.window_id, sizeof(window.window_id), panel->panel_id);
        /* Continue copying only while the window remains complete. */
        if (status == UMI_STATUS_OK)
            status = copy_text(window.title, sizeof(window.title), panel->title);
        /* Tool identity links the window instance to its reusable panel. */
        if (status == UMI_STATUS_OK)
            status = copy_text(window.tool_id, sizeof(window.tool_id), panel->panel_id);
        /* The original group field remains populated for saved-layout
         * compatibility while newer code uses placement and stack fields. */
        if (status == UMI_STATUS_OK)
            status = copy_text(window.group_id, sizeof(window.group_id), panel->default_region);
        /* Explicit placement removes ambiguity for render-plan adapters. */
        if (status == UMI_STATUS_OK)
            status = copy_text(window.placement_id,
                               sizeof(window.placement_id),
                               panel->default_region);
        /* Panels sharing a starting region begin in the same tab stack. */
        if (status == UMI_STATUS_OK)
            status = copy_text(window.stack_id, sizeof(window.stack_id),
                               panel->default_region);
        /* Linked context is optional, so an absent group becomes empty text. */
        if (status == UMI_STATUS_OK)
            status = copy_text(
                window.context_group_id,
                sizeof(window.context_group_id),
                panel->context_group_id != NULL
                    ? panel->context_group_id
                    : "");
        /* Never add a partially copied window record. */
        if (status != UMI_STATUS_OK) return status;
        window.x = rect.x;
        window.y = rect.y;
        window.width = rect.width;
        window.height = rect.height;
        window.visible = true;
        window.floating = placement == UMI_UI_PLACEMENT_FLOATING;
        window.maximised = false;
        window.closable = (panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) == 0U;
        window.pinned = (panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U;
        window.resizable = true;
        window.z_order = (int32_t)index;
        status = umi_ui_workspace_layout_add_window(out_layout, &window);
        /* Capacity or validation failure leaves the caller with an error
         * rather than silently dropping a panel. */
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_ui_workspace_layout_validate(
        out_layout, reason, sizeof(reason));
    /* Whole-layout validation catches overlaps and identity problems that
     * cannot be decided while processing one panel. */
    if (status != UMI_STATUS_OK) return status;
    /* A projected layout is published locked.  The shared customisation
     * service opens an explicit edit session before any user mutation. */
    return umi_ui_workspace_layout_set_locked(out_layout, true);
}

/* Resolve an application and delegate to the named projection path. */
UmiStatus umi_application_suite_layout_project_default(
    const char *application_id,
    UmiUiWorkspaceLayout *out_layout)
{
    const UmiApplicationExperienceDefinition *experience;
    /* The catalogue key and caller-owned result are both required. */
    if (application_id == NULL || out_layout == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    experience = umi_application_experience_catalogue_find(application_id);
    /* An unknown application has no default layout to project. */
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    return umi_application_suite_layout_project(
        experience, experience->default_layout_id, out_layout);
}
