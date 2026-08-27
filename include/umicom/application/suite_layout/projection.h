/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/projection.h
 *
 * PURPOSE:
 *   Materialise toolkit-neutral experience layouts as canonical geometric workspace layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_PROJECTION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_PROJECTION_H

#include "umicom/application/experience.h"
#include "umicom/ui/workspace_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_application_suite_layout_project(
    const UmiApplicationExperienceDefinition *experience,
    const char *layout_id,
    UmiUiWorkspaceLayout *out_layout);
UmiStatus umi_application_suite_layout_project_default(
    const char *application_id,
    UmiUiWorkspaceLayout *out_layout);

#ifdef __cplusplus
}
#endif
#endif
