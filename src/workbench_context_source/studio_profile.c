/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/studio_profile.c
 *
 * PURPOSE:
 *   Register editor, project, Problems, Source Control, Test Explorer, debugger and AI sources with dynamic panel-group routing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/studio_profile.h"

#include <string.h>

UmiWorkbenchContextSourceStudioProfile
umi_workbench_context_source_studio_profile_default(void)
{
    UmiWorkbenchContextSourceStudioProfile profile;
    profile.application_id = "org.umicom.studio";
    profile.development_group_id = "";
    profile.testing_group_id = "";
    profile.ai_group_id = "";
    profile.data_group_id = "";
    return profile;
}

static UmiStatus register_source(
    UmiWorkbenchContextSourceService *service,
    const UmiWorkbenchContextSourceStudioProfile *profile,
    const char *source_id,
    const char *panel_id,
    const char *display_name,
    UmiWorkbenchContextSourceKind source_kind,
    UmiWorkbenchContextSourceTrigger trigger,
    UmiContextKind context_kind,
    const char *preferred_group,
    uint64_t coalescing_window_ms,
    uint64_t minimum_interval_ms)
{
    UmiWorkbenchContextSourceDefinition definition;
    UmiStatus status;

    umi_workbench_context_source_definition_init(
        &definition, source_id);
    status = umi_workbench_context_source_definition_set_identity(
        &definition,
        profile->application_id,
        panel_id,
        display_name);
    if (status != UMI_STATUS_OK) return status;
    if (preferred_group != NULL && preferred_group[0] != '\0') {
        status = umi_workbench_context_source_definition_set_group(
            &definition,
            preferred_group);
        if (status != UMI_STATUS_OK) return status;
    }
    definition.source_kind = source_kind;
    definition.trigger = trigger;
    definition.context_kind = context_kind;
    definition.accepted_kinds_mask =
        UINT64_C(1) << ((unsigned)context_kind - 1U);
    definition.coalescing_window_ms = coalescing_window_ms;
    definition.minimum_interval_ms = minimum_interval_ms;
    definition.enabled = true;

    return umi_workbench_context_source_service_register(
        service, &definition);
}

UmiStatus umi_workbench_context_source_register_studio_profile(
    UmiWorkbenchContextSourceService *service,
    const UmiWorkbenchContextSourceStudioProfile *profile)
{
    UmiWorkbenchContextSourceStudioProfile effective;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    effective = profile != NULL
        ? *profile
        : umi_workbench_context_source_studio_profile_default();
    if (effective.application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = register_source(
        service,
        &effective,
        "studio.project.selection",
        "studio.project-explorer",
        "Project Explorer selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_PROJECT_EXPLORER,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT,
        UMI_CONTEXT_KIND_PROJECT,
        effective.development_group_id,
        40U,
        0U);
    if (status != UMI_STATUS_OK) return status;

    status = register_source(
        service,
        &effective,
        "studio.editor.location",
        "studio.editor",
        "Editor caret and selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,
        effective.development_group_id,
        35U,
        10U);
    if (status != UMI_STATUS_OK) return status;

    status = register_source(
        service,
        &effective,
        "studio.problems.selection",
        "studio.problems",
        "Problems selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_PROBLEMS,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,
        effective.development_group_id,
        50U,
        0U);
    if (status != UMI_STATUS_OK) return status;

    status = register_source(
        service,
        &effective,
        "studio.source-control.selection",
        "studio.source-control",
        "Source Control selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT,
        UMI_CONTEXT_KIND_PROJECT,
        effective.development_group_id,
        50U,
        0U);
    if (status != UMI_STATUS_OK) return status;

    status = register_source(
        service,
        &effective,
        "studio.test.selection",
        "studio.test-explorer",
        "Test Explorer selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_TEST_EXPLORER,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT,
        UMI_CONTEXT_KIND_SELECTION,
        effective.testing_group_id,
        50U,
        0U);
    if (status != UMI_STATUS_OK) return status;

    status = register_source(
        service,
        &effective,
        "studio.debug.location",
        "studio.debug",
        "Debugger source location",
        UMI_WORKBENCH_CONTEXT_SOURCE_DEBUGGER,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_NAVIGATE,
        UMI_CONTEXT_KIND_SOURCE_LOCATION,
        effective.development_group_id,
        25U,
        0U);
    if (status != UMI_STATUS_OK) return status;

    return register_source(
        service,
        &effective,
        "studio.ai.selection",
        "studio.ai",
        "AI message and evidence selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_AI,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT,
        UMI_CONTEXT_KIND_SELECTION,
        effective.ai_group_id,
        50U,
        0U);
}
