/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/source_control.c
 *
 * PURPOSE:
 *   Preserve Source Control repository IDs, paths, status, branch, commit hash, author, upstream and conflict/staging state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/source_control.h"

#include "umicom/workbench_selection/builders.h"

/* Provide the add text operation used by this module and its client applications. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

/* Provide the add boolean operation used by this module and its client applications. */
static UmiStatus add_boolean(
    UmiWorkbenchSelection *selection,
    const char *name,
    bool value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_boolean(&field, value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

/* Provide the add unsigned operation used by this module and its client applications. */
static UmiStatus add_unsigned(
    UmiWorkbenchSelection *selection,
    const char *name,
    uint64_t value)
{
    UmiWorkbenchSelectionField field;
    UmiStatus status;
    umi_workbench_selection_field_init(&field, name);
    status = umi_workbench_selection_field_set_unsigned(&field, value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_add_field(selection, &field);
}

/*
 * Provide the workbench selection provider from source control change operation used by
 * this module and its client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = add_text(out_selection, "change-id", change->id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "old-uri", change->old_uri);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "staged", change->staged != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "conflict", change->conflict != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_selection->boolean_value = change->staged != 0;
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection provider from source control commit operation used by
 * this module and its client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_set_subject(
        out_selection,
        commit->id,
        commit->hash);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = add_text(out_selection, "repository-id", commit->repository_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "hash", commit->hash);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "author", commit->author);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "email", commit->email);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "subject", commit->subject);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_unsigned(out_selection, "commit-timestamp", commit->timestamp);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "head", commit->head != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the workbench selection provider from source control branch operation used by
 * this module and its client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_selection_set_subject(
        out_selection,
        branch->id,
        branch->name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = add_text(out_selection, "repository-id", branch->repository_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "branch", branch->name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "upstream", branch->upstream);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_text(out_selection, "head", branch->head);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "current", branch->current != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = add_boolean(out_selection, "remote", branch->remote != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_selection->timestamp_ms = timestamp_ms;
    out_selection->state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;
    out_selection->boolean_value = branch->current != 0;
    (void)umi_workbench_selection_set_display_text(
        out_selection, branch->name);
    (void)umi_workbench_selection_refresh_hash(out_selection);
    return UMI_STATUS_OK;
}

/*
 * Find workbench selection provider source control change while leaving the underlying
 * catalogue or model owned by this module.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || change_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_source_control_change_registry_find(
        registry, change_id, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Find workbench selection provider source control commit while leaving the underlying
 * catalogue or model owned by this module.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || commit_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_source_control_commit_registry_find(
        registry, commit_id, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_provider_from_source_control_commit(
        &snapshot,
        application_id,
        panel_id,
        workspace_id,
        timestamp_ms,
        out_selection);
}

/*
 * Find workbench selection provider source control branch while leaving the underlying
 * catalogue or model owned by this module.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || branch_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_source_control_branch_registry_find(
        registry, branch_id, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_workbench_selection_provider_from_source_control_branch(
        &snapshot,
        application_id,
        panel_id,
        workspace_id,
        timestamp_ms,
        out_selection);
}
