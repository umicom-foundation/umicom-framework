/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/platform.h
 *
 * PURPOSE:
 *   Compose canonical Framework developer services into one reusable IDE
 *   integration platform for thin IDE applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_PLATFORM_H
#define UMICOM_IDE_INTEGRATION_PLATFORM_H

#include "umicom/ide_integration/builtin_surfaces.h"
#include "umicom/ide_integration/cross_navigation.h"
#include "umicom/ide_integration/inline_controller.h"
#include "umicom/ide_integration/workflow_service.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ide integration platform snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiIdeIntegrationPlatformSnapshot {
    UmiIdeActiveContext context;
    UmiIdeWorkflowReport workflow;
    UmiIdeInlineSuggestion inline_suggestion;
    size_t surface_count;
    size_t navigation_history_count;
    uint64_t revision;
    int inline_ready;
} UmiIdeIntegrationPlatformSnapshot;

/**
 * Represent the ide integration platform data shared with callers of this public contract.
 */
typedef struct UmiIdeIntegrationPlatform UmiIdeIntegrationPlatform;

/**
 * Initialise ide integration platform from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ide_integration_platform_create(
    const char *workspace_root,
    const UmiIdeIntegrationBindings *bindings,
    UmiIdeIntegrationPlatform **out_platform);

/**
 * Release or reset state held by ide integration platform so the same storage can be
 * reused safely.
 */
void umi_ide_integration_platform_destroy(
    UmiIdeIntegrationPlatform *platform);

/**
 * Provide the ide integration platform enable inline ai operation used by this module and
 * its client applications.
 */
UmiStatus umi_ide_integration_platform_enable_inline_ai(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeInlineExecutor *executor,
    const UmiIdeEditorEditAdapter *edits);

/**
 * Provide the ide integration platform set debug resolver operation used by this module
 * and its client applications.
 */
UmiStatus umi_ide_integration_platform_set_debug_resolver(
    UmiIdeIntegrationPlatform *platform,
    UmiIdeDebugFrameLocationResolver resolver,
    void *user_data);

/**
 * Provide the ide integration platform set workflow policy operation used by this module
 * and its client applications.
 */
UmiStatus umi_ide_integration_platform_set_workflow_policy(
    UmiIdeIntegrationPlatform *platform,
    const UmiIdeWorkflowPolicy *policy);

/**
 * Provide the ide integration platform refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_integration_platform_refresh(
    UmiIdeIntegrationPlatform *platform);

/**
 * Provide the ide integration platform snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_integration_platform_snapshot(
    UmiIdeIntegrationPlatform *platform,
    UmiIdeIntegrationPlatformSnapshot *out_snapshot);

/**
 * Provide the ide integration platform bindings operation used by this module and its
 * client applications.
 */
UmiIdeIntegrationBindings *umi_ide_integration_platform_bindings(
    UmiIdeIntegrationPlatform *platform);

/**
 * Provide the ide integration platform navigation operation used by this module and its
 * client applications.
 */
UmiIdeCrossNavigation *umi_ide_integration_platform_navigation(
    UmiIdeIntegrationPlatform *platform);

/**
 * Provide the ide integration platform inline operation used by this module and its client
 * applications.
 */
UmiIdeInlineController *umi_ide_integration_platform_inline(
    UmiIdeIntegrationPlatform *platform);

/**
 * Provide the ide integration platform workflow operation used by this module and its
 * client applications.
 */
UmiIdeWorkflowService *umi_ide_integration_platform_workflow(
    UmiIdeIntegrationPlatform *platform);

/**
 * Provide the ide integration platform surfaces operation used by this module and its
 * client applications.
 */
UmiIdeSurfaceRegistry *umi_ide_integration_platform_surfaces(
    UmiIdeIntegrationPlatform *platform);

/**
 * Provide the ide integration platform workspace root operation used by this module and
 * its client applications.
 */
const char *umi_ide_integration_platform_workspace_root(
    const UmiIdeIntegrationPlatform *platform);

/**
 * @brief Create IDE services for a welcome screen before a workspace is selected.
 * @param bindings Borrowed canonical services; follow the normal platform lifetime.
 * @param out_platform Receives an owned platform; destroy with the normal destructor.
 * @return Initialization status. No path is inferred and no workspace is opened.
 */
UmiStatus umi_ide_integration_platform_create_closed(
    const UmiIdeIntegrationBindings *bindings, UmiIdeIntegrationPlatform **out_platform);
/**
 * @brief Synchronize the identity of an already opened or closed workspace.
 * @param platform Initialized owner-thread platform.
 * @param workspace_root Copied path, or an empty string to record closure.
 * @return OK on publication; failure retains the previous root and readiness.
 * @details The caller remains responsible for opening, closing and trusting the
 * authoritative workspace. This operation supplies identity to IDE surfaces.
 */
UmiStatus umi_ide_integration_platform_set_workspace(
    UmiIdeIntegrationPlatform *platform, const char *workspace_root);
#ifdef __cplusplus
}
#endif
#endif
