/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/validation.c
 *
 * PURPOSE:
 *   Prove complete, unique and projectable presentation coverage for every
 *   Framework application component and workspace recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/validation.h"

#include <string.h>

#include "umicom/application/component/recipe_catalogue.h"
#include "umicom/application/component_catalogue.h"
#include "umicom/application/presentation/panel_catalogue.h"
#include "umicom/application/presentation/surface_behavior_catalogue.h"
#include "umicom/application/presentation/workspace_runtime_policy_catalogue.h"

/*
 * Check that application presentation catalogues satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_presentation_catalogues_validate(void)
{
    size_t index;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_application_component_catalogue_validate() != UMI_STATUS_OK ||
        umi_application_presentation_surface_behavior_catalogue_validate() !=
            UMI_STATUS_OK ||
        umi_application_presentation_workspace_runtime_policy_catalogue_validate() !=
            UMI_STATUS_OK ||
        umi_application_presentation_panel_catalogue_count() !=
            umi_application_component_catalogue_count() ||
        umi_application_presentation_window_catalogue_count() !=
            umi_application_component_recipe_catalogue_count()) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_presentation_panel_catalogue_count(); ++index) {
        const UmiApplicationPresentationPanelSpec *panel =
            umi_application_presentation_panel_catalogue_at(index);
        size_t previous;
        UmiStatus status = umi_application_presentation_panel_spec_validate(panel);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Visit each bounded item once so every record receives the same rule. */
        for (previous = 0U; previous < index; ++previous) {
            const UmiApplicationPresentationPanelSpec *earlier =
                umi_application_presentation_panel_catalogue_at(previous);
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(earlier->component_id, panel->component_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_presentation_window_catalogue_count(); ++index) {
        const UmiApplicationPresentationWindowSpec *window =
            umi_application_presentation_window_catalogue_at(index);
        UmiApplicationPresentationPlan plan;
        size_t previous;
        UmiStatus status = umi_application_presentation_window_spec_validate(window);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Visit each bounded item once so every record receives the same rule. */
        for (previous = 0U; previous < index; ++previous) {
            const UmiApplicationPresentationWindowSpec *earlier =
                umi_application_presentation_window_catalogue_at(previous);
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(earlier->recipe_id, window->recipe_id) == 0 ||
                strcmp(earlier->window_id, window->window_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
        status = umi_application_presentation_project(window->recipe_id, &plan);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
