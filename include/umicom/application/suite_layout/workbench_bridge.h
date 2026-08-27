/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/workbench_bridge.h
 *
 * PURPOSE:
 *   Register canonical application layouts in the authoritative workbench,
 *   optionally resolving product composition pane IDs through a thin callback.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_WORKBENCH_BRIDGE_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_WORKBENCH_BRIDGE_H

#include "umicom/application/experience.h"
#include "umicom/application/suite_layout/profile_projection.h"
#include "umicom/ui/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_application_suite_layout_register_workbench_profiles(
    const UmiApplicationExperienceDefinition *experience,
    UmiUiWorkbench *workbench,
    int activate_default);
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
