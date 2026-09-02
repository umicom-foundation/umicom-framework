/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/master_authority.c
 *
 * PURPOSE:
 *   Bind reusable application authorities to the Framework Master Controller
 *   while leaving all application and Slave Controller lifecycle decisions
 *   with the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/master_authority.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/application/portfolio.h"
#include "umicom/runtime/capability_registry.h"

typedef struct UmiMasterApplicationAuthority {
    const UmiApplicationDefinition *definition;
    UmiFederationRouter *federation;
    UmiApplicationContextHub *context;
    UmiApplicationResourceBroker *resources;
} UmiMasterApplicationAuthority;

typedef struct UmiPublishedApplicationCapability {
    const char *capability_id;
    void *provider;
} UmiPublishedApplicationCapability;

/*
 * Provide the destroy application authority operation used by this module and its client
 * applications.
 */
static void destroy_application_authority(void *value)
{
    UmiMasterApplicationAuthority *authority =
        (UmiMasterApplicationAuthority *)value;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (authority == NULL) return;
    umi_application_context_hub_destroy(authority->context);
    umi_application_resource_broker_destroy(authority->resources);
    umi_federation_router_destroy(authority->federation);
    free(authority);
}

/*
 * Provide the application authority operation used by this module and its client
 * applications.
 */
static UmiMasterApplicationAuthority *application_authority(
    UmiMasterController *controller)
{
    return (UmiMasterApplicationAuthority *)
        umi_master_controller_authority(
            controller, UMI_APPLICATION_MASTER_AUTHORITY_ID);
}

/*
 * Provide the publish capability operation used by this module and its client
 * applications.
 */
static UmiStatus publish_capability(
    UmiMasterController *controller,
    const UmiPublishedApplicationCapability *capability)
{
    UmiCapabilityDescriptor descriptor;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.capability_id = capability->capability_id;
    descriptor.capability_version = (UmiVersion){0U, 5U, 0U};
    descriptor.provider_module_id = UMI_APPLICATION_MASTER_AUTHORITY_ID;
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
    const UmiPublishedApplicationCapability *capabilities,
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
            UMI_APPLICATION_MASTER_AUTHORITY_ID);
    }
}

/*
 * Provide the master controller install application authority operation used by this
 * module and its client applications.
 */
UmiStatus umi_master_controller_install_application_authority(
    UmiMasterController *controller,
    const char *application_id)
{
    UmiMasterApplicationAuthority *authority;
    UmiPublishedApplicationCapability capabilities[5];
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || application_id == NULL ||
        application_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_authority(controller) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;

    authority = (UmiMasterApplicationAuthority *)calloc(1U,
                                                         sizeof(*authority));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (authority == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    authority->definition = umi_application_portfolio_find(application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (authority->definition == NULL) {
        free(authority);
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_federation_router_create(&authority->federation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_application_context_hub_create(&authority->context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_application_resource_broker_create(&authority->resources);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_federation_router_register_application(
            authority->federation, authority->definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        destroy_application_authority(authority);
        return status;
    }

    capabilities[0] = (UmiPublishedApplicationCapability){
        "umicom.application", authority};
    capabilities[1] = (UmiPublishedApplicationCapability){
        "umicom.application.federation", authority->federation};
    capabilities[2] = (UmiPublishedApplicationCapability){
        "umicom.application.context", authority->context};
    capabilities[3] = (UmiPublishedApplicationCapability){
        "umicom.application.components", authority};
    capabilities[4] = (UmiPublishedApplicationCapability){
        "umicom.application.resources", authority->resources};

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(capabilities) / sizeof(capabilities[0]);
         ++index) {
        status = publish_capability(controller, &capabilities[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            rollback_capabilities(controller, capabilities, index);
            destroy_application_authority(authority);
            return status;
        }
    }

    status = umi_master_controller_register_authority(
        controller, UMI_APPLICATION_MASTER_AUTHORITY_ID, authority,
        destroy_application_authority);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        rollback_capabilities(
            controller, capabilities,
            sizeof(capabilities) / sizeof(capabilities[0]));
        destroy_application_authority(authority);
    }
    return status;
}

/*
 * Provide the master controller attach application operation used by this module and its
 * client applications.
 */
UmiStatus umi_master_controller_attach_application(
    UmiMasterController *controller,
    const UmiApplicationDefinition *application)
{
    UmiMasterApplicationAuthority *authority =
        application_authority(controller);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (authority == NULL) return UMI_STATUS_INVALID_STATE;
    return umi_federation_router_register_application(
        authority->federation, application);
}

/*
 * Provide the master controller application definition operation used by this module and
 * its client applications.
 */
const UmiApplicationDefinition *umi_master_controller_application_definition(
    UmiMasterController *controller)
{
    UmiMasterApplicationAuthority *authority =
        application_authority(controller);
    return authority != NULL ? authority->definition : NULL;
}

/*
 * Provide the master controller application federation operation used by this module and
 * its client applications.
 */
UmiFederationRouter *umi_master_controller_application_federation(
    UmiMasterController *controller)
{
    UmiMasterApplicationAuthority *authority =
        application_authority(controller);
    return authority != NULL ? authority->federation : NULL;
}

/*
 * Provide the master controller application context operation used by this module and its
 * client applications.
 */
UmiApplicationContextHub *umi_master_controller_application_context(
    UmiMasterController *controller)
{
    UmiMasterApplicationAuthority *authority =
        application_authority(controller);
    return authority != NULL ? authority->context : NULL;
}

/*
 * Provide the master controller application resources operation used by this module and
 * its client applications.
 */
UmiApplicationResourceBroker *umi_master_controller_application_resources(
    UmiMasterController *controller)
{
    UmiMasterApplicationAuthority *authority =
        application_authority(controller);
    return authority != NULL ? authority->resources : NULL;
}
