/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/profile_projection.h
 *
 * PURPOSE:
 *   Project experience layouts into existing workbench workspace-profile
 *   snapshots, with optional thin-product pane identifier resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_PROFILE_PROJECTION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_PROFILE_PROJECTION_H

#include "umicom/application/experience.h"
#include "umicom/ui/workspace_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Lets a thin application map a canonical panel to its composed pane ID.
 *
 * @param panel Borrowed panel definition currently being projected.
 * @param user_data Opaque caller context supplied to the projection function.
 * @return A borrowed, non-empty pane identifier, or `NULL` when the panel
 *         cannot be resolved. The callback owner controls its lifetime.
 */
typedef const char *(*UmiApplicationSuiteLayoutPaneResolver)(
    const UmiExperiencePanelDefinition *panel,
    void *user_data);

/**
 * Projects one canonical layout into a workbench profile without ID mapping.
 *
 * @param experience Borrowed application experience that owns `layout`.
 * @param layout Borrowed canonical layout to project.
 * @param order Display order used by a profile selector.
 * @param out_profile Receives an owned snapshot with copied pane identifiers.
 * @return `UMI_STATUS_OK` when every panel produces a valid profile pane.
 */
UmiStatus umi_application_suite_layout_profile_project(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperienceLayoutDefinition *layout,
    int32_t order,
    UmiUiWorkspaceProfileSnapshot *out_profile);
/**
 * Projects one layout while resolving application composition pane IDs.
 *
 * @param experience Borrowed application experience that owns `layout`.
 * @param layout Borrowed canonical layout to project.
 * @param order Display order used by a profile selector.
 * @param resolver Optional callback that maps panels to composed pane IDs.
 * @param user_data Opaque value passed unchanged to `resolver`.
 * @param out_profile Receives an owned snapshot with copied pane identifiers.
 * @return `UMI_STATUS_OK` when metadata, mappings and profile capacity are
 *         valid; otherwise the first precise failure status.
 *
 * Passing `NULL` as `resolver` keeps each canonical panel identifier.
 */
UmiStatus umi_application_suite_layout_profile_project_resolved(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperienceLayoutDefinition *layout,
    int32_t order,
    UmiApplicationSuiteLayoutPaneResolver resolver,
    void *user_data,
    UmiUiWorkspaceProfileSnapshot *out_profile);

#ifdef __cplusplus
}
#endif
#endif
