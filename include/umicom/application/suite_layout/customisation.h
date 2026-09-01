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

/* Load all layouts and catalogue panels declared by one experience. */
UmiStatus umi_application_suite_customisation_load_experience(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiApplicationExperienceDefinition *experience);

/* Resolve an application ID and load its Framework-owned experience. */
UmiStatus umi_application_suite_customisation_load_application(
    UmiUiWorkspaceCustomisation *customisation,
    const char *application_id);

/* Apply experience policy before docking a panel into a tab stack. */
UmiStatus umi_application_suite_customisation_dock_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    const char *placement_id,
    const char *stack_id);

/* Apply experience policy before detaching a panel into floating geometry. */
UmiStatus umi_application_suite_customisation_float_window(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    double x,
    double y,
    double width,
    double height);

/* Apply experience policy before changing an auto-hide strip. */
UmiStatus umi_application_suite_customisation_set_auto_hidden(
    UmiUiWorkspaceCustomisation *customisation,
    const char *window_id,
    bool auto_hidden);

/* Validate experience flags before applying one atomic panel settings edit. */
UmiStatus umi_application_suite_customisation_apply_panel_settings(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiUiWorkspacePanelSettings *settings);

#ifdef __cplusplus
}
#endif
#endif
