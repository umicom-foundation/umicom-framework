/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/platform.h
 *
 * PURPOSE:
 *   Compose project/workbench, Source Control, Terminal, Problems/output,
 *   diff/merge and navigation foundations into one Framework-owned productivity
 *   platform suitable for a thin Umicom Studio application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_PLATFORM_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_PLATFORM_H

#include "umicom/developer_productivity/builtin_source_control_operations.h"
#include "umicom/developer_productivity/builtin_terminal_profiles.h"
#include "umicom/developer_productivity/diff_navigation.h"
#include "umicom/developer_productivity/directory_compare.h"
#include "umicom/developer_productivity/merge.h"
#include "umicom/developer_productivity/unified_diff.h"
#include "umicom/developer_productivity/workbench_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer productivity platform data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProductivityPlatform
    UmiDeveloperProductivityPlatform;

/**
 * Represent the developer productivity platform bindings data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperProductivityPlatformBindings {
    UmiDeveloperWorkbenchBindings workbench;
    UmiClock *clock;
} UmiDeveloperProductivityPlatformBindings;

/**
 * Represent the developer productivity platform snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperProductivityPlatformSnapshot {
    UmiDeveloperProjectWorkbenchPlatformSnapshot project_workbench;
    UmiDeveloperSourceControlSnapshot source_control;
    UmiDeveloperTerminalWorkspaceSnapshot terminal;
    UmiDeveloperProblemStoreSnapshot problems;
    size_t output_channel_count;
    size_t symbol_count;
    int source_control_open;
    int terminal_available;
    uint64_t revision;
} UmiDeveloperProductivityPlatformSnapshot;

/**
 * Initialise developer productivity platform from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_productivity_platform_create(
    const UmiDeveloperProductivityPlatformBindings *bindings,
    UmiDeveloperProductivityPlatform **out_platform);

/**
 * Release or reset state held by developer productivity platform so the same storage can
 * be reused safely.
 */
void umi_developer_productivity_platform_destroy(
    UmiDeveloperProductivityPlatform *platform);

/**
 * Provide the developer productivity platform open git operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_productivity_platform_open_git(
    UmiDeveloperProductivityPlatform *platform,
    const char *root);

/**
 * Provide the developer productivity platform close source control operation used by this
 * module and its client applications.
 */
void umi_developer_productivity_platform_close_source_control(
    UmiDeveloperProductivityPlatform *platform);

/**
 * Provide the developer productivity platform ingest output operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_productivity_platform_ingest_output(
    UmiDeveloperProductivityPlatform *platform,
    const char *channel_id,
    const char *channel_title,
    const char *text,
    size_t *out_problems_added);

/**
 * Provide the developer productivity platform snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_productivity_platform_snapshot(
    UmiDeveloperProductivityPlatform *platform,
    UmiDeveloperProductivityPlatformSnapshot *out_snapshot);

/**
 * Provide the developer productivity platform project workbench operation used by this
 * module and its client applications.
 */
UmiDeveloperProjectWorkbenchPlatform *
umi_developer_productivity_platform_project_workbench(
    UmiDeveloperProductivityPlatform *platform);

/**
 * Provide the developer productivity platform diagnostics operation used by this module
 * and its client applications.
 */
UmiDeveloperDiagnosticPipeline *
umi_developer_productivity_platform_diagnostics(
    UmiDeveloperProductivityPlatform *platform);

/**
 * Provide the developer productivity platform navigation operation used by this module and
 * its client applications.
 */
UmiDeveloperNavigationService *
umi_developer_productivity_platform_navigation(
    UmiDeveloperProductivityPlatform *platform);

/**
 * Provide the developer productivity platform output operation used by this module and its
 * client applications.
 */
UmiDeveloperOutputChannels *
umi_developer_productivity_platform_output(
    UmiDeveloperProductivityPlatform *platform);

/**
 * Provide the developer productivity platform source control operation used by this module
 * and its client applications.
 */
UmiDeveloperSourceControl *
umi_developer_productivity_platform_source_control(
    UmiDeveloperProductivityPlatform *platform);

/**
 * Provide the developer productivity platform terminal operation used by this module and
 * its client applications.
 */
UmiDeveloperTerminalWorkspace *
umi_developer_productivity_platform_terminal(
    UmiDeveloperProductivityPlatform *platform);

#ifdef __cplusplus
}
#endif

#endif
