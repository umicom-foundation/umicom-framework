/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/source_control.c
 *
 * PURPOSE:
 *   Preserve Source Control repository IDs, paths, status, branch, commit hash, author, upstream and conflict/staging state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/source_control.h"

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

static UmiStatus add_boolean(
    UmiWorkbenchSelection *selection,
    const char *name,
    bool value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_boolean(&field, value);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

static UmiStatus add_unsigned(
    UmiWorkbenchSelection *selection,
    const char *name,
    uint64_t value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_unsigned(&field, value);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

UmiStatus umi_workbench_selection_provider_from_source_control_change(
    const UmiSourceControlChangeSnapshot *change,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *workspace_root,
    const char *branch,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    char selection_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    UmiStatus status;
    if (change == NULL || application_id == NULL ||
        panel_id == NULL || out_selection == NULL ||
        change->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_selection_provider_make_id(
        selection_id,
        sizeof(selection_id),
        "vcs-change",
        change->id,
        change->revision);
    if (status != UMI_STATUS_OK) return status;

    status = umi_workbench_selection_build_source_control_change(
        out_selection,
        selection_id,
        application_id,
        panel_id,
        workspace_id,
        change->repository_id[0] != '\0'
            ? change->repository_id : "repository",
        workspace_root != NULL ? workspace_root : "",
        change->repository_id,
        branch != NULL ? branch : "",
        change->uri,
        change->status,
        timestamp_ms);
    if (status != UMI_STATUS_OK) return status;

    status = add_text(out_selection, "change-id", change->id);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "old-uri", change->old_uri);
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "staged", change->staged != 0);
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "conflict", change->conflict != 0);
    if (status != UMI_STATUS_OK) return status;

    out_selection->boolean_value = change->staged != 0;
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_selection_provider_from_source_control_commit(
    const UmiSourceControlCommitSnapshot *commit,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    char selection_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    UmiStatus status;
    if (commit == NULL || application_id == NULL ||
        panel_id == NULL || out_selection == NULL ||
        commit->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_selection_provider_make_id(
        selection_id,
        sizeof(selection_id),
        "vcs-commit",
        commit->id,
        commit->revision);
    if (status != UMI_STATUS_OK) return status;

    umi_workbench_selection_init(
        out_selection,
        UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_COMMIT,
        UMI_CONTEXT_KIND_PROJECT,
        selection_id);
    status = umi_workbench_selection_set_origin(
        out_selection,
        application_id,
        panel_id,
        workspace_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_set_subject(
        out_selection,
        commit->id,
        commit->hash);
    if (status != UMI_STATUS_OK) return status;

    status = add_text(out_selection, "repository-id", commit->repository_id);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "hash", commit->hash);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "author", commit->author);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "email", commit->email);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "subject", commit->subject);
    if (status != UMI_STATUS_OK) return status;
    status = add_unsigned(out_selection, "commit-timestamp", commit->timestamp);
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "head", commit->head != 0);
    if (status != UMI_STATUS_OK) return status;

    out_selection->timestamp_ms = timestamp_ms;
    out_selection->state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;
    out_selection->boolean_value = commit->head != 0;
    (void)umi_workbench_selection_set_display_text(
        out_selection,
        commit->subject[0] != '\0' ? commit->subject : commit->hash);
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_selection_provider_from_source_control_branch(
    const UmiSourceControlBranchSnapshot *branch,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    char selection_id[UMI_WORKBENCH_SELECTION_ID_CAPACITY];
    UmiStatus status;
    if (branch == NULL || application_id == NULL ||
        panel_id == NULL || out_selection == NULL ||
        branch->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_selection_provider_make_id(
        selection_id,
        sizeof(selection_id),
        "vcs-branch",
        branch->id,
        branch->revision);
    if (status != UMI_STATUS_OK) return status;

    umi_workbench_selection_init(
        out_selection,
        UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_BRANCH,
        UMI_CONTEXT_KIND_PROJECT,
        selection_id);
    status = umi_workbench_selection_set_origin(
        out_selection,
        application_id,
        panel_id,
        workspace_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_set_subject(
        out_selection,
        branch->id,
        branch->name);
    if (status != UMI_STATUS_OK) return status;

    status = add_text(out_selection, "repository-id", branch->repository_id);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "branch", branch->name);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "upstream", branch->upstream);
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "head", branch->head);
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "current", branch->current != 0);
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "remote", branch->remote != 0);
    if (status != UMI_STATUS_OK) return status;

    out_selection->timestamp_ms = timestamp_ms;
    out_selection->state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;
    out_selection->boolean_value = branch->current != 0;
    (void)umi_workbench_selection_set_display_text(
        out_selection, branch->name);
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_selection_provider_source_control_change_find(
    const UmiSourceControlChangeRegistry *registry,
    const char *change_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *workspace_root,
    const char *branch,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    UmiSourceControlChangeSnapshot snapshot;
    UmiStatus status;
    if (registry == NULL || change_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_source_control_change_registry_find(
        registry, change_id, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_provider_from_source_control_change(
        &snapshot,
        application_id,
        panel_id,
        workspace_id,
        workspace_root,
        branch,
        timestamp_ms,
        out_selection);
}

UmiStatus umi_workbench_selection_provider_source_control_commit_find(
    const UmiSourceControlCommitRegistry *registry,
    const char *commit_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    UmiSourceControlCommitSnapshot snapshot;
    UmiStatus status;
    if (registry == NULL || commit_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_source_control_commit_registry_find(
        registry, commit_id, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_provider_from_source_control_commit(
        &snapshot,
        application_id,
        panel_id,
        workspace_id,
        timestamp_ms,
        out_selection);
}

UmiStatus umi_workbench_selection_provider_source_control_branch_find(
    const UmiSourceControlBranchRegistry *registry,
    const char *branch_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection)
{
    UmiSourceControlBranchSnapshot snapshot;
    UmiStatus status;
    if (registry == NULL || branch_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_source_control_branch_registry_find(
        registry, branch_id, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_provider_from_source_control_branch(
        &snapshot,
        application_id,
        panel_id,
        workspace_id,
        timestamp_ms,
        out_selection);
}
