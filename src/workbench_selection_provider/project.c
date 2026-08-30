/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/project.c
 *
 * PURPOSE:
 *   Preserve project IDs, root URI, active configuration, target and language in structured project selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/project.h"

#include "umicom/workbench_selection/builders.h"

static UmiStatus add_text(
    UmiWorkbenchSelection *selection,
    const char *name,
    const char *value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_text(
        &field, value != NULL ? value : "");
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

UmiStatus umi_workbench_selection_provider_from_project_descriptor(
    const UmiProjectDescriptorSnapshot *project,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    char selection_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    UmiStatus status;
    if (project == NULL || application_id == NULL ||
        panel_id == NULL || out_selection == NULL ||
        project->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_selection_provider_make_id(
        selection_id,
        sizeof(selection_id),
        "project",
        project->id,
        project->revision);
    if (status != UMI_STATUS_OK) return status;

    status = umi_workbench_selection_build_project(
        out_selection,
        selection_id,
        application_id,
        panel_id,
        workspace_id,
        project->id,
        project->root_uri,
        "",
        project->primary_language,
        timestamp_ms);
    if (status != UMI_STATUS_OK) return status;

    status = add_text(out_selection, "project-name", project->name);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "project-kind", project->kind);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "version", project->version);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "description", project->description);
    if (status != UMI_STATUS_OK) return status;

    out_selection->boolean_value = project->enabled != 0;
    (void)umi_workbench_selection_set_display_text(
        out_selection,
        project->name[0] != '\0' ? project->name : project->id);
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_selection_provider_from_project_selection(
    const UmiProjectWorkspaceSelectionSnapshot *snapshot,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    UmiStatus status;
    if (snapshot == NULL || application_id == NULL ||
        panel_id == NULL || out_selection == NULL ||
        snapshot->project.id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_selection_provider_from_project_descriptor(
        &snapshot->project,
        application_id,
        panel_id,
        workspace_id,
        timestamp_ms,
        out_selection);
    if (status != UMI_STATUS_OK) return status;

    if (snapshot->has_configuration) {
        status = add_text(
            out_selection,
            "configuration-id",
            snapshot->configuration.id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (snapshot->has_target) {
        status = add_text(
            out_selection,
            "target-id",
            snapshot->target.id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (snapshot->has_task) {
        status = add_text(
            out_selection,
            "task-id",
            snapshot->task.id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (snapshot->has_launch_profile) {
        status = add_text(
            out_selection,
            "launch-profile-id",
            snapshot->launch_profile.id);
        if (status != UMI_STATUS_OK) return status;
    }
    if (snapshot->has_environment) {
        status = add_text(
            out_selection,
            "environment-id",
            snapshot->environment.id);
        if (status != UMI_STATUS_OK) return status;
    }

    out_selection->state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}
