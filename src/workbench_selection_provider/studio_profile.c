/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/studio_profile.c
 *
 * PURPOSE:
 *   Register Studio provider descriptors while leaving current colour-group resolution to the Context Host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/studio_profile.h"

UmiWorkbenchSelectionProviderStudioProfile
umi_workbench_selection_provider_studio_profile_default(void)
{
    UmiWorkbenchSelectionProviderStudioProfile profile;
    profile.application_id = "org.umicom.studio";
    profile.project_panel_id = "studio.project-explorer";
    profile.problems_panel_id = "studio.problems";
    profile.source_control_panel_id = "studio.source-control";
    profile.test_panel_id = "studio.test-explorer";
    profile.project_source_id = "studio.project.selection";
    profile.problems_source_id = "studio.problems.selection";
    profile.source_control_source_id = "studio.source-control.selection";
    profile.test_source_id = "studio.test.selection";
    return profile;
}

static UmiStatus register_descriptor(
    UmiWorkbenchSelectionProviderService *service,
    const char *provider_id,
    const char *application_id,
    const char *panel_id,
    const char *display_name,
    const char *source_id,
    UmiWorkbenchSelectionProviderKind provider_kind,
    UmiWorkbenchSelectionKind selection_kind,
    UmiContextKind context_kind)
{
    UmiWorkbenchSelectionProviderDescriptor descriptor;
    UmiStatus status;

    umi_workbench_selection_provider_descriptor_init(
        &descriptor, provider_id);
    status = umi_workbench_selection_provider_descriptor_set_identity(
        &descriptor,
        application_id,
        panel_id,
        display_name);
    if (status != UMI_STATUS_OK) return status;

    /*
     * The group is intentionally empty. Normal model selections follow the
     * panel's current colour-link assignment resolved downstream by Context
     * Host, exactly like the real GTK interaction sources.
     */
    status = umi_workbench_selection_provider_descriptor_set_routing(
        &descriptor,
        source_id,
        "");
    if (status != UMI_STATUS_OK) return status;

    descriptor.kind = provider_kind;
    descriptor.state = UMI_WORKBENCH_SELECTION_PROVIDER_ACTIVE;
    descriptor.selection_kind = selection_kind;
    descriptor.context_kind = context_kind;
    descriptor.enabled = true;

    return umi_workbench_selection_provider_service_register(
        service, &descriptor);
}

UmiStatus umi_workbench_selection_provider_register_studio_profile(
    UmiWorkbenchSelectionProviderService *service,
    const UmiWorkbenchSelectionProviderStudioProfile *profile)
{
    UmiWorkbenchSelectionProviderStudioProfile effective;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    effective = profile != NULL
        ? *profile
        : umi_workbench_selection_provider_studio_profile_default();
    if (effective.application_id == NULL ||
        effective.project_panel_id == NULL ||
        effective.problems_panel_id == NULL ||
        effective.source_control_panel_id == NULL ||
        effective.test_panel_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = register_descriptor(
        service,
        "studio.provider.project",
        effective.application_id,
        effective.project_panel_id,
        "Project Centre Selection",
        effective.project_source_id,
        UMI_WORKBENCH_SELECTION_PROVIDER_PROJECT,
        UMI_WORKBENCH_SELECTION_PROJECT,
        UMI_CONTEXT_KIND_PROJECT);
    if (status != UMI_STATUS_OK) return status;

    status = register_descriptor(
        service,
        "studio.provider.problems",
        effective.application_id,
        effective.problems_panel_id,
        "Problems Selection",
        effective.problems_source_id,
        UMI_WORKBENCH_SELECTION_PROVIDER_PROBLEM,
        UMI_WORKBENCH_SELECTION_DIAGNOSTIC,
        UMI_CONTEXT_KIND_SOURCE_LOCATION);
    if (status != UMI_STATUS_OK) return status;

    status = register_descriptor(
        service,
        "studio.provider.source-control",
        effective.application_id,
        effective.source_control_panel_id,
        "Source Control Selection",
        effective.source_control_source_id,
        UMI_WORKBENCH_SELECTION_PROVIDER_SOURCE_CONTROL_CHANGE,
        UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_CHANGE,
        UMI_CONTEXT_KIND_PROJECT);
    if (status != UMI_STATUS_OK) return status;

    return register_descriptor(
        service,
        "studio.provider.test-explorer",
        effective.application_id,
        effective.test_panel_id,
        "Test Explorer Selection",
        effective.test_source_id,
        UMI_WORKBENCH_SELECTION_PROVIDER_TEST,
        UMI_WORKBENCH_SELECTION_TEST,
        UMI_CONTEXT_KIND_SELECTION);
}
