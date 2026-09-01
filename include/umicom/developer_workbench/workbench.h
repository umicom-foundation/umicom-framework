/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/workbench.h
 *
 * PURPOSE:
 *   Compose commands, lifecycle execution, Search Everywhere, Quick Open,
 *   recent projects and perspectives into one reusable developer workbench.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_WORKBENCH_H
#define UMICOM_DEVELOPER_WORKBENCH_WORKBENCH_H

#include "umicom/developer_workbench/builtin_perspectives.h"
#include "umicom/developer_workbench/command_palette.h"
#include "umicom/developer_workbench/command_service.h"
#include "umicom/developer_workbench/project_wizard.h"
#include "umicom/developer_workbench/quick_open.h"
#include "umicom/developer_workbench/recent_project_provider.h"
#include "umicom/developer_workbench/recent_projects.h"
#include "umicom/developer_workbench/search_session.h"
#include "umicom/developer_workbench/start_centre.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperWorkbenchBindings {
    const UmiToolchainProfile *toolchain;
    UmiEnvironmentPlan *environment;
    UmiFileIndex *file_index;
    const UmiWorkspaceTrustStore *trust_store;
} UmiDeveloperWorkbenchBindings;

typedef struct UmiDeveloperWorkbenchSnapshot {
    size_t command_count;
    size_t external_action_count;
    size_t configuration_count;
    size_t search_provider_count;
    size_t recent_project_count;
    size_t perspective_count;
    size_t operation_history_count;
    char active_perspective_id[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    uint64_t revision;
} UmiDeveloperWorkbenchSnapshot;

typedef struct UmiDeveloperWorkbench UmiDeveloperWorkbench;

UmiStatus umi_developer_workbench_create(
    const UmiDeveloperWorkbenchBindings *bindings,
    UmiDeveloperWorkbench **out_workbench);

void umi_developer_workbench_destroy(
    UmiDeveloperWorkbench *workbench);

UmiStatus umi_developer_workbench_add_configuration(
    UmiDeveloperWorkbench *workbench,
    const UmiDeveloperWorkbenchConfiguration *configuration,
    int make_active);

UmiStatus umi_developer_workbench_bind_action(
    UmiDeveloperWorkbench *workbench,
    const char *command_id,
    UmiDeveloperWorkbenchActionHandler handler,
    UmiDeveloperWorkbenchActionEnabled enabled,
    void *user_data);

/**
 * Remove one command callback without removing the command definition.
 *
 * Bridges call this before releasing callback context, preventing a workbench
 * from retaining a pointer to memory that no longer exists.
 */
UmiStatus umi_developer_workbench_unbind_action(
    UmiDeveloperWorkbench *workbench,
    const char *command_id);

UmiStatus umi_developer_workbench_execute_command(
    UmiDeveloperWorkbench *workbench,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

int umi_developer_workbench_command_enabled(
    UmiDeveloperWorkbench *workbench,
    const char *command_id,
    const char *argument);

UmiStatus umi_developer_workbench_search(
    UmiDeveloperWorkbench *workbench,
    const char *query,
    UmiDeveloperWorkbenchSearchSession *session);

UmiStatus umi_developer_workbench_activate_perspective(
    UmiDeveloperWorkbench *workbench,
    const char *perspective_id);

UmiStatus umi_developer_workbench_snapshot(
    UmiDeveloperWorkbench *workbench,
    UmiDeveloperWorkbenchSnapshot *out_snapshot);

UmiCommandRegistry *umi_developer_workbench_commands(
    UmiDeveloperWorkbench *workbench);

UmiDeveloperWorkbenchConfigurationRegistry *
umi_developer_workbench_configurations(
    UmiDeveloperWorkbench *workbench);

UmiRecentItemRegistry *umi_developer_workbench_recent_items(
    UmiDeveloperWorkbench *workbench);

UmiDeveloperWorkbenchPerspectiveRegistry *
umi_developer_workbench_perspectives(
    UmiDeveloperWorkbench *workbench);

UmiDeveloperWorkbenchOperationHistory *
umi_developer_workbench_history(
    UmiDeveloperWorkbench *workbench);

#ifdef __cplusplus
}
#endif

#endif
