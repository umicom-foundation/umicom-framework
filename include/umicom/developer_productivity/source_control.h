/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/source_control.h
 *
 * PURPOSE:
 *   Provide a reusable provider-neutral source-control controller over the
 *   existing Umicom VCS contract. Studio should bind commands to this controller
 *   rather than executing Git directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_SOURCE_CONTROL_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_SOURCE_CONTROL_H

#include "umicom/developer_productivity/types.h"
#include "umicom/vcs/change.h"
#include "umicom/vcs/git_cli.h"
#include "umicom/vcs/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperSourceControl UmiDeveloperSourceControl;

typedef struct UmiDeveloperSourceControlSnapshot {
    char root[UMI_DEVELOPER_PRODUCTIVITY_PATH_CAPACITY];
    char provider_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    UmiVcsBranch branch;
    size_t change_count;
    size_t staged_count;
    uint64_t capabilities;
    uint64_t revision;
    int available;
    UmiStatus last_status;
    char last_message[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY];
} UmiDeveloperSourceControlSnapshot;

UmiStatus umi_developer_source_control_create_git(
    const char *root,
    UmiDeveloperSourceControl **out_controller);

UmiStatus umi_developer_source_control_create(
    const char *root,
    const UmiVcsProvider *provider,
    UmiDeveloperSourceControl **out_controller);

void umi_developer_source_control_destroy(
    UmiDeveloperSourceControl *controller);

UmiStatus umi_developer_source_control_refresh(
    UmiDeveloperSourceControl *controller);

UmiStatus umi_developer_source_control_stage(
    UmiDeveloperSourceControl *controller,
    const char *path);

UmiStatus umi_developer_source_control_unstage(
    UmiDeveloperSourceControl *controller,
    const char *path);

UmiStatus umi_developer_source_control_stage_all(
    UmiDeveloperSourceControl *controller);

UmiStatus umi_developer_source_control_discard(
    UmiDeveloperSourceControl *controller,
    const char *path);

UmiStatus umi_developer_source_control_commit(
    UmiDeveloperSourceControl *controller,
    const char *message,
    char *out_commit_id,
    size_t capacity);

UmiStatus umi_developer_source_control_fetch(
    UmiDeveloperSourceControl *controller);

UmiStatus umi_developer_source_control_pull(
    UmiDeveloperSourceControl *controller);

UmiStatus umi_developer_source_control_push(
    UmiDeveloperSourceControl *controller);

UmiStatus umi_developer_source_control_branch_create(
    UmiDeveloperSourceControl *controller,
    const char *name,
    int checkout);

UmiStatus umi_developer_source_control_branch_checkout(
    UmiDeveloperSourceControl *controller,
    const char *name);

UmiStatus umi_developer_source_control_branch_delete(
    UmiDeveloperSourceControl *controller,
    const char *name,
    int force);

UmiStatus umi_developer_source_control_diff(
    UmiDeveloperSourceControl *controller,
    const char *path,
    int staged,
    char *out_text,
    size_t capacity);

UmiStatus umi_developer_source_control_snapshot(
    const UmiDeveloperSourceControl *controller,
    UmiDeveloperSourceControlSnapshot *out_snapshot);

const UmiVcsChangeList *umi_developer_source_control_changes(
    const UmiDeveloperSourceControl *controller);

#ifdef __cplusplus
}
#endif

#endif
