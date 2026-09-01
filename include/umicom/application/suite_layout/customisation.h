/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/customisation.h
 *
 * PURPOSE:
 *   Connect application experience panels and layouts to the reusable
 *   Framework workspace customisation service without product-side logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_CUSTOMISATION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_CUSTOMISATION_H

#include "umicom/application/experience.h"
#include "umicom/ui/workspace_customisation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Loads every panel and layout declared by one application experience.
 *
 * @param customisation Existing workspace service that will own the editable
 *        layout state created by this operation.
 * @param experience Borrowed canonical application metadata from Framework.
 * @return `UMI_STATUS_OK` when the complete experience is available for
 *         editing, or a validation, capacity or state error.
 *
 * The caller retains ownership of both arguments. Existing customisation
 * state is replaced only through the workspace service's validated APIs.
 */
UmiStatus umi_application_suite_customisation_load_experience(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiApplicationExperienceDefinition *experience);

/**
 * Resolves an application identifier and loads its Framework-owned experience.
 *
 * @param customisation Existing workspace customisation service.
 * @param application_id Stable identifier stored by the launcher or session.
 * @return `UMI_STATUS_OK` when the application is found and fully loaded.
 *
 * @see umi_application_suite_customisation_load_experience
 */
UmiStatus umi_application_suite_customisation_load_application(
    UmiUiWorkspaceCustomisation *customisation,
    const char *application_id);

/**
 * Docks one workspace window after checking its experience policy.
 *
 * @param customisation Loaded workspace customisation service.
 * @param window_id Stable identifier of the window instance to move.
 * @param placement_id Target region such as `left`, `centre` or `bottom`.
 * @param stack_id Stable tab-group identifier within that region.
 * @return `UMI_STATUS_OK` when the panel allows docking and the move succeeds.
 */
UmiStatus umi_application_suite_customisation_dock_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *placement_id,
    const char *stack_id);

/**
 * Detaches one workspace window into normalized floating geometry.
 *
 * @param customisation Loaded workspace customisation service.
 * @param window_id Stable identifier of the window instance to float.
 * @param x Horizontal start position between zero and one.
 * @param y Vertical start position between zero and one.
 * @param width Window width expressed as a normalized fraction.
 * @param height Window height expressed as a normalized fraction.
 * @return `UMI_STATUS_OK` when floating is allowed and geometry is valid.
 */
UmiStatus umi_application_suite_customisation_float_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    double x,
    double y,
    double width,
    double height);

/**
 * Shows or hides one panel through the shared auto-hide strip.
 *
 * @param customisation Loaded workspace customisation service.
 * @param window_id Stable identifier of the window instance to update.
 * @param auto_hidden `true` to collapse the panel, or `false` to keep it open.
 * @return `UMI_STATUS_OK` when the panel supports the requested behaviour.
 */
UmiStatus umi_application_suite_customisation_set_auto_hidden(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    bool auto_hidden);

/**
 * Applies one complete panel-settings edit as an atomic operation.
 *
 * @param customisation Loaded workspace customisation service.
 * @param settings Borrowed request containing placement, geometry, visibility,
 *        pinning and context-group changes.
 * @return `UMI_STATUS_OK` when every requested change is permitted and the
 *         whole edit is committed; otherwise no partial edit is accepted.
 */
UmiStatus umi_application_suite_customisation_apply_panel_settings(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspacePanelSettings *settings);

#ifdef __cplusplus
}
#endif
#endif
