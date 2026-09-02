/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/master_authority.c
 *
 * PURPOSE:
 *   Bind desktop layout, window, monitor and context authorities to the Master
 *   Controller. Independent applications communicate through these Framework
 *   capabilities and never hold direct product-to-product dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/master_authority.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/application/master_authority.h"
#include "umicom/runtime/capability_registry.h"

typedef struct UmiDesktopMasterAuthority {
    UmiDesktopRuntime *runtime;
    UmiDesktopShellModel *shell;
    UmiDesktopContentRuntime *content;
    UmiDesktopComponentDragDrop *component_drag_drop;
    UmiDesktopContextSynchronizer *context_synchronizer;
    UmiDesktopLayoutHistory *layout_history;
    UmiDesktopSessionRecovery *session_recovery;
    UmiDesktopMonitorInteraction *monitor_interaction;
} UmiDesktopMasterAuthority;

typedef struct UmiPublishedDesktopCapability {
    const char *capability_id;
    void *provider;
} UmiPublishedDesktopCapability;

/* Provide the desktop authority operation used by this module and its client applications. */
static UmiDesktopMasterAuthority *desktop_authority(
    UmiMasterController *controller)
{
    return (UmiDesktopMasterAuthority *)umi_master_controller_authority(
        controller, UMI_DESKTOP_MASTER_AUTHORITY_ID);
}

/*
 * Provide the destroy desktop authority operation used by this module and its client
 * applications.
 */
static void destroy_desktop_authority(void *value)
{
    UmiDesktopMasterAuthority *authority =
        (UmiDesktopMasterAuthority *)value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (authority == NULL) return;
    umi_desktop_monitor_interaction_destroy(authority->monitor_interaction);
    umi_desktop_session_recovery_destroy(authority->session_recovery);
    umi_desktop_layout_history_destroy(authority->layout_history);
    umi_desktop_context_synchronizer_destroy(authority->context_synchronizer);
    umi_desktop_component_drag_drop_destroy(authority->component_drag_drop);
    umi_desktop_content_runtime_destroy(authority->content);
    umi_desktop_shell_model_destroy(authority->shell);
    umi_desktop_runtime_destroy(authority->runtime);
    free(authority);
}

/*
 * Provide the publish capability operation used by this module and its client
 * applications.
 */
static UmiStatus publish_capability(
    UmiMasterController *controller,
    const UmiPublishedDesktopCapability *capability)
{
    UmiCapabilityDescriptor descriptor;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.capability_id = capability->capability_id;
    descriptor.capability_version = (UmiVersion){0U, 6U, 0U};
    descriptor.provider_module_id = UMI_DESKTOP_MASTER_AUTHORITY_ID;
    descriptor.provider = capability->provider;
    descriptor.flags = UMI_CAPABILITY_SINGLETON | UMI_CAPABILITY_EXTERNAL;
    return umi_capability_registry_register(
        umi_master_controller_capabilities(controller), &descriptor);
}

/*
 * Provide the rollback capabilities operation used by this module and its client
 * applications.
 */
static void rollback_capabilities(
    UmiMasterController *controller,
    const UmiPublishedDesktopCapability *capabilities,
    size_t published_count)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (published_count > 0U) {
        published_count -= 1U;
        (void)umi_capability_registry_unregister(
            umi_master_controller_capabilities(controller),
            capabilities[published_count].capability_id,
            UMI_DESKTOP_MASTER_AUTHORITY_ID);
    }
}

/*
 * Provide the master controller install desktop authority operation used by this module
 * and its client applications.
 */
UmiStatus umi_master_controller_install_desktop_authority(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority;
    UmiPublishedDesktopCapability capabilities[17];
    UmiApplicationContextHub *context_hub;
    UmiFederationRouter *federation;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desktop_authority(controller) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    context_hub = umi_master_controller_application_context(controller);
    federation = umi_master_controller_application_federation(controller);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context_hub == NULL || federation == NULL)
        return UMI_STATUS_INVALID_STATE;
    authority = (UmiDesktopMasterAuthority *)calloc(1U, sizeof(*authority));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (authority == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_desktop_runtime_create(context_hub, &authority->runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_desktop_runtime_seed(authority->runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_shell_model_create(
            authority->runtime, &authority->shell);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_content_runtime_create(
            authority->runtime, federation, &authority->content);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_component_drag_drop_create(
            authority->runtime, authority->content,
            &authority->component_drag_drop);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_context_synchronizer_create(
            authority->runtime, authority->content,
            &authority->context_synchronizer);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_history_create(
            authority->runtime, &authority->layout_history);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_session_recovery_create(
            authority->runtime, &authority->session_recovery);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_monitor_interaction_create(
            authority->runtime, &authority->monitor_interaction);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        destroy_desktop_authority(authority);
        return status;
    }
    capabilities[0] = (UmiPublishedDesktopCapability){
        "umicom.desktop", authority->runtime};
    capabilities[1] = (UmiPublishedDesktopCapability){
        "umicom.desktop.layouts",
        umi_desktop_runtime_layouts(authority->runtime)};
    capabilities[2] = (UmiPublishedDesktopCapability){
        "umicom.desktop.windows",
        umi_desktop_runtime_windows(authority->runtime)};
    capabilities[3] = (UmiPublishedDesktopCapability){
        "umicom.desktop.monitors",
        umi_desktop_runtime_monitors(authority->runtime)};
    capabilities[4] = (UmiPublishedDesktopCapability){
        "umicom.desktop.context",
        umi_desktop_runtime_context_links(authority->runtime)};
    capabilities[5] = (UmiPublishedDesktopCapability){
        "umicom.desktop.persistence", authority->runtime};
    capabilities[6] = (UmiPublishedDesktopCapability){
        "umicom.desktop.shell", authority->shell};
    capabilities[7] = (UmiPublishedDesktopCapability){
        "umicom.desktop.designer",
        umi_desktop_shell_model_designer(authority->shell)};
    capabilities[8] = (UmiPublishedDesktopCapability){
        "umicom.desktop.presentation", authority->shell};
    capabilities[9] = (UmiPublishedDesktopCapability){
        "umicom.desktop.content", authority->content};
    capabilities[10] = (UmiPublishedDesktopCapability){
        "umicom.desktop.component-host",
        umi_desktop_content_runtime_component_host(authority->content)};
    capabilities[11] = (UmiPublishedDesktopCapability){
        "umicom.desktop.view-factories",
        umi_desktop_content_runtime_view_factories(authority->content)};
    capabilities[12] = (UmiPublishedDesktopCapability){
        "umicom.desktop.component-drag-drop", authority->component_drag_drop};
    capabilities[13] = (UmiPublishedDesktopCapability){
        "umicom.desktop.context-synchronizer", authority->context_synchronizer};
    capabilities[14] = (UmiPublishedDesktopCapability){
        "umicom.desktop.layout-history", authority->layout_history};
    capabilities[15] = (UmiPublishedDesktopCapability){
        "umicom.desktop.session-recovery", authority->session_recovery};
    capabilities[16] = (UmiPublishedDesktopCapability){
        "umicom.desktop.monitor-interaction", authority->monitor_interaction};
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(capabilities) / sizeof(capabilities[0]);
         ++index) {
        status = publish_capability(controller, &capabilities[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            rollback_capabilities(controller, capabilities, index);
            destroy_desktop_authority(authority);
            return status;
        }
    }
    status = umi_master_controller_register_authority(
        controller, UMI_DESKTOP_MASTER_AUTHORITY_ID, authority,
        destroy_desktop_authority);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        rollback_capabilities(
            controller, capabilities,
            sizeof(capabilities) / sizeof(capabilities[0]));
        destroy_desktop_authority(authority);
    }
    return status;
}

/*
 * Provide the master controller desktop runtime operation used by this module and its
 * client applications.
 */
UmiDesktopRuntime *umi_master_controller_desktop_runtime(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority = desktop_authority(controller);
    return authority != NULL ? authority->runtime : NULL;
}

/*
 * Provide the master controller desktop shell operation used by this module and its client
 * applications.
 */
UmiDesktopShellModel *umi_master_controller_desktop_shell(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority = desktop_authority(controller);
    return authority != NULL ? authority->shell : NULL;
}

/*
 * Provide the master controller desktop content operation used by this module and its
 * client applications.
 */
UmiDesktopContentRuntime *umi_master_controller_desktop_content(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority = desktop_authority(controller);
    return authority != NULL ? authority->content : NULL;
}

/*
 * Provide the master controller desktop component host operation used by this module and
 * its client applications.
 */
UmiUiComponentHostService *umi_master_controller_desktop_component_host(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority = desktop_authority(controller);
    return authority != NULL
        ? umi_desktop_content_runtime_component_host(authority->content)
        : NULL;
}

/*
 * Provide the master controller desktop component drag drop operation used by this module
 * and its client applications.
 */
UmiDesktopComponentDragDrop *umi_master_controller_desktop_component_drag_drop(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority = desktop_authority(controller);
    return authority != NULL ? authority->component_drag_drop : NULL;
}

/*
 * Provide the master controller desktop context synchronizer operation used by this module
 * and its client applications.
 */
UmiDesktopContextSynchronizer *umi_master_controller_desktop_context_synchronizer(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority = desktop_authority(controller);
    return authority != NULL ? authority->context_synchronizer : NULL;
}

/*
 * Provide the master controller desktop layout history operation used by this module and
 * its client applications.
 */
UmiDesktopLayoutHistory *umi_master_controller_desktop_layout_history(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority = desktop_authority(controller);
    return authority != NULL ? authority->layout_history : NULL;
}

/*
 * Provide the master controller desktop session recovery operation used by this module and
 * its client applications.
 */
UmiDesktopSessionRecovery *umi_master_controller_desktop_session_recovery(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority = desktop_authority(controller);
    return authority != NULL ? authority->session_recovery : NULL;
}

/*
 * Provide the master controller desktop monitor interaction operation used by this module
 * and its client applications.
 */
UmiDesktopMonitorInteraction *umi_master_controller_desktop_monitor_interaction(
    UmiMasterController *controller)
{
    UmiDesktopMasterAuthority *authority = desktop_authority(controller);
    return authority != NULL ? authority->monitor_interaction : NULL;
}
