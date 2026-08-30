/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control.c
 *
 * PURPOSE:
 *   Implement real source-control operations through the existing stable VCS
 *   provider contract, retaining status/change evidence for professional views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperSourceControl {
    UmiVcsProvider provider;
    UmiVcsChangeList *changes;
    UmiVcsBranch branch;
    char root[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    uint64_t revision;
    UmiStatus last_status;
    char last_message[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
};

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus finish_operation(UmiDeveloperSourceControl *controller,
                                  UmiStatus status,
                                  const char *message,
                                  int refresh_after)
{
    if (controller == NULL) return status;

    controller->last_status = status;
    (void)snprintf(controller->last_message,
                   sizeof(controller->last_message),
                   "%s",
                   message != NULL ? message : umi_status_text(status));

    if (status == UMI_STATUS_OK && refresh_after) {
        const UmiStatus refresh_status =
            umi_developer_source_control_refresh(controller);

        if (refresh_status != UMI_STATUS_OK) return refresh_status;
    } else {
        controller->revision += 1U;
    }

    return status;
}

UmiStatus umi_developer_source_control_create(
    const char *root,
    const UmiVcsProvider *provider,
    UmiDeveloperSourceControl **out_controller)
{
    UmiDeveloperSourceControl *controller;
    UmiStatus status;

    if (root == NULL || root[0] == '\0' ||
        provider == NULL || out_controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_vcs_provider_validate(provider);
    if (status != UMI_STATUS_OK) return status;

    *out_controller = NULL;
    controller = (UmiDeveloperSourceControl *)calloc(
        1U, sizeof(*controller));
    if (controller == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    controller->provider = *provider;
    controller->revision = 1U;
    controller->last_status = UMI_STATUS_OK;

    status = copy_text(controller->root, sizeof(controller->root), root);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_change_list_create(&controller->changes);
    }

    if (status != UMI_STATUS_OK) {
        umi_developer_source_control_destroy(controller);
        return status;
    }

    *out_controller = controller;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_source_control_create_git(
    const char *root,
    UmiDeveloperSourceControl **out_controller)
{
    UmiVcsProvider provider;
    UmiStatus status;

    if (root == NULL || out_controller == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&provider, 0, sizeof(provider));
    status = umi_vcs_git_cli_provider(&provider);
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_source_control_create(
        root, &provider, out_controller);
    if (status != UMI_STATUS_OK && provider.destroy != NULL) {
        provider.destroy(provider.instance);
    }

    return status;
}

void umi_developer_source_control_destroy(
    UmiDeveloperSourceControl *controller)
{
    if (controller == NULL) return;

    umi_vcs_change_list_destroy(controller->changes);

    if (controller->provider.destroy != NULL) {
        controller->provider.destroy(controller->provider.instance);
    }

    free(controller);
}

UmiStatus umi_developer_source_control_refresh(
    UmiDeveloperSourceControl *controller)
{
    UmiStatus status;

    if (controller == NULL || controller->provider.status == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_vcs_change_list_clear(controller->changes);
    (void)memset(&controller->branch, 0, sizeof(controller->branch));

    status = controller->provider.status(
        controller->provider.instance,
        controller->root,
        controller->changes,
        &controller->branch);

    controller->last_status = status;
    (void)snprintf(
        controller->last_message,
        sizeof(controller->last_message),
        "%s",
        status == UMI_STATUS_OK
            ? "Source-control status refreshed."
            : "Source-control status refresh failed.");
    controller->revision += 1U;
    return status;
}

UmiStatus umi_developer_source_control_stage(
    UmiDeveloperSourceControl *controller,
    const char *path)
{
    if (controller == NULL || path == NULL ||
        controller->provider.stage == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return finish_operation(
        controller,
        controller->provider.stage(
            controller->provider.instance, controller->root, path),
        "Stage completed.",
        1);
}

UmiStatus umi_developer_source_control_unstage(
    UmiDeveloperSourceControl *controller,
    const char *path)
{
    if (controller == NULL || path == NULL ||
        controller->provider.unstage == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return finish_operation(
        controller,
        controller->provider.unstage(
            controller->provider.instance, controller->root, path),
        "Unstage completed.",
        1);
}

UmiStatus umi_developer_source_control_stage_all(
    UmiDeveloperSourceControl *controller)
{
    if (controller == NULL ||
        controller->provider.stage_all == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return finish_operation(
        controller,
        controller->provider.stage_all(
            controller->provider.instance, controller->root),
        "Stage all completed.",
        1);
}

UmiStatus umi_developer_source_control_discard(
    UmiDeveloperSourceControl *controller,
    const char *path)
{
    if (controller == NULL || path == NULL ||
        controller->provider.discard == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return finish_operation(
        controller,
        controller->provider.discard(
            controller->provider.instance, controller->root, path),
        "Discard completed.",
        1);
}

UmiStatus umi_developer_source_control_commit(
    UmiDeveloperSourceControl *controller,
    const char *message,
    char *out_commit_id,
    size_t capacity)
{
    UmiStatus status;

    if (controller == NULL || message == NULL ||
        message[0] == '\0' || out_commit_id == NULL ||
        capacity == 0U || controller->provider.commit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = controller->provider.commit(
        controller->provider.instance,
        controller->root,
        message,
        out_commit_id,
        capacity);

    return finish_operation(controller, status, "Commit completed.", 1);
}

UmiStatus umi_developer_source_control_fetch(
    UmiDeveloperSourceControl *controller)
{
    if (controller == NULL || controller->provider.fetch == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return finish_operation(
        controller,
        controller->provider.fetch(
            controller->provider.instance, controller->root),
        "Fetch completed.",
        1);
}

UmiStatus umi_developer_source_control_pull(
    UmiDeveloperSourceControl *controller)
{
    if (controller == NULL || controller->provider.pull == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return finish_operation(
        controller,
        controller->provider.pull(
            controller->provider.instance, controller->root),
        "Pull completed.",
        1);
}

UmiStatus umi_developer_source_control_push(
    UmiDeveloperSourceControl *controller)
{
    if (controller == NULL || controller->provider.push == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return finish_operation(
        controller,
        controller->provider.push(
            controller->provider.instance, controller->root),
        "Push completed.",
        1);
}

UmiStatus umi_developer_source_control_branch_create(
    UmiDeveloperSourceControl *controller,
    const char *name,
    int checkout)
{
    if (controller == NULL || name == NULL ||
        controller->provider.branch_create == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return finish_operation(
        controller,
        controller->provider.branch_create(
            controller->provider.instance,
            controller->root,
            name,
            checkout),
        "Branch created.",
        1);
}

UmiStatus umi_developer_source_control_branch_checkout(
    UmiDeveloperSourceControl *controller,
    const char *name)
{
    if (controller == NULL || name == NULL ||
        controller->provider.branch_checkout == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return finish_operation(
        controller,
        controller->provider.branch_checkout(
            controller->provider.instance,
            controller->root,
            name),
        "Branch checkout completed.",
        1);
}

UmiStatus umi_developer_source_control_branch_delete(
    UmiDeveloperSourceControl *controller,
    const char *name,
    int force)
{
    if (controller == NULL || name == NULL ||
        controller->provider.branch_delete == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    return finish_operation(
        controller,
        controller->provider.branch_delete(
            controller->provider.instance,
            controller->root,
            name,
            force),
        "Branch deletion completed.",
        1);
}

UmiStatus umi_developer_source_control_diff(
    UmiDeveloperSourceControl *controller,
    const char *path,
    int staged,
    char *out_text,
    size_t capacity)
{
    UmiStatus status;

    if (controller == NULL || path == NULL ||
        out_text == NULL || capacity == 0U ||
        controller->provider.diff == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    status = controller->provider.diff(
        controller->provider.instance,
        controller->root,
        path,
        staged,
        out_text,
        capacity);

    return finish_operation(controller, status, "Diff loaded.", 0);
}

UmiStatus umi_developer_source_control_snapshot(
    const UmiDeveloperSourceControl *controller,
    UmiDeveloperSourceControlSnapshot *out_snapshot)
{
    if (controller == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->root,
                   sizeof(out_snapshot->root),
                   "%s",
                   controller->root);
    (void)snprintf(out_snapshot->provider_id,
                   sizeof(out_snapshot->provider_id),
                   "%s",
                   controller->provider.provider_id != NULL
                       ? controller->provider.provider_id
                       : "");
    out_snapshot->branch = controller->branch;
    out_snapshot->change_count =
        umi_vcs_change_list_count(controller->changes);
    out_snapshot->staged_count =
        umi_vcs_change_list_staged_count(controller->changes);
    out_snapshot->capabilities =
        umi_vcs_provider_capabilities(&controller->provider);
    out_snapshot->revision = controller->revision;
    out_snapshot->available = controller->last_status == UMI_STATUS_OK;
    out_snapshot->last_status = controller->last_status;
    (void)snprintf(out_snapshot->last_message,
                   sizeof(out_snapshot->last_message),
                   "%s",
                   controller->last_message);
    return UMI_STATUS_OK;
}

const UmiVcsChangeList *umi_developer_source_control_changes(
    const UmiDeveloperSourceControl *controller)
{
    return controller != NULL ? controller->changes : NULL;
}
