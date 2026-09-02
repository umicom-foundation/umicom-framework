/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/workspace_state.h
 *
 * PURPOSE:
 *   Persist the active developer context and workflow preferences using a
 *   versioned, crash-resistant workspace-state file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This state is deliberately small.  It stores identifiers and preferences,
 * not editor buffers or private service internals.  The file is written with
 * the Framework atomic-file service so an interrupted write cannot leave half
 * of a developer session behind.
 */
#ifndef UMICOM_DEVELOPER_WORKSPACE_STATE_H
#define UMICOM_DEVELOPER_WORKSPACE_STATE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/context.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_WORKSPACE_STATE_API_VERSION 1U
#define UMI_DEVELOPER_WORKSPACE_STATE_CODEC_PREFIX "UDW1"
#define UMI_DEVELOPER_WORKSPACE_STATE_TEXT_CAPACITY 12288U

/**
 * Represent the developer workspace state snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperWorkspaceStateSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDeveloperContextSnapshot context;
    char last_task_id[UMI_DEVELOPER_ID_CAPACITY];
    char last_launch_profile_id[UMI_DEVELOPER_ID_CAPACITY];
    int configure_before_build;
    int build_before_run;
    int test_after_build;
    uint64_t revision;
} UmiDeveloperWorkspaceStateSnapshot;

/**
 * Initialise developer workspace state from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_workspace_state_init(
    UmiDeveloperWorkspaceStateSnapshot *state);

/**
 * Write developer workspace state in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_developer_workspace_state_encode(
    const UmiDeveloperWorkspaceStateSnapshot *state,
    char *out_text,
    size_t capacity,
    size_t *out_length);

/**
 * Read developer workspace state into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_developer_workspace_state_decode(
    const char *text,
    UmiDeveloperWorkspaceStateSnapshot *out_state);

/**
 * Write developer workspace state in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_developer_workspace_state_save(
    const char *path,
    const UmiDeveloperWorkspaceStateSnapshot *state);

/**
 * Read developer workspace state into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_developer_workspace_state_load(
    const char *path,
    UmiDeveloperWorkspaceStateSnapshot *out_state);

#ifdef __cplusplus
}
#endif

#endif
