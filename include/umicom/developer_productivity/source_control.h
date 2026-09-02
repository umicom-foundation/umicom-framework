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

/**
 * Represent the developer source control data shared with callers of this public contract.
 */
typedef struct UmiDeveloperSourceControl UmiDeveloperSourceControl;

/**
 * Represent the developer source control snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Provide the developer source control create git operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_source_control_create_git(
    const char *root,
    UmiDeveloperSourceControl **out_controller);

/**
 * Initialise developer source control from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_source_control_create(
    const char *root,
    const UmiVcsProvider *provider,
    UmiDeveloperSourceControl **out_controller);

/**
 * Release or reset state held by developer source control so the same storage can be
 * reused safely.
 */
void umi_developer_source_control_destroy(
    UmiDeveloperSourceControl *controller);

/**
 * Provide the developer source control refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_source_control_refresh(
    UmiDeveloperSourceControl *controller);

/**
 * Provide the developer source control stage operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_source_control_stage(
    UmiDeveloperSourceControl *controller,
    const char *path);

/**
 * Provide the developer source control unstage operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_source_control_unstage(
    UmiDeveloperSourceControl *controller,
    const char *path);

/**
 * Provide the developer source control stage all operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_source_control_stage_all(
    UmiDeveloperSourceControl *controller);

/**
 * Provide the developer source control discard operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_source_control_discard(
    UmiDeveloperSourceControl *controller,
    const char *path);

/**
 * Provide the developer source control commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_source_control_commit(
    UmiDeveloperSourceControl *controller,
    const char *message,
    char *out_commit_id,
    size_t capacity);

/**
 * Provide the developer source control fetch operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_source_control_fetch(
    UmiDeveloperSourceControl *controller);

/**
 * Provide the developer source control pull operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_source_control_pull(
    UmiDeveloperSourceControl *controller);

/**
 * Provide the developer source control push operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_source_control_push(
    UmiDeveloperSourceControl *controller);

/**
 * Initialise developer source control branch from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_source_control_branch_create(
    UmiDeveloperSourceControl *controller,
    const char *name,
    int checkout);

/**
 * Provide the developer source control branch checkout operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_source_control_branch_checkout(
    UmiDeveloperSourceControl *controller,
    const char *name);

/**
 * Provide the developer source control branch delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_source_control_branch_delete(
    UmiDeveloperSourceControl *controller,
    const char *name,
    int force);

/**
 * Provide the developer source control diff operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_source_control_diff(
    UmiDeveloperSourceControl *controller,
    const char *path,
    int staged,
    char *out_text,
    size_t capacity);

/**
 * Provide the developer source control snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_source_control_snapshot(
    const UmiDeveloperSourceControl *controller,
    UmiDeveloperSourceControlSnapshot *out_snapshot);

/**
 * Provide the developer source control changes operation used by this module and its
 * client applications.
 */
const UmiVcsChangeList *umi_developer_source_control_changes(
    const UmiDeveloperSourceControl *controller);

#ifdef __cplusplus
}
#endif

#endif
