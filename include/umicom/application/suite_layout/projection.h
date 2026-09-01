/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/projection.h
 *
 * PURPOSE:
 *   Materialise toolkit-neutral experience layouts as canonical geometric workspace layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_PROJECTION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_PROJECTION_H

#include "umicom/application/experience.h"
#include "umicom/ui/workspace_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Materialises one named experience layout as an editable workspace layout.
 *
 * @param experience Borrowed application metadata that owns the layout.
 * @param layout_id Stable unqualified layout identifier.
 * @param out_layout Receives copied window records and normalized geometry.
 * @return `UMI_STATUS_OK` when every referenced panel can be projected.
 *
 * The result owns its fixed-capacity records and does not retain pointers to
 * temporary caller data.
 */
UmiStatus umi_application_suite_layout_project(
    const UmiApplicationExperienceDefinition *experience,
    const char *layout_id,
    UmiUiWorkspaceLayout *out_layout);
/**
 * Finds one application and projects its canonical default layout.
 *
 * @param application_id Stable application identifier used by the catalogue.
 * @param out_layout Receives copied window records and normalized geometry.
 * @return `UMI_STATUS_OK` when both application and default layout are valid.
 *
 * @see umi_application_suite_layout_project
 */
UmiStatus umi_application_suite_layout_project_default(
    const char *application_id,
    UmiUiWorkspaceLayout *out_layout);

#ifdef __cplusplus
}
#endif
#endif
