/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/workbench_bridge.h
 *
 * PURPOSE:
 *   Register canonical application layouts in the authoritative workbench,
 *   optionally resolving product composition pane IDs through a thin callback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_WORKBENCH_BRIDGE_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_WORKBENCH_BRIDGE_H

#include "umicom/application/experience.h"
#include "umicom/application/suite_layout/profile_projection.h"
#include "umicom/ui/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Registers every canonical application layout as a workbench profile.
 *
 * @param experience Borrowed application experience containing the layouts.
 * @param workbench Existing workbench that owns the resulting profiles.
 * @param activate_default Non-zero to activate the canonical default profile.
 * @return `UMI_STATUS_OK` when every profile is registered successfully.
 */
UmiStatus umi_application_suite_layout_register_workbench_profiles(
    const UmiApplicationExperienceDefinition *experience,
    UmiUiWorkbench *workbench,
    int activate_default);
/**
 * Registers profiles while mapping panels to composed product pane IDs.
 *
 * @param experience Borrowed application experience containing the layouts.
 * @param workbench Existing workbench that owns the resulting profiles.
 * @param activate_default Non-zero to activate the canonical default profile.
 * @param resolver Optional panel-to-pane mapping callback.
 * @param user_data Opaque caller value passed unchanged to `resolver`.
 * @return `UMI_STATUS_OK` when projection, registration and optional
 *         activation all succeed.
 *
 * @see umi_application_suite_layout_profile_project_resolved
 */
UmiStatus umi_application_suite_layout_register_workbench_profiles_resolved(
    const UmiApplicationExperienceDefinition *experience,
    UmiUiWorkbench *workbench,
    int activate_default,
    UmiApplicationSuiteLayoutPaneResolver resolver,
    void *user_data);

#ifdef __cplusplus
}
#endif
#endif
