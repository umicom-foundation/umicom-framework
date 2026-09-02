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

/**
 * Represent the developer workbench bindings data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperWorkbenchBindings {
    const UmiToolchainProfile *toolchain;
    UmiEnvironmentPlan *environment;
    UmiFileIndex *file_index;
    const UmiWorkspaceTrustStore *trust_store;
} UmiDeveloperWorkbenchBindings;

/**
 * Represent the developer workbench snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the developer workbench data shared with callers of this public contract.
 */
typedef struct UmiDeveloperWorkbench UmiDeveloperWorkbench;

/**
 * Initialise developer workbench from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_developer_workbench_create(
    const UmiDeveloperWorkbenchBindings *bindings,
    UmiDeveloperWorkbench **out_workbench);

/**
 * Release or reset state held by developer workbench so the same storage can be reused
 * safely.
 */
void umi_developer_workbench_destroy(
    UmiDeveloperWorkbench *workbench);

/**
 * Provide the developer workbench add configuration operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_workbench_add_configuration(
    UmiDeveloperWorkbench *workbench,
    const UmiDeveloperWorkbenchConfiguration *configuration,
    int make_active);

/**
 * Provide the developer workbench bind action operation used by this module and its client
 * applications.
 */
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

/**
 * Provide the developer workbench execute command operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_workbench_execute_command(
    UmiDeveloperWorkbench *workbench,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

/**
 * Provide the developer workbench command enabled operation used by this module and its
 * client applications.
 */
int umi_developer_workbench_command_enabled(
    UmiDeveloperWorkbench *workbench,
    const char *command_id,
    const char *argument);

/**
 * Provide the developer workbench search operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_workbench_search(
    UmiDeveloperWorkbench *workbench,
    const char *query,
    UmiDeveloperWorkbenchSearchSession *session);

/**
 * Provide the developer workbench activate perspective operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_activate_perspective(
    UmiDeveloperWorkbench *workbench,
    const char *perspective_id);

/**
 * Provide the developer workbench snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_workbench_snapshot(
    UmiDeveloperWorkbench *workbench,
    UmiDeveloperWorkbenchSnapshot *out_snapshot);

/**
 * Provide the developer workbench commands operation used by this module and its client
 * applications.
 */
UmiCommandRegistry *umi_developer_workbench_commands(
    UmiDeveloperWorkbench *workbench);

/**
 * Provide the developer workbench configurations operation used by this module and its
 * client applications.
 */
UmiDeveloperWorkbenchConfigurationRegistry *
umi_developer_workbench_configurations(
    UmiDeveloperWorkbench *workbench);

/**
 * Provide the developer workbench recent items operation used by this module and its
 * client applications.
 */
UmiRecentItemRegistry *umi_developer_workbench_recent_items(
    UmiDeveloperWorkbench *workbench);

/**
 * Provide the developer workbench perspectives operation used by this module and its
 * client applications.
 */
UmiDeveloperWorkbenchPerspectiveRegistry *
umi_developer_workbench_perspectives(
    UmiDeveloperWorkbench *workbench);

/**
 * Provide the developer workbench history operation used by this module and its client
 * applications.
 */
UmiDeveloperWorkbenchOperationHistory *
umi_developer_workbench_history(
    UmiDeveloperWorkbench *workbench);

#ifdef __cplusplus
}
#endif

#endif
