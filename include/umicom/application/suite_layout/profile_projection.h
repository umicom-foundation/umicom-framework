/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/profile_projection.h
 *
 * PURPOSE:
 *   Project experience layouts into existing workbench workspace-profile snapshots for frontend adoption.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_PROFILE_PROJECTION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_PROFILE_PROJECTION_H

#include "umicom/application/experience.h"
#include "umicom/ui/workspace_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_application_suite_layout_profile_project(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperienceLayoutDefinition *layout,
    int32_t order,
    UmiUiWorkspaceProfileSnapshot *out_profile);

#ifdef __cplusplus
}
#endif
#endif
